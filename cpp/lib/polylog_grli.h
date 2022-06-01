// Grassmannian polylogarithm

#pragma once

#include "gamma.h"


GammaExpr GrLiVec(const std::vector<int>& bonus_points, const std::vector<int>& main_points);


namespace internal {
class GrLiFixedBonusPoints {
public:
  GrLiFixedBonusPoints(std::vector<int> bonus_points)
    : bonus_points_(std::move(bonus_points)) {}
  template<typename... Args>
  GammaExpr operator()(Args... qli_points) const {
    return GrLiVec(bonus_points_, {qli_points...});
  }
private:
  std::vector<int> bonus_points_;
};
}  // namespace internal

template<typename... Args> internal::GrLiFixedBonusPoints GrLi(Args... args) { return {{args...}}; }
