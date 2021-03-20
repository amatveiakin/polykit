#pragma once

#include "lazy.h"
#include "lib/delta.h"


using LazyDeltaExpr = LazyExpr<DeltaExpr>;

auto substitute_variables_lazy(LazyExpr<DeltaExpr> subexpr, std::vector<X> new_points) {
  return MAKE_LAZY(substitute_variables, subexpr, new_points);
}
