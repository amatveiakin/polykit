#pragma once

#include "gamma.h"


GammaExpr GrQLiVec(int weight, const XArgs& bonus_points, const XArgs& qli_points);


namespace internal {
class GrQLiFixedWeights {
public:
  GrQLiFixedWeights(int weight, XArgs bonus_points)
    : weight_(weight), bonus_points_(std::move(bonus_points)) {}
  template<typename... Args>
  GammaExpr operator()(Args... qli_points) const {
    return GrQLiVec(weight_, bonus_points_, {qli_points...});
  }
private:
  int weight_;
  XArgs bonus_points_;
};
}  // namespace internal


// Usage:
//   GrQLi_w(q_1, ..., q_m)(p_1, ..., p_n)
template<typename... Args> internal::GrQLiFixedWeights GrQLi1(Args... args) { return {1, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi2(Args... args) { return {2, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi3(Args... args) { return {3, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi4(Args... args) { return {4, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi5(Args... args) { return {5, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi6(Args... args) { return {6, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi7(Args... args) { return {7, {args...}}; }
template<typename... Args> internal::GrQLiFixedWeights GrQLi8(Args... args) { return {8, {args...}}; }
