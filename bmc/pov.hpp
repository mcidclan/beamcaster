#pragma once
#include "./mth.hpp"
#define OPT_VERTICAL_POV_COUNT OPT_POV_RANGE
#define OPT_HORIZONTAL_POV_COUNT OPT_POV_RANGE
namespace pov {
  void init();
  Pov* const getPov(const u16, const u16);
}
