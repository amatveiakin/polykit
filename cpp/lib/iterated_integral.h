#pragma once

#include "delta.h"


DeltaExpr IVec(const std::vector<X>& points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(IVec(...))
// but faster.
WordExpr IVecPr(
    const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector);

DeltaExpr CorrVec(const std::vector<X>& points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(CorrVec(...))
// but faster.
WordExpr CorrVecPr(
    const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector);

template<typename... Args>
DeltaExpr I(Args... args) {
  return IVec({args...});
}

template<typename... Args>
DeltaExpr Corr(Args... args) {
  return CorrVec({args...});
}
