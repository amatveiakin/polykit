// Functions that compute symbol for QLi and its variations.
//
// Contains functions:
//   * `Log` gives a simple cross-ratio
//   * `QLi` corresponds to QLi+
//   * `QLiNeg` corresponds to QLi-
//   * `QLiSymm` is a symmetrized version of QLi (except weight 1 where it's just QLi)
//   * `A2` is defined in https://arxiv.org/pdf/1401.6446.pdf
//
// Each function comes in three forms (examples use QLi, but the same is true for QLiNeg
// and QLiSymm):
//   * Simple form: `QLiN(x1,x2,...,xk)` where N is weight, e.g. `QLi4(1,2,3,4,5,6)`.
//   * Vector form: `QLiVec(weight, vector_of_arguments)` e.g. `QLiVec(4, {1,2,3,4,5,6})`.
//   * Projected form: `QLiVecPr(weight, vector_of_arguments, projector)`.
//     Assuming that `projector` is indeed a projection, this is equivalent to
//     `projector(QLiVec(weight, vector_of_arguments))` but faster.
//
// Any argument can be `Inf` denoting that the corresponding variable is sent to infinity.
// In practice this means that all terms including this variable are discarded.

#pragma once

#include "delta_ratio.h"
#include "delta.h"
#include "projection.h"


DeltaExpr LogVec(const XArgs& args);

DeltaExpr QLiVec(int weight, const XArgs& points);
ProjectionExpr QLiVecPr(int weight, const XArgs& points, DeltaProjector projector);

DeltaExpr QLiNegVec(int weight, const XArgs& points);
ProjectionExpr QLiNegVecPr(int weight, const XArgs& points, DeltaProjector projector);

DeltaExpr QLiSymmVec(int weight, const XArgs& points);
ProjectionExpr QLiSymmVecPr(int weight, const XArgs& points, DeltaProjector projector);

DeltaExpr A2Vec(const XArgs& args);

// Reduce QLi expr, weight and points must be the same as in QLi.
// Incompatible with substituting infinity.
// Can be used as a projector.
// TODO: Dedup against `normalize_remove_consecutive`.
DeltaExpr normalized_delta(const DeltaExpr& expr, int weight, const XArgs& points);


namespace internal {
template<typename... Args>
DeltaExpr QLi_dispatch(int weight, Args... args) {
  return QLiVec(weight, std::vector<X>{args...});
}
template<typename... Args>
DeltaExpr QLiNeg_dispatch(int weight, Args... args) {
  return QLiNegVec(weight, std::vector<X>{args...});
}
template<typename... Args>
DeltaExpr QLiSymm_dispatch(int weight, Args... args) {
  return QLiSymmVec(weight, std::vector<X>{args...});
}
}  // namespace internal


template<typename... Args> DeltaExpr Log(Args... args) { return LogVec(std::vector<X>{args...}); }

template<typename... Args> DeltaExpr QLi1(Args... args) { return internal::QLi_dispatch(1, args...); }
template<typename... Args> DeltaExpr QLi2(Args... args) { return internal::QLi_dispatch(2, args...); }
template<typename... Args> DeltaExpr QLi3(Args... args) { return internal::QLi_dispatch(3, args...); }
template<typename... Args> DeltaExpr QLi4(Args... args) { return internal::QLi_dispatch(4, args...); }
template<typename... Args> DeltaExpr QLi5(Args... args) { return internal::QLi_dispatch(5, args...); }
template<typename... Args> DeltaExpr QLi6(Args... args) { return internal::QLi_dispatch(6, args...); }
template<typename... Args> DeltaExpr QLi7(Args... args) { return internal::QLi_dispatch(7, args...); }
template<typename... Args> DeltaExpr QLi8(Args... args) { return internal::QLi_dispatch(8, args...); }

template<typename... Args> DeltaExpr QLiNeg1(Args... args) { return internal::QLiNeg_dispatch(1, args...); }
template<typename... Args> DeltaExpr QLiNeg2(Args... args) { return internal::QLiNeg_dispatch(2, args...); }
template<typename... Args> DeltaExpr QLiNeg3(Args... args) { return internal::QLiNeg_dispatch(3, args...); }
template<typename... Args> DeltaExpr QLiNeg4(Args... args) { return internal::QLiNeg_dispatch(4, args...); }
template<typename... Args> DeltaExpr QLiNeg5(Args... args) { return internal::QLiNeg_dispatch(5, args...); }
template<typename... Args> DeltaExpr QLiNeg6(Args... args) { return internal::QLiNeg_dispatch(6, args...); }
template<typename... Args> DeltaExpr QLiNeg7(Args... args) { return internal::QLiNeg_dispatch(7, args...); }
template<typename... Args> DeltaExpr QLiNeg8(Args... args) { return internal::QLiNeg_dispatch(8, args...); }

template<typename... Args> DeltaExpr QLiSymm1(Args... args) { return internal::QLiSymm_dispatch(1, args...); }
template<typename... Args> DeltaExpr QLiSymm2(Args... args) { return internal::QLiSymm_dispatch(2, args...); }
template<typename... Args> DeltaExpr QLiSymm3(Args... args) { return internal::QLiSymm_dispatch(3, args...); }
template<typename... Args> DeltaExpr QLiSymm4(Args... args) { return internal::QLiSymm_dispatch(4, args...); }
template<typename... Args> DeltaExpr QLiSymm5(Args... args) { return internal::QLiSymm_dispatch(5, args...); }
template<typename... Args> DeltaExpr QLiSymm6(Args... args) { return internal::QLiSymm_dispatch(6, args...); }
template<typename... Args> DeltaExpr QLiSymm7(Args... args) { return internal::QLiSymm_dispatch(7, args...); }
template<typename... Args> DeltaExpr QLiSymm8(Args... args) { return internal::QLiSymm_dispatch(8, args...); }

template<typename... Args> DeltaExpr A2(Args... args) { return A2Vec(std::vector<X>{args...}); }
