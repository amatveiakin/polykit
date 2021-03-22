#pragma once

#include "lazy.h"
#include "lazy_delta.h"
#include "lib/projection.h"


using LazyProjectionExpr = LazyExpr<ProjectionExpr>;

LazyProjectionExpr project_on_lazy(int axis, const LazyDeltaExpr& expr) {
  return MAKE_LAZY(project_on, axis, expr);
}
