#include "./pov.hpp"

namespace pov {
  static Pov* pov;
  static void cachePov() {
    pov = (Pov*)memalign(16, sizeof(Pov) * OPT_HORIZONTAL_POV_COUNT * OPT_VERTICAL_POV_COUNT); 
    u16 hstep = 0;
    while(hstep < OPT_HORIZONTAL_POV_COUNT) {
      u16 vstep = 0;
      const float hspin = (float)(hstep * (360.0f / OPT_HORIZONTAL_POV_COUNT));
      const v4 qa = mth::getNormalized4(mth::getOrientedQuat<v4>({0.0f, 1.0f, 0.0f, _ang(hspin)}));
      while(vstep < OPT_VERTICAL_POV_COUNT) {
        const float vspin = (float)(vstep * (360.0f / OPT_VERTICAL_POV_COUNT));
        const v4 qb = mth::getNormalized4(mth::getOrientedQuat<v4>({1.0f, 0.0f, 0.0f, _ang(vspin)}));
        const v4 qc = mth::getNormalized4(mth::mulQuat(qa, qb));
        pov[vstep + hstep * OPT_VERTICAL_POV_COUNT] = {qc};
        vstep++;
      }
      hstep++;
    }
  }
  
  Pov* const getPov(const u16 vstep, const u16 hstep) {
    return &pov[vstep + hstep * OPT_VERTICAL_POV_COUNT];
  }
  
  void init() {
    cachePov();
  }
  
  void clean() {
    free(pov);
  }
}
