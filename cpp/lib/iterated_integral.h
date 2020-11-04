#pragma once

#include "delta.h"


DeltaExpr IVec(const std::vector<X>& points);

template<typename... Args>
DeltaExpr I(Args... args) {
  return IVec({args...});
}

template<typename... Args>
DeltaExpr Corr(Args... args) {
  return IVec({Inf, args...});
}
