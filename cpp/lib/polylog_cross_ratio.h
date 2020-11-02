#pragma once

#include "cross_ratio.h"
#include "delta.h"


DeltaExpr LidoVec(int weight, const std::vector<X>& points);
DeltaExpr LidoVec(int weight, const std::vector<int>& points);

namespace internal {
template<typename... Args>
DeltaExpr Lido_dispatch(int weight, Args... args) {
  return LidoVec(weight, std::vector<X>{args...});
}
}  // namespace internal

template<typename... Args> inline DeltaExpr Lido2(Args... args) { return internal::Lido_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr Lido3(Args... args) { return internal::Lido_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr Lido4(Args... args) { return internal::Lido_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr Lido5(Args... args) { return internal::Lido_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr Lido6(Args... args) { return internal::Lido_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr Lido7(Args... args) { return internal::Lido_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr Lido8(Args... args) { return internal::Lido_dispatch(8, args...); }
