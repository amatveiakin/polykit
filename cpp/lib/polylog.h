#pragma once

#include "delta.h"
#include "util.h"


inline DeltaExpr cross_ratio(int a, int b, int c, int d) {
  return D(a, b) + D(c, d) - D(b, c) - D(d, a);
}

template<typename T>
DeltaExpr cross_ratio(const T& v) {
  DeltaExpr ret;
  const int n = v.size();
  for (int i = 0; i < n; ++i) {
    ret += neg_one_pow(i) * D(v[i], v[(i+1)%n]);
  }
  return ret;
}

// 1 - cross_ratio(a, b, c, d)
inline DeltaExpr neg_cross_ratio(int a, int b, int c, int d) {
  return cross_ratio(a, c, b, d);
}

DeltaExpr Li(int weight, const std::vector<int>& points);

namespace internal {
template<typename... Args>
DeltaExpr Li_dispatch(int weight, Args... args) {
  return Li(weight, {args...});
}
template<>
inline DeltaExpr Li_dispatch(int weight, const std::vector<int>& points) {
  return Li(weight, points);
}
}  // namespace internal

template<typename... Args> inline DeltaExpr Li2(Args... args) { return internal::Li_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr Li3(Args... args) { return internal::Li_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr Li4(Args... args) { return internal::Li_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr Li5(Args... args) { return internal::Li_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr Li6(Args... args) { return internal::Li_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr Li7(Args... args) { return internal::Li_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr Li8(Args... args) { return internal::Li_dispatch(8, args...); }
