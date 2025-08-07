#include "./bmc.hpp"
#include <sstream>

namespace bmc {

  constexpr u16 VOXEL_TYPE_MASK = 0b1111111111011111;
  constexpr u8 MIN_LEVEL = OPT_BOOST_LEVEL;
  constexpr u8 MAX_SPACE_LEVEL = 8;
  constexpr u8 MAX_DETAIL_LEVEL = OPT_MAX_DETAIL_LEVEL;
  
  static u32* OFFSETS;
  static u32 OFFSET_CELL_NUMBER;
  static ucb* _buffer;
  static BMC* space = NULL;
  static BMC* stack[10] = {NULL};
  static float COLOR_DEPTH_STEP;
  static float RAY_COLOR_DEPTH_START;

  static u32 BMC_MEMORY_SPACE = 0;
  
  inline void initMemory() {
    u8 level = MAX_SPACE_LEVEL;
    do {
        u32 size = 1 << level;
        BMC_MEMORY_SPACE += size * size * size;
    } while (--level >= MIN_LEVEL);
  }

  Tracer _ = {0};
  
  static ucb* _bmc_memory_space = NULL;
  static inline void fadeToColor(ucb* const buffer, const ucb a, const ucb b, const float fade) {
    Vec3<char> _c;
    _c.x = (a & 0x1F) * fade + (b & 0x1F) * (1.0f - fade);                 // r
    _c.y = ((a >> 5) & 0x3F) * fade + ((b >> 5) & 0x3F) * (1.0f - fade);   // g
    _c.z = ((a >> 11) & 0x1F) * fade + ((b >> 11) & 0x1F) * (1.0f - fade); // b
    *buffer = _c.x | (_c.y << 5) | (_c.z << 11);
  }
  
  static inline void fadeVoxelTo(ucb* const buffer, const ucb color, float fade, const ucb type) {
    Vec3<char> _c;
    if(*buffer & (1 << 5)) {
      return;
    }
    _c.x = (*buffer & 0x1F) * fade + (color & 0x1F) * (1.0f - fade);                 // r
    _c.y = ((*buffer >> 5) & 0x3F) * fade + ((color >> 5) & 0x3F) * (1.0f - fade);   // g
    _c.z = ((*buffer >> 11) & 0x1F) * fade + ((color >> 11) & 0x1F) * (1.0f - fade); // b
    *buffer = (VOXEL_TYPE_MASK & ((_c.x) |
      ((_c.y) << 5) | ((_c.z)) << 11)) | type;
  }

  static inline u32 evalOffset(Trace& __) {
    if(/*_.iray.x < 0 || */__.iray.x >= OPT_SPACE_SIZE ||
      /*_.iray.y < 0 || */__.iray.y >= OPT_SPACE_SIZE ||
      /*_.iray.z < 0 || */__.iray.z >= OPT_SPACE_SIZE) {
      return ((u32)-1);
    }
    return (__.iray.x >> _.bmc->rlevel) |
      ((__.iray.y >> _.bmc->rlevel) << _.bmc->yBitOffset) |
      ((__.iray.z >> _.bmc->rlevel) << _.bmc->zBitOffset);
  }
  
  static inline Vec3<u16> getCoordinates(const u32 offset) {
    return {
      offset & space->sizeDigitsX,
      (offset & space->sizeDigitsY) >> space->yBitOffset,
      (offset & space->sizeDigitsZ) >> space->zBitOffset
    };
  }

  u32 _count = 0;
  static u8 frameLevel;
  static void genSpace(const u8 higher, const u8 lower, BMC* bmc = NULL) {
    u8 level = higher;
    do {
      BMC* const parent = bmc;
      bmc = (BMC*) memalign(16, sizeof(BMC));
      bmc->child = NULL;
      bmc->level = level;
      if(parent != NULL) {
        parent->child = bmc;
      } else {
        space = bmc;
        space->frame_size = 1 << frameLevel;
        space->frame_half = space->frame_size / 2;
        space->frame_yBitOffset = frameLevel;
        space->frame_zBitOffset = frameLevel * 2;
        space->frame_sizeDigitsX = (space->frame_size - 1);
        space->frame_sizeDigitsY = (space->frame_size - 1) << space->frame_yBitOffset;
        space->frame_sizeDigitsZ = (space->frame_size - 1) << space->frame_zBitOffset;
      }
      stack[level] = bmc;
      bmc->size = (u32)(powf(2.0f, (float)level));
      bmc->half = bmc->size / 2;
      bmc->yBitOffset = level;
      bmc->zBitOffset = level * 2;
      bmc->sizeDigitsX = (bmc->size - 1);
      bmc->sizeDigitsY = (bmc->size - 1) << bmc->yBitOffset;
      bmc->sizeDigitsZ = (bmc->size - 1) << bmc->zBitOffset;
      bmc->rlevel = MAX_SPACE_LEVEL - level;
      const float voxSize = (float)(1 << bmc->rlevel);
      bmc->voxInvSize = 1.0f / voxSize;
      bmc->lstep = sqrtf(2.0f * voxSize) * OPT_RAY_STEP_FACTOR;
      bmc->lstep = fmaxf(OPT_RAY_MIN_STEP, bmc->lstep);
      const u32 count = bmc->size * bmc->size * bmc->size; 
      bmc->region = &(((ucb*)_bmc_memory_space)[_count]);
      _count += count;
      
      memset(bmc->region, 0x0, count * sizeof(ucb));
    } while(level-- > lower);
  }
  
  static void fillSpace() {
    constexpr float VOXEL_FADE = 0.1f;
    u32 i = OPT_SPACE_BUFFER_SIZE;
    Trace trace;
    while (i--) {
      _.bmc = space;
      const ucb color = _.bmc->region[i];
      if (color != 0) {
        trace.iray = getCoordinates(i);
        while((_.bmc = _.bmc->child)) {
          const u32 _offset = evalOffset(trace);
          if (_offset != ((u32)-1)) {
            fadeVoxelTo(&_.bmc->region[_offset], color, VOXEL_FADE, 0);
          }
        }
      }
    }
  }

  static void fillSpace(Voxel* const voxels, const u32 count) {
    _.bmc = space;
    u32 i = count;
    Trace trace;
    while(i--) {
      Voxel* const voxel = &voxels[i];
      trace.iray.x = (u16)(voxel->coordinates.x + space->half);
      trace.iray.y = (u16)(voxel->coordinates.y + space->half);
      trace.iray.z = (u16)(voxel->coordinates.z + space->half);
      const u32 _offset = evalOffset(trace);
      if (_offset != ((u32)-1)) {
        if (voxel->color > 0x0) {
          _.bmc->region[_offset] = voxel->color & VOXEL_TYPE_MASK;
        }
      }
    }
  }
  
  static inline ucb getDepthColor(const ucb _color) {
    float _darkness;
    Vec3<char> _c;
    if (_.rayLength < RAY_COLOR_DEPTH_START) {
      return _color;
    } else {
      _darkness = 1.0f - (COLOR_DEPTH_STEP * (_.rayLength - RAY_COLOR_DEPTH_START));
      _darkness = _darkness * _darkness;
    }
    #if OPT_USE_FOG == 1
    float fog = ((0.5f - _darkness));
    fog = (0.25f - (fog * fog));
    _c.x = _darkness * (_color & 0x1F) + fog * OPT_FOG_RED_INTENSITY;
    _c.y = _darkness * ((_color >> 5) & 0x3F) + fog * OPT_FOG_GREEN_INTENSITY;
    _c.z = _darkness * ((_color >> 11) & 0x1F) + fog * OPT_FOG_BLUE_INTENSITY;
    if(_c.x > 0x1F) {
      _c.x = 0x1F;
    }
    if(_c.y > 0x3F) {
      _c.y = 0x3F;
    }
    if(_c.z > 0x1F) {
      _c.z = 0x1F;
    }
    #else
    _c.x = _darkness * (_color & 0x1F);         //r
    _c.y = _darkness * ((_color >> 5) & 0x3F);  //g
    _c.z = _darkness * ((_color >> 11) & 0x1F); //b
    #endif
    return _c.x | (_c.y << 5) | (_c.z << 11);
  }
  
  static v3 _origins[OPT_FRAME_BUFFER_SIZE] __attribute__((aligned(16))) = {0};
  static inline void cacheRayOrigins() {
    constexpr u32 JSTEP = OPT_FRAME_SIZE * OPT_GRID_HEIGHT;
    constexpr u32 H_SCISSOR_START = JSTEP * OPT_H_SCISSOR;
    constexpr u32 H_SCISSOR_END = OPT_FRAME_BUFFER_SIZE - JSTEP * OPT_H_SCISSOR;
    constexpr u32 V_SCISSOR_START = OPT_GRID_WIDTH * OPT_V_SCISSOR_START + OPT_V_DISPLACEMENT;
    constexpr u32 V_SCISSOR_END = OPT_FRAME_SIZE - OPT_GRID_WIDTH * OPT_V_SCISSOR_END;
    
    v3* origin;
    u32 offset = 0;
    u32 j = H_SCISSOR_START;
    while(j < H_SCISSOR_END) {
      u32 i = V_SCISSOR_START;
      while (i < V_SCISSOR_END) {
        offset = i + j;
        origin = &_origins[offset];
        
        origin->x = (
          ((float)(offset & space->frame_sizeDigitsX)
        ) - space->frame_half) * OPT_ORIGIN_SCALE - (OPT_V_DISPLACEMENT/2);
        origin->y = (
          ((float)((offset & space->frame_sizeDigitsY) >> space->frame_yBitOffset)
        ) - space->frame_half) * OPT_ORIGIN_SCALE;
        origin->z = OPT_NEAR_PLANE;

        #if OPT_USE_LENS_DISTORTION == 1
        const float radius = sqrtf(origin->x*origin->x + origin->y*origin->y);
        const float distortion = fmaxf(0.0f, 1.0f - radius * OPT_LENS_DISTORTION_FACTOR);
        origin->x *= distortion;
        origin->y *= distortion;
        
        #elif OPT_USE_SPHERICAL_DISTORTION == 1
        const float dist = sqrtf(origin->x*origin->x + origin->y*origin->y + origin->z*origin->z);
        if (dist > 0.001f) {
            const float distortionFactor = 1.0f + (dist * OPT_SPHERICAL_DISTORTION_FACTOR);
            const v3 distortion = {
                (origin->x / dist) * distortionFactor,
                (origin->y / dist) * distortionFactor,
                (origin->z / dist) * distortionFactor
            };
            
            origin->x = origin->x * (1.0f - OPT_SPHERICAL_DISTORTION_INTENSITY) 
              + distortion.x * OPT_SPHERICAL_DISTORTION_INTENSITY;
            origin->y = origin->y * (1.0f - OPT_SPHERICAL_DISTORTION_INTENSITY)
              + distortion.y * OPT_SPHERICAL_DISTORTION_INTENSITY;
            origin->z = origin->z * (1.0f - OPT_SPHERICAL_DISTORTION_INTENSITY)
              + distortion.z * OPT_SPHERICAL_DISTORTION_INTENSITY;
        }
        #endif
        
        #if OPT_USE_BARREL_DISTORTION == 1
        const float distanceFromCenter = sqrtf(origin->x*origin->x + origin->y*origin->y);
        const float depthFactor = 1.0f - distanceFromCenter * OPT_BARREL_DISTORTION_FACTOR;
        origin->z *= depthFactor;
        #endif
        
        i++;
      }
      j += OPT_FRAME_SIZE;
    }
  }
  
  static inline void calcBase(Trace& trace) {
    _.rayBase = OPT_RAY_DEPTH_SCALE *
                sqrtf(_.rayLength*_.rayLength*_.rayLength) +
                OPT_RAY_BASE; // powf(x, 1.5f)
    trace.rayDir.x = _.rayStep.x + _.position.x;
    trace.rayDir.y = _.rayStep.y + _.position.y;
    trace.rayDir.z = _.rayStep.z + _.position.z;
  }
   
  static inline void evalBeamStep(Trace& trace) {
    calcBase(trace);
    
    u32 _offset;
    ucb _color;
    
    u8 i = OPT_NUMBER_OF_RAY_PER_BEAM;
    while(i--) {
      if (
        #if OPT_BEAM_MASK != 0xFFFFFFFF && OPT_BEAM_MASK != 0xFFFFFFFFFFFFFFFF
        !((1 << i) & OPT_BEAM_MASK) || 
        #endif
        (_.colorChecker & (OPT_COLOR_CHECKER_ENCODING(1) << i))) {
        continue;
      }

      trace.ray.x = trace.rayDir.x + _.rayBase * _.coords[i].x;
      trace.ray.y = trace.rayDir.y + _.rayBase * _.coords[i].y;
      trace.ray.z = trace.rayDir.z + _.rayBase * _.coords[i].z;
      
      if(
        trace.ray.x < 0.0f || trace.ray.x >= OPT_SPACE_SIZE ||
        trace.ray.y < 0.0f || trace.ray.y >= OPT_SPACE_SIZE ||
        trace.ray.z < 0.0f || trace.ray.z >= OPT_SPACE_SIZE) {
        continue;
      }
      
      trace.iray.x = static_cast<u16>(trace.ray.x);
      trace.iray.y = static_cast<u16>(trace.ray.y); 
      trace.iray.z = static_cast<u16>(trace.ray.z);
        
      _offset = (trace.iray.x >> _.bmc->rlevel) |
        ((trace.iray.y >> _.bmc->rlevel) << _.bmc->yBitOffset) |
        ((trace.iray.z >> _.bmc->rlevel) << _.bmc->zBitOffset);
      
      _color = _.bmc->region[_offset];
      
      if (_color) {
        _buffer[_.o[i]] = getDepthColor(_color);

        #if OPT_USE_COLLIDE != 0
        if(_.collide) {
          fadeToColor(&_buffer[_.o[i]], _buffer[_.o[i]], 0xFF, 0.86f);
        }
        #endif
        _.colorChecker |= OPT_COLOR_CHECKER_ENCODING(1) << i;
      }
    }
    return;
  }
  
  static u8 _originToFind[OPT_NUMBER_OF_ORIGIN_TO_FIND] __attribute__((aligned(16))) = {0};
  static inline void originFound(Trace& trace) {
    calcBase(trace);
    u32 _offset;
    u8 i = OPT_NUMBER_OF_ORIGIN_TO_FIND;
    while (i--) {
      trace.ray.x = trace.rayDir.x + _.rayBase * _.coords[_originToFind[i]].x;
      trace.ray.y = trace.rayDir.y + _.rayBase * _.coords[_originToFind[i]].y;
      trace.ray.z = trace.rayDir.z + _.rayBase * _.coords[_originToFind[i]].z;
      
      if(
        trace.ray.x < 0.0f || trace.ray.x >= OPT_SPACE_SIZE ||
        trace.ray.y < 0.0f || trace.ray.y >= OPT_SPACE_SIZE ||
        trace.ray.z < 0.0f || trace.ray.z >= OPT_SPACE_SIZE) {
        continue;
      }
      trace.iray.x = static_cast<u16>(trace.ray.x); 
      trace.iray.y = static_cast<u16>(trace.ray.y); 
      trace.iray.z = static_cast<u16>(trace.ray.z);
        
      _offset = (trace.iray.x >> _.bmc->rlevel) |
        ((trace.iray.y >> _.bmc->rlevel) << _.bmc->yBitOffset) |
        ((trace.iray.z >> _.bmc->rlevel) << _.bmc->zBitOffset);
        
      if(_.bmc->region[_offset]) {
        if (trace.level == MAX_DETAIL_LEVEL - 1) {
          trace.found = 1;
        }
        trace.level++;
        _.rayLength -= trace.prevStep;
        _.bmc = stack[trace.level];
        return;
      }
    }
    return;
  }
  
  static inline void beam() {
    u8 i = OPT_NUMBER_OF_RAY_PER_BEAM;
    while (i--) {
      _.coords[i] = mth::getSandwichProduct<v4>(&_origins[_.o[i]], &_.pov->q);
      _buffer[_.o[i]] = 0x0;
    }
    
    _.rayLength = OPT_RAY_BASE_CUT;
    _.colorChecker = 0;
    
    Trace trace;
    trace.prevStep = 0.0f;
    trace.found = 0;
    trace.level = OPT_BOOST_LEVEL;
    _.bmc = stack[trace.level];
    
    do {
      if(!trace.found) {
        originFound(trace);
        trace.prevStep = _.bmc->lstep;
      }
      
      if(trace.found) {
        evalBeamStep(trace);
        if(_.colorChecker == OPT_COLOR_CHECKER_ENCODING(OPT_BEAM_MASK)) {
          return;
        }
      }
      _.rayLength += _.bmc->lstep;      
    } while(_.rayLength < OPT_RAY_MAX_DEPTH);
  }

  void getRendering(ucb* const buffer, SpacePov* const pov) {
    _buffer = buffer;
    _.position = pov->position;
    _.pov = pov::getPov(pov->vstep, pov->hstep);
    _.rayStep = mth::getNormalized4(mth::getSandwichProduct({0.0f, 0.0f, 1.0f, 0.0f}, _.pov->q));
    
    u32 i = 0;
    while(i < OFFSET_CELL_NUMBER) {
      _.o = &(OFFSETS[i]);
      beam();
      i += OPT_NUMBER_OF_RAY_PER_BEAM;
    } 
  }
  
  static void initParams() {
    {
      u8 i = 0;
      u8 j = 0;
      while (i < OPT_NUMBER_OF_RAY_PER_BEAM) {
        if (OPT_COLOR_CHECKER_ENCODING(OPT_ORIGIN_MASK) & ((OPT_COLOR_CHECKER_ENCODING(1)) << i)) {
          _originToFind[j] = i;
          j++;
        }
        i++;
      }
    }
    RAY_COLOR_DEPTH_START = (OPT_RAY_MAX_DEPTH / 8.0f) * OPT_RAY_COLOR_DEPTH_FACTOR;
    COLOR_DEPTH_STEP = (1.0f / (OPT_RAY_MAX_DEPTH - RAY_COLOR_DEPTH_START));
  }
  
  u8 loadVoxelFrom(const char* const filename) {
    const u32 FILE_CHUNK_SIZE = 1024;
    FILE* const file = fopen(filename, "rb");
    if(file != NULL) {
      u32 count = 0;
      Voxel* const voxels = (Voxel* const)memalign(16, sizeof(Voxel) * FILE_CHUNK_SIZE);
      do {
        count = fread((void*)voxels, sizeof(Voxel), FILE_CHUNK_SIZE, file);
        if (count != 0) {
          fillSpace(voxels, count);
        }
      } while (count == FILE_CHUNK_SIZE);
      fclose(file);
      free(voxels);
      return 1;
    }
    return 0;
  }
  
  int init() {
    initMemory();
    _bmc_memory_space = (ucb*)memalign(16, sizeof(ucb) * BMC_MEMORY_SPACE);
    memset(_bmc_memory_space, 0, sizeof(ucb) * BMC_MEMORY_SPACE);
    constexpr u32 JSTEP = OPT_FRAME_SIZE * OPT_GRID_HEIGHT;
    constexpr u32 H_SCISSOR_START = JSTEP * OPT_H_SCISSOR;
    constexpr u32 H_SCISSOR_END = OPT_FRAME_BUFFER_SIZE - JSTEP * OPT_H_SCISSOR;
    constexpr u32 V_SCISSOR_START = OPT_GRID_WIDTH * OPT_V_SCISSOR_START + OPT_V_DISPLACEMENT;
    constexpr u32 V_SCISSOR_END = OPT_FRAME_SIZE - OPT_GRID_WIDTH * OPT_V_SCISSOR_END;
    
    constexpr u32 V_CELL_NUMBER = (V_SCISSOR_END - V_SCISSOR_START) / OPT_GRID_WIDTH;
    constexpr u32 H_CELL_NUMBER = ((H_SCISSOR_END - H_SCISSOR_START)
      / OPT_FRAME_SIZE) / OPT_GRID_HEIGHT;
    constexpr u32 CELL_NUMBER = V_CELL_NUMBER * H_CELL_NUMBER;
    OFFSET_CELL_NUMBER = OPT_NUMBER_OF_RAY_PER_BEAM * CELL_NUMBER;
    OFFSETS = (u32*)memalign(16, sizeof(u32) * OFFSET_CELL_NUMBER);
  
    u32 n = 0;
    u32 offset = 0;
    u32 j = H_SCISSOR_START;
    while(j < H_SCISSOR_END) {
      u32 i = V_SCISSOR_START;
      while (i < V_SCISSOR_END) {
        offset = i + j;
        
        #if OPT_NUMBER_OF_RAY_PER_BEAM == 64
        {
          u8 m = 0;
          u8 k = 0;
          while(k < 8) {
            u8 l = 0;
            while(l < 8){
              OFFSETS[m + n] = offset + l + OPT_FRAME_SIZE * k;
              m++;
              l++;
            }
            k+=1;
          }
        }
        #endif
        
        #if OPT_NUMBER_OF_RAY_PER_BEAM == 32
        {
          u8 m = 0;
          for (u8 k = 0; k < 4; k++) {
            for (u8 l = 0; l < 8; l++) {
              OFFSETS[m + n] = offset + l + OPT_FRAME_SIZE_X2 * k;
              m++;
            }
          }
        }
        #endif
        
        n += OPT_NUMBER_OF_RAY_PER_BEAM;
        i += OPT_GRID_WIDTH;
      }
      j += JSTEP;
    }
    initParams();
    frameLevel = mth::getPO2(OPT_FRAME_SIZE);
    genSpace(MAX_SPACE_LEVEL, MAX_SPACE_LEVEL);
    u8 found;
    int num = -1;
    do {
      num++;
      std::stringstream stream;
      std::string str;
      stream << num;
      stream >> str;
      str = "./object_" + str + ".bin";
      found = loadVoxelFrom(str.c_str());
    } while(found);
    
    if (MIN_LEVEL < MAX_DETAIL_LEVEL) {
      const u8 lod = MAX_DETAIL_LEVEL < MAX_SPACE_LEVEL ? MAX_DETAIL_LEVEL : MAX_SPACE_LEVEL -1; 
      genSpace(lod, MIN_LEVEL, space);
      fillSpace();
    }
    _.space = space;
    cacheRayOrigins();
    
    if (num == 0) {
      return -1;
    }
    return 0;
  }
}
