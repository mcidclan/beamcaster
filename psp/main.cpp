#include <pspkernel.h>
#include <pspctrl.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <psprtc.h>
#include <psppower.h>
#include <pspdisplay.h>
#include <pspgu.h>

#include "../bmc/bmc.hpp"

PSP_MODULE_INFO("beamcaster", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-1024);

#if OPT_USE_COLLIDE
static u8 canMove(const Vec3<i16>* const p) {
  if(p->x < 0 || p->x >= OPT_SPACE_SIZE ||
    p->y < 0 || p->y >= OPT_SPACE_SIZE ||
    p->z < 0 || p->z >= OPT_SPACE_SIZE) {
    return 1;
  }
  static u32 offset;
  static ucb color;
  offset = (p->x >> bmc::_.space->rlevel) |
    ((p->y >> bmc::_.space->rlevel) << bmc::_.space->yBitOffset) |
    ((p->z >> bmc::_.space->rlevel) << bmc::_.space->zBitOffset);
  color = bmc::_.space->region[offset];
  return (color & 1 << 5) || color == 0;
}
#endif

SceCtrlData pad;
static float dstep = OPT_CAM_STEP;
static inline void getView(ucb* const frame) {
  static v3 position = OPT_DEFAULT_CAM_POSITION;
  static float ax = OPT_DEFAULT_CAM_ANGLE_X;
  static float ay = OPT_DEFAULT_CAM_ANGLE_Y;
  
  SpacePov pov = { 0.0f, ax, ay, position };

  #if OPT_USE_COLLIDE == 1
  static v3 _position = position;
  if (pad.Buttons & PSP_CTRL_TRIANGLE) {
    _iposition.x = position.x;
    _position.x = position.x + bmc::_.rayStep.x * dstep;
    _iposition.y = position.y;
    _position.y = position.y + bmc::_.rayStep.y * dstep;
    _iposition.z = position.z;
    _position.z = position.z + bmc::_.rayStep.z * dstep;
  }

  if (pad.Buttons & PSP_CTRL_CROSS) {
    _iposition.x = position.x;
    _position.x = position.x - bmc::_.rayStep.x * dstep;
    _iposition.y = position.y;
    _position.y = position.y - bmc::_.rayStep.y * dstep;
    _iposition.z = position.z;
    _position.z = position.z - bmc::_.rayStep.z * dstep;
  }

  bmc::_.collide = 0;
  if (canMove(&_iposition) && canMove(&_position)) {
    position = _position;
  } else {
    bmc::_.collide = 1;
  }
  #else
  if (pad.Buttons & PSP_CTRL_TRIANGLE) {
    position.x += bmc::_.rayStep.x * dstep;
    position.y += bmc::_.rayStep.y * dstep;
    position.z += bmc::_.rayStep.z * dstep;
  }
  
  if (pad.Buttons & PSP_CTRL_CROSS) {
    position.x -= bmc::_.rayStep.x * dstep;
    position.y -= bmc::_.rayStep.y * dstep;
    position.z -= bmc::_.rayStep.z * dstep;
  }
  #endif

  if (pad.Buttons & PSP_CTRL_LEFT) ay = (ay - 1) < 0 ? OPT_POV_RANGE - 1 : ay - 1;
  if (pad.Buttons & PSP_CTRL_RIGHT) ay = (ay + 1) >= OPT_POV_RANGE ? 0 : ay + 1;
  if (pad.Buttons & PSP_CTRL_DOWN) ax = (ax - 1) < 0 ? OPT_POV_RANGE - 1 : ax - 1;
  if (pad.Buttons & PSP_CTRL_UP) ax = (ax + 1) >= OPT_POV_RANGE ? 0 : ax + 1;
  
  bmc::getRendering(frame, &pov);
}

constexpr u32 VRAM_BASE = 0x44000000;
static unsigned int __attribute__((aligned(16))) list[1024] = {0};

int main() {
  scePowerSetClockFrequency(333, 333, 166);
  
  sceGuInit();
  pspDebugScreenInitEx(0x0, PSP_DISPLAY_PIXEL_FORMAT_565, 0);
  sceDisplaySetFrameBuf((void*)VRAM_BASE, 512,
  PSP_DISPLAY_PIXEL_FORMAT_565, PSP_DISPLAY_SETBUF_NEXTFRAME);

  pspDebugScreenSetXY(1, 1);
  pspDebugScreenSetTextColor(0xFF00A0FF);
  pspDebugScreenPrintf("init...");
  u64 prev, now, fps = 0;
  const u64 tickResolution = sceRtcGetTickResolution();
  
  bmc::init();
  pov::init();
  pspDebugScreenSetXY(1, 1);
  pspDebugScreenPrintf("                                                     ");
  
  ucb* const drawbuffer = (ucb*)(VRAM_BASE + 512*512*sizeof(ucb));
  
  do {
    sceCtrlPeekBufferPositive(&pad, 1);
    sceRtcGetCurrentTick(&prev);
    
    getView(drawbuffer);
    
    sceGuStart(GU_DIRECT, list);
    sceGuCopyImage(GU_PSM_5650, 0, 0, OPT_FRAME_SIZE, OPT_FRAME_SIZE, OPT_FRAME_SIZE, (void*)drawbuffer,
      112, 8, 512, (void*)(VRAM_BASE));
    sceGuFinish();
    sceGuSync(0,0);

    sceRtcGetCurrentTick(&now);
    fps = tickResolution / (now - prev);
    
    pspDebugScreenSetXY(1, 1);
    pspDebugScreenPrintf("Fps: %llu    ", fps);
    sceDisplayWaitVblankStart();
  } while(!(pad.Buttons & PSP_CTRL_SELECT));

  sceKernelExitGame();
  return 0;
}

