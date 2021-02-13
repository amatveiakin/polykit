// The result of substituting cross ratios into a generic polylog function (Li).
// The name "Lira" stands for "LI of a RAtio".
//
// The interface is similar to that of Li/CoLi, but it takes CompoundRatio-s instead
// of variable products. Note that a CompoundRatio can be implicitly constructed from
// a CrossRatio, meaning that the latter can be passed as well. Sample usages:
//   * Lira(3,2)(CR(2,1,7,8), CR(3,6,1,2));
//   * Lira(3,2)(CR(2,1,7,8), CR(3,4,5,6) * CR(3,6,1,2));

#pragma once

#include "coalgebra.h"
#include "polylog_lira_param.h"
#include "theta.h"


ThetaExpr LiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios);
ThetaExpr LiraVec(const LiraParam& param);

ThetaCoExpr CoLiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios);
ThetaCoExpr CoLiraVec(const LiraParam& param);


ThetaExpr eval_formal_symbols(const ThetaExpr& expr);
ThetaCoExpr eval_formal_symbols(const ThetaCoExpr& expr);


namespace internal {
class LiraFixedWeights {
public:
  LiraFixedWeights(std::vector<int> weights)
    : weights_(std::move(weights)) {}
  template<typename... Args>
  ThetaExpr operator()(Args&&... args) const {
    return LiraVec(1, weights_, std::vector<CompoundRatio>{std::forward<Args>(args)...});
  }
private:
  std::vector<int> weights_;
};

class CoLiraFixedWeights {
public:
  CoLiraFixedWeights(std::vector<int> weights)
    : weights_(std::move(weights)) {}
  template<typename... Args>
  ThetaCoExpr operator()(Args&&... args) const {
    return CoLiraVec(1, weights_, std::vector<CompoundRatio>{std::forward<Args>(args)...});
  }
private:
  std::vector<int> weights_;
};
}  // namespace internal


// Usage:
//   Lira(w_1, ..., w_n)(r_1, ..., r_n)
template<typename... Args>
internal::LiraFixedWeights Lira(Args... args) {
  return {{args...}};
}

// Usage:
//   CoLira(w_1, ..., w_n)(r_1, ..., r_n)
template<typename... Args>
internal::CoLiraFixedWeights CoLira(Args... args) {
  return {{args...}};
}
