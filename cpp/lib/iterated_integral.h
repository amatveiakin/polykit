#pragma once

#include "delta.h"


DeltaExpr IVec(const std::vector<X>& points);

template<typename... Args>
DeltaExpr I(Args... args) {
  return IVec({args...});
}
