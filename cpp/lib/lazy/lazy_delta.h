#pragma once

#include "lazy.h"
#include "lib/delta.h"


using LazyDeltaExpr = LazyExpr<DeltaExpr>;
using LazyDeltaCoExpr = LazyExpr<DeltaCoExpr>;

inline LazyDeltaExpr substitute_variables_lazy(const LazyDeltaExpr& subexpr, const std::vector<X>& new_points) {
  return MAKE_LAZY(substitute_variables, subexpr, new_points);
}

inline LazyDeltaExpr involute_lazy(const LazyDeltaExpr& expr, const std::vector<int>& points) {
  return MAKE_LAZY(involute, expr, points);
}

inline LazyDeltaCoExpr coproduct_lazy(const LazyDeltaExpr& lhs, const LazyDeltaExpr& rhs) {
  return MAKE_LAZY(coproduct, lhs, rhs);
}

inline LazyDeltaCoExpr comultiply_lazy(const LazyDeltaExpr& expr, std::pair<int, int> form) {
  return MAKE_LAZY(comultiply, expr, form);
}
