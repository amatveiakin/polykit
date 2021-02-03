#pragma once

#include "delta.h"
#include "projection.h"


DeltaExpr IVec(SpanX points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(IVec(...))
// but faster.
ProjectionExpr IVecPr(SpanX points, DeltaProjector projector);

DeltaExpr CorrVec(SpanX points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(CorrVec(...))
// but faster.
ProjectionExpr CorrVecPr(SpanX points, DeltaProjector projector);

template<typename... Args>
DeltaExpr I(Args... args) {
  return IVec({args...});
}

template<typename... Args>
DeltaExpr Corr(Args... args) {
  return CorrVec({args...});
}
