// TODO: Rename module to 'polylog'
// TODO: Allow to pass LiParam as arg

#pragma once

#include "coalgebra.h"
#include "epsilon.h"
#include "polylog_param.h"


EpsilonExpr LiVec(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

EpsilonCoExpr CoLi(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

namespace internal {
class LiFixedWeights {
public:
  LiFixedWeights(std::vector<int> weights)
    : weights_(std::move(weights)) {}
  template<typename... Args>
  EpsilonExpr operator()(std::initializer_list<Args>... args) const {
    return LiVec(weights_, std::vector({std::vector(args)...}));
  }
private:
  std::vector<int> weights_;
};
}  // namespace internal

// Usage:
//   Li(w_1, ..., w_n)({p_1_1, ..., p_1_k}, ..., {p_n_1, ..., p_n_l})
template<typename... Args>
internal::LiFixedWeights Li(Args... args) {
  return {{args...}};
}
