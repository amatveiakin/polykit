#pragma once

#include "delta.h"


DeltaExpr IVec(SpanX points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(IVec(...))
// but faster.
WordExpr IVecPr(SpanX points, std::function<WordExpr(DeltaExpr)> projector);

DeltaExpr CorrVec(SpanX points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(CorrVec(...))
// but faster.
WordExpr CorrVecPr(SpanX points, std::function<WordExpr(DeltaExpr)> projector);

template<typename... Args>
DeltaExpr I(Args... args) {
  return IVec({args...});
}

template<typename... Args>
DeltaExpr Corr(Args... args) {
  return CorrVec({args...});
}
