// TODO: Lily -> Li
// TODO: Rename module to 'polylog'
// TODO: Allow to pass LiParam as arg

#pragma once

#include "coalgebra.h"
#include "epsilon.h"
#include "polylog_param.h"


EpsilonExpr LilyVec(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

EpsilonCoExpr CoLi(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

namespace internal {
class LilyFixedWeights {
public:
  LilyFixedWeights(std::vector<int> weights)
    : weights_(std::move(weights)) {}
  template<typename... Args>
  EpsilonExpr operator()(std::initializer_list<Args>... args) const {
    return LilyVec(weights_, std::vector({std::vector(args)...}));
  }
private:
  std::vector<int> weights_;
};
}  // namespace internal

// Usage:
//   Lily(w_1, ..., w_n)({p_1_1, ..., p_1_k}, ..., {p_n_1, ..., p_n_l})
template<typename... Args>
internal::LilyFixedWeights Lily(Args... args) {
  return {{args...}};
}
