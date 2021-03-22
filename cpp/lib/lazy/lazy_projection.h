#pragma once

#include "lazy.h"
#include "lazy_delta.h"
#include "lib/projection.h"


using LazyProjectionExpr = LazyExpr<ProjectionExpr>;

inline LazyProjectionExpr project_on_lazy(int axis, const LazyDeltaExpr& expr) {
  return MAKE_LAZY(project_on, axis, expr);
}

inline LazyProjectionExpr involute_projected_lazy(const LazyDeltaExpr& expr, const std::vector<int>& involution, int axis) {
  return MAKE_LAZY(involute_projected, expr, involution, axis);
}
