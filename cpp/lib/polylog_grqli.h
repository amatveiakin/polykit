#pragma once

#include "gamma.h"


GammaExpr GrLogVec(const std::vector<int>& bonus_points, const std::vector<int>& log_points);
GammaExpr GrLogVec(const std::pair<std::vector<int>, std::vector<int>>& points);

GammaExpr GrQLiVec(int weight, const std::vector<int>& bonus_points, const std::vector<int>& qli_points);
GammaExpr GrQLiVec(int weight, const std::pair<std::vector<int>, std::vector<int>>& points);


namespace internal {
class GrLogFixedBonusPoints {
public:
  GrLogFixedBonusPoints(std::vector<int> bonus_points)
    : bonus_points_(std::move(bonus_points)) {}
  template<typename... Args>
  GammaExpr operator()(Args... qli_points) const {
    return GrLogVec(bonus_points_, {qli_points...});
  }
private:
  std::vector<int> bonus_points_;
};
class GrQLiFixedBonusPoints {
public:
  GrQLiFixedBonusPoints(int weight, std::vector<int> bonus_points)
    : weight_(weight), bonus_points_(std::move(bonus_points)) {}
  template<typename... Args>
  GammaExpr operator()(Args... qli_points) const {
    return GrQLiVec(weight_, bonus_points_, {qli_points...});
  }
private:
  int weight_;
  std::vector<int> bonus_points_;
};
}  // namespace internal


template<typename... Args> internal::GrLogFixedBonusPoints GrLog(Args... args) { return {{args...}}; }

// Usage:
//   GrQLi_w(q_1, ..., q_m)(p_1, ..., p_n)
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi1(Args... args) { return {1, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi2(Args... args) { return {2, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi3(Args... args) { return {3, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi4(Args... args) { return {4, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi5(Args... args) { return {5, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi6(Args... args) { return {6, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi7(Args... args) { return {7, {args...}}; }
template<typename... Args> internal::GrQLiFixedBonusPoints GrQLi8(Args... args) { return {8, {args...}}; }
