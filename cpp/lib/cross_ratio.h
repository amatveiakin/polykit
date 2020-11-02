#pragma once

#include "delta.h"
#include "util.h"


inline DeltaExpr cross_ratio(X a, X b, X c, X d) {
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
inline DeltaExpr neg_cross_ratio(X a, X b, X c, X d) {
  return cross_ratio(a, c, b, d);
}
