#pragma once

#include "lazy.h"
#include "lib/delta.h"


using LazyDeltaExpr = LazyExpr<DeltaExpr>;

LazyDeltaExpr substitute_variables_lazy(const LazyDeltaExpr& subexpr, const std::vector<X>& new_points) {
  return MAKE_LAZY(substitute_variables, subexpr, new_points);
}
