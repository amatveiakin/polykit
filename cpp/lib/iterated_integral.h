#pragma once

#include "delta.h"


namespace internal {
DeltaExpr I_impl(const std::vector<int>& points);
}

template<typename... Args>
DeltaExpr I(Args... args) {
  return internal::I_impl({args...});
}

template<>
inline DeltaExpr I(const std::vector<int>& points) {
  return internal::I_impl(points);
}
