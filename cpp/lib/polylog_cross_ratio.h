#pragma once

#include "cross_ratio.h"
#include "delta.h"


DeltaExpr LidoVec(int weight, const std::vector<X>& points);
DeltaExpr LidoVec(int weight, const std::vector<int>& points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(LidoVec(...))
// but faster.
WordExpr LidoVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector);

// Only 6 point are supported for now
DeltaExpr LidoSymmVec(int weight, const std::vector<X>& points);
DeltaExpr LidoSymmVec(int weight, const std::vector<int>& points);

// Assuming projector is indeed a projection, this is equivalent to
//   projector(LidoSymmVec(...))
// but faster.
WordExpr LidoSymmVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector);

namespace internal {
template<typename... Args>
DeltaExpr Lido_dispatch(int weight, Args... args) {
  return LidoVec(weight, std::vector<X>{args...});
}
template<typename... Args>
DeltaExpr LidoSymm_dispatch(int weight, Args... args) {
  return LidoSymmVec(weight, std::vector<X>{args...});
}
}  // namespace internal

template<typename... Args> inline DeltaExpr Lido1(Args... args) { return internal::Lido_dispatch(1, args...); }
template<typename... Args> inline DeltaExpr Lido2(Args... args) { return internal::Lido_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr Lido3(Args... args) { return internal::Lido_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr Lido4(Args... args) { return internal::Lido_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr Lido5(Args... args) { return internal::Lido_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr Lido6(Args... args) { return internal::Lido_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr Lido7(Args... args) { return internal::Lido_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr Lido8(Args... args) { return internal::Lido_dispatch(8, args...); }

template<typename... Args> inline DeltaExpr LidoSymm1(Args... args) { return internal::LidoSymm_dispatch(1, args...); }
template<typename... Args> inline DeltaExpr LidoSymm2(Args... args) { return internal::LidoSymm_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr LidoSymm3(Args... args) { return internal::LidoSymm_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr LidoSymm4(Args... args) { return internal::LidoSymm_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr LidoSymm5(Args... args) { return internal::LidoSymm_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr LidoSymm6(Args... args) { return internal::LidoSymm_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr LidoSymm7(Args... args) { return internal::LidoSymm_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr LidoSymm8(Args... args) { return internal::LidoSymm_dispatch(8, args...); }
