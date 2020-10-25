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

DeltaExpr Lira(int weight, const std::vector<int>& points);

namespace internal {
template<typename... Args>
DeltaExpr Lira_dispatch(int weight, Args... args) {
  return Lira(weight, {args...});
}
template<>
inline DeltaExpr Lira_dispatch(int weight, const std::vector<int>& points) {
  return Lira(weight, points);
}
}  // namespace internal

template<typename... Args> inline DeltaExpr Lira2(Args... args) { return internal::Lira_dispatch(2, args...); }
template<typename... Args> inline DeltaExpr Lira3(Args... args) { return internal::Lira_dispatch(3, args...); }
template<typename... Args> inline DeltaExpr Lira4(Args... args) { return internal::Lira_dispatch(4, args...); }
template<typename... Args> inline DeltaExpr Lira5(Args... args) { return internal::Lira_dispatch(5, args...); }
template<typename... Args> inline DeltaExpr Lira6(Args... args) { return internal::Lira_dispatch(6, args...); }
template<typename... Args> inline DeltaExpr Lira7(Args... args) { return internal::Lira_dispatch(7, args...); }
template<typename... Args> inline DeltaExpr Lira8(Args... args) { return internal::Lira_dispatch(8, args...); }
