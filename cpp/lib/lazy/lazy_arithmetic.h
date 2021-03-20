#pragma once

#include "lazy.h"


template<typename T>
T lazy_add(const T& lhs, const T& rhs) {
  return make_lazy("add", [](auto&& x, auto&& y) { return x + y; }, lhs, rhs);
}
template<typename T>
T lazy_sub(const T& lhs, const T& rhs) {
  return make_lazy("sub", [](auto&& x, auto&& y) { return x - y; }, lhs, rhs);
}

template<typename T>
T lazy_mul(const T& expr, int scalar) {
  return make_lazy("sub", [](auto&& x, auto&& y) { return x * y; }, expr, scalar);
}
template<typename T>
T lazy_dived_int(const T& expr, int scalar) {
  return make_lazy("div", [](auto&& x, auto&& y) { return x.dived_int(y); }, expr, scalar);
}
