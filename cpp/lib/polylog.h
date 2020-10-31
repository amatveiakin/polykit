#pragma once

#include "coalgebra.h"
#include "epsilon.h"
#include "polylog_param.h"
#include "theta.h"


EpsilonExpr LiVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

EpsilonExpr LiVec(
    const LiParam& param);

EpsilonCoExpr CoLiVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);

EpsilonCoExpr CoLiVec(
    const LiParam& param);

ThetaExpr eval_formal_symbols(const ThetaExpr& expr);
ThetaCoExpr eval_formal_symbols(const ThetaCoExpr& expr);


namespace internal {
class LiFixedWeights {
public:
  LiFixedWeights(std::vector<int> weights)
    : weights_(std::move(weights)) {}
  template<typename... Args>
  EpsilonExpr operator()(std::initializer_list<Args>... args) const {
    return LiVec(1, weights_, std::vector({std::vector(args)...}));
  }
private:
  std::vector<int> weights_;
};

class CoLiFixedWeights {
public:
  CoLiFixedWeights(std::vector<int> weights)
    : weights_(std::move(weights)) {}
  template<typename... Args>
  EpsilonCoExpr operator()(std::initializer_list<Args>... args) const {
    return CoLiVec(1, weights_, std::vector({std::vector(args)...}));
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

// Usage:
//   CoLi(w_1, ..., w_n)({p_1_1, ..., p_1_k}, ..., {p_n_1, ..., p_n_l})
template<typename... Args>
internal::CoLiFixedWeights CoLi(Args... args) {
  return {{args...}};
}
