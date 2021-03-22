#pragma once

#include "lazy.h"
#include "lib/lyndon.h"


template<typename LinearT>
LazyExpr<LinearT> to_lyndon_basis_lazy(const LazyExpr<LinearT>& expression) {
  return MAKE_LAZY(to_lyndon_basis, expression);
}
