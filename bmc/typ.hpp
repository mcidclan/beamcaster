#pragma once
#include "./opt.hpp"
#include <malloc.h>
#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>

#define i16 short int
#define u8 unsigned char
#define u16 unsigned short int
#define u32 unsigned int
#define u64 uint64_t
#define v4 Vec4<float>
#define v3 Vec3<float>
#define v2 Vec2<float>
#define ucb u16

#ifdef _WIN32
  #define memalign(alignment, size) _aligned_malloc(size, alignment)
  #define free(ptr) _aligned_free(ptr)
#endif

template<typename T>
struct Vec4 {
  T x, y, z, w;
};

template<typename T>
struct Vec3 {
  T x, y, z;
};

template<typename T>
struct Vec2 {
  T x, y;
};

struct Voxel {
  ucb color;
  Vec4<char> coordinates;
};

struct SpacePov {
  float dstep;
  float vstep;
  float hstep;
  v3 position;
} __attribute__((packed, aligned(MEM_ALIGN)));

struct Pov {
  v4 q;
} __attribute__((packed, aligned(MEM_ALIGN)));

struct BMC {
  float lstep;
  float voxInvSize;
  u32 frame_sizeDigitsX;
  u32 frame_sizeDigitsY;
  u32 frame_sizeDigitsZ;
  u32 sizeDigitsX;
  u32 sizeDigitsY;
  u32 sizeDigitsZ;
  BMC* child;
  ucb* region;
  u16 size;
  u16 half;
  u16 rlevel;
  u16 frame_size;
  u16 frame_half;
  u8 level;
  u8 yBitOffset;
  u8 zBitOffset;
  u8 frame_yBitOffset;
  u8 frame_zBitOffset;
  char padding[1];
} __attribute__((packed, aligned(MEM_ALIGN)));

struct Tracer {
  v4 rayStep;
  v3 coords[OPT_NUMBER_OF_RAY_PER_BEAM];
  v3 position;
  v3 coordinates;
  float rayBase; //
  float rayLength; //
  #if OPT_NUMBER_OF_RAY_PER_BEAM == 64
  u64 colorChecker;
  #else
  u32 colorChecker;
  #endif  
  u32* o;
  Pov* pov;
  BMC* bmc;
  BMC* space;
  #if OPT_USE_COLLIDE
  struct {
    u8 collide;
  } __attribute__((packed, aligned(MEM_ALIGN)));
  #endif
} __attribute__((packed, aligned(MEM_ALIGN)));

struct Trace {
  v3 ray;
  v3 rayDir;
  float prevStep;
  Vec3<u16> iray;
  u8 level;
  u8 found;
} __attribute__((packed, aligned(MEM_ALIGN)));

