#pragma once

#include "lazy.h"


template<typename T>
LazyExpr<T> operator+(const LazyExpr<T>& expr) {
  return expr;
}
template<typename T>
LazyExpr<T> operator-(const LazyExpr<T>& expr) {
  return make_lazy("neg", [](auto&& x) { return -x; }, expr);
}

template<typename T>
LazyExpr<T> operator+(const LazyExpr<T>& lhs, const LazyExpr<T>& rhs) {
  return make_lazy("add", [](auto&& x, auto&& y) { return x + y; }, lhs, rhs);
}
template<typename T>
LazyExpr<T> operator-(const LazyExpr<T>& lhs, const LazyExpr<T>& rhs) {
  return make_lazy("sub", [](auto&& x, auto&& y) { return x - y; }, lhs, rhs);
}

template<typename T>
LazyExpr<T> operator*(const LazyExpr<T>& expr, int scalar) {
  return make_lazy("mul", [](auto&& x, auto&& y) { return x * y; }, expr, scalar);
}
template<typename T>
LazyExpr<T> operator*(int scalar, const LazyExpr<T>& expr) {
  return expr * scalar;
}
template<typename T>
LazyExpr<T> dived_int(const LazyExpr<T>& expr, int scalar) {
  return make_lazy("div", [](auto&& x, auto&& y) { return x.dived_int(y); }, expr, scalar);
}
