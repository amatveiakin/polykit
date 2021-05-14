#pragma once

#include "delta.h"
#include "util.h"


inline DeltaExpr cross_ratio(X a, X b, X c, X d) {
  return D(a, b) + D(c, d) - D(b, c) - D(d, a);
}

template<typename T>
DeltaExpr cross_ratio(const T& v) {
  CHECK(v.size() % 2 == 0) << dump_to_string(v);
  DeltaExpr ret;
  const int n = v.size();
  for (int i : range(n)) {
    ret += neg_one_pow(i) * D(v[i], v[(i+1)%n]);
  }
  return ret;
}

// 1 - cross_ratio(a, b, c, d)
inline DeltaExpr neg_cross_ratio(X a, X b, X c, X d) {
  return cross_ratio(a, c, b, d);
}

// 1 - 1 / cross_ratio(a, b, c, d)
inline DeltaExpr neg_inv_cross_ratio(X a, X b, X c, X d) {
  return cross_ratio(a, c, d, b);
}
