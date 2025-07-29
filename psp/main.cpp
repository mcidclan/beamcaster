// wip

PSP_MODULE_INFO("beamcaster", 0, 1, 1);
PSP_HEAP_SIZE_KB(-1024);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[1024] = {0};


sceGuStart(GU_DIRECT, list);
sceGuCopyImage(GU_PSM_5650, 0, 0, 480, 272, STRIDE, (void*)DST_BUFFER_565,
  0, 0, 512, (void*)(UNCACHED_USER_MASK | GE_EDRAM_BASE));
sceGuFinish();
sceGuSync(0,0);

int main() {
  scePowerSetClockFrequency(333, 333, 166);

  sceGuInit();
  pspDebugScreenInitEx(0x0, PSP_DISPLAY_PIXEL_FORMAT_8888, 0);
  sceDisplaySetFrameBuf((void*)(UNCACHED_USER_MASK | GE_EDRAM_BASE),
    512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
  
  pspDebugScreenClear();
  SceCtrlData ctl;
  do {
    sceCtrlPeekBufferPositive(&ctl, 1);
    //
    sceDisplayWaitVblankStart();
  } while(!(ctl.Buttons & PSP_CTRL_HOME));
  
  pspDebugScreenClear();
  pspDebugScreenPrintf("Exiting...");
  sceKernelDelayThread(500000);
  sceKernelExitGame();
  return 0;
}