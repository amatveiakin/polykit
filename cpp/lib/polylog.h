#pragma once

#include "algebra.h"
#include "delta.h"


DeltaExpr cross_ratio(int a, int b, int c, int d) {
  return D(a, b) + D(c, d) - D(b, c) - D(d, a);
}

// 1 - cross_ratio(a, b, c, d)
DeltaExpr neg_cross_ratio(int a, int b, int c, int d) {
  return cross_ratio(a, c, b, d);
}

DeltaExpr Li2(int a, int b, int c, int d) {
  return tensor_product(
    cross_ratio(a, b, c, d),
    neg_cross_ratio(a, b, c, d)
  );
}
