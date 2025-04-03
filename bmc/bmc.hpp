#pragma once
#include "./pov.hpp"
namespace bmc {
  extern Tracer _;
  void init();
  void initExtra();
  void updateParameters();
  void getRendering(ucb* const, SpacePov* const, const u8);
  void applyFilters(ucb* const, ucb* const);
}
