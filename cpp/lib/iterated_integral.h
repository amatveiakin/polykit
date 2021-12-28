// Iterated integral and correlator.
//
//   Corr(x1, .., xn) = I(Inf, x1, ..., xn)
//
// The functions come in three forms (examples use I, but the same is true for Corr):
//   * Simple form: `I(x1,x2,...,xn)`, e.g. `I(1,2,3,4,5,6)`.
//   * Vector form: `IVec(vector_of_arguments)` e.g. `IVec({1,2,3,4,5,6})`.
//   * Projected form: `IVecPr(vector_of_arguments, projector)`.
//     Assuming that `projector` is indeed a projection, this is equivalent to
//     `projector(IVec(vector_of_arguments))` but faster.
//
// Any argument can be `Inf` denoting that the corresponding variable is sent to infinity.
// In practice this means that all terms including this variable are discarded.

#pragma once

#include "corr_expression.h"
#include "delta.h"
#include "projection.h"


DeltaExpr IVec(const XArgs& points);
ProjectionExpr IVecPr(const XArgs& points, DeltaProjector projector);

DeltaExpr IAltVec(const XArgs& points);
ProjectionExpr IAltVecPr(const XArgs& points, DeltaProjector projector);

DeltaExpr CorrVec(const XArgs& points);
ProjectionExpr CorrVecPr(const XArgs& points, DeltaProjector projector);

DeltaExpr CorrAltVec(const XArgs& points);
ProjectionExpr CorrAltVecPr(const XArgs& points, DeltaProjector projector);

DeltaExpr eval_formal_symbols(const CorrExpr& expr);


template<typename... Args>
DeltaExpr I(Args... args) {
  return IVec({args...});
}

template<typename... Args>
DeltaExpr IAlt(Args... args) {
  return IAltVec({args...});
}

template<typename... Args>
DeltaExpr Corr(Args... args) {
  return CorrVec({args...});
}

template<typename... Args>
DeltaExpr CorrAlt(Args... args) {
  return CorrAltVec({args...});
}
