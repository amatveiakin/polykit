// Functions that compute symbol for QLi and its variations.
//
// Contains three functions:
//   * `QLi` corresponds to QLi+
//   * `QLiNeg` corresponds to QLi-
//   * `QLiSymm` is a symmetrized version of QLi (except weight 1 where it's just QLi)
//
// Each function can be called using one of the three forms (examples use QLi, but
// the same is true for QLiNeg and QLiSymm):
//   * Simple form: `QLiN(x1,x2,...,xk)` where N is weight, e.g. `QLi4(1,2,3,4,5,6)`.
//   * Full form: `QLi(weight, vector_of_arguments)` e.g. `QLi(4, {1,2,3,4,5,6})`.
//   * Projected form: `QLi(weight, vector_of_arguments, projector)`.
//     Assuming that `projector` is indeed a projection, this is equivalent to
//     `projector(QLi(weight, vector_of_arguments))` but faster.
//
// Any argument can be `Inf` denoting that the corresponding variable is sent to infinity.
// In practice this means that any terms including this variable are discarded.

#pragma once

#include "delta_ratio.h"
#include "delta.h"
#include "projection.h"


DeltaExpr QLiVec(int weight, SpanX points);
ProjectionExpr QLiVecPr(int weight, SpanX points, DeltaProjector projector);

DeltaExpr QLiNegVec(int weight, SpanX points);
ProjectionExpr QLiNegVecPr(int weight, SpanX points, DeltaProjector projector);

DeltaExpr QLiSymmVec(int weight, SpanX points);
ProjectionExpr QLiSymmVecPr(int weight, SpanX points, DeltaProjector projector);


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


template<typename... Args> inline DeltaExpr QLi1(Args... args) { return internal::QLi_dispatch(1, args...); }
template<typename... Args> inline DeltaExpr QLi2(Args... args) { return internal::QLi_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr QLi3(Args... args) { return internal::QLi_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr QLi4(Args... args) { return internal::QLi_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr QLi5(Args... args) { return internal::QLi_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr QLi6(Args... args) { return internal::QLi_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr QLi7(Args... args) { return internal::QLi_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr QLi8(Args... args) { return internal::QLi_dispatch(8, args...); }

template<typename... Args> inline DeltaExpr QLiNeg1(Args... args) { return internal::QLiNeg_dispatch(1, args...); }
template<typename... Args> inline DeltaExpr QLiNeg2(Args... args) { return internal::QLiNeg_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr QLiNeg3(Args... args) { return internal::QLiNeg_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr QLiNeg4(Args... args) { return internal::QLiNeg_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr QLiNeg5(Args... args) { return internal::QLiNeg_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr QLiNeg6(Args... args) { return internal::QLiNeg_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr QLiNeg7(Args... args) { return internal::QLiNeg_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr QLiNeg8(Args... args) { return internal::QLiNeg_dispatch(8, args...); }

template<typename... Args> inline DeltaExpr QLiSymm1(Args... args) { return internal::QLiSymm_dispatch(1, args...); }
template<typename... Args> inline DeltaExpr QLiSymm2(Args... args) { return internal::QLiSymm_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr QLiSymm3(Args... args) { return internal::QLiSymm_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr QLiSymm4(Args... args) { return internal::QLiSymm_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr QLiSymm5(Args... args) { return internal::QLiSymm_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr QLiSymm6(Args... args) { return internal::QLiSymm_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr QLiSymm7(Args... args) { return internal::QLiSymm_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr QLiSymm8(Args... args) { return internal::QLiSymm_dispatch(8, args...); }
