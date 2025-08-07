#pragma once
#include "./pov.hpp"
namespace bmc {
  extern Tracer _;
  int init();
  void initExtra();
  void updateParameters();
  void getRendering(ucb* const, SpacePov* const);
  void applyFilters(ucb* const, ucb* const);
}
