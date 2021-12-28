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

ThetaICoExpr CoLiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios);
ThetaICoExpr CoLiraVec(const LiraParam& param);


ThetaExpr eval_formal_symbols(const ThetaExpr& expr);
ThetaICoExpr eval_formal_symbols(const ThetaICoExpr& expr);


namespace internal {
class LiraFixedWeights {
public:
  LiraFixedWeights(int foreweight, std::vector<int> weights)
    : foreweight_(foreweight), weights_(std::move(weights)) {}
  template<typename... Args>
  ThetaExpr operator()(Args&&... args) const {
    return LiraVec(foreweight_, weights_, std::vector<CompoundRatio>{std::forward<Args>(args)...});
  }
private:
  int foreweight_;
  std::vector<int> weights_;
};

class CoLiraFixedWeights {
public:
  CoLiraFixedWeights(int foreweight, std::vector<int> weights)
    : foreweight_(foreweight), weights_(std::move(weights)) {}
  template<typename... Args>
  ThetaICoExpr operator()(Args&&... args) const {
    return CoLiraVec(foreweight_, weights_, std::vector<CompoundRatio>{std::forward<Args>(args)...});
  }
private:
  int foreweight_;
  std::vector<int> weights_;
};
}  // namespace internal


// Usage:
//   Lira(w_1, ..., w_n)(r_1, ..., r_n)
template<typename... Args> internal::LiraFixedWeights Lira (Args... args) { return {0, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira0(Args... args) { return {0, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira1(Args... args) { return {1, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira2(Args... args) { return {2, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira3(Args... args) { return {3, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira4(Args... args) { return {4, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira5(Args... args) { return {5, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira6(Args... args) { return {6, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira7(Args... args) { return {7, {args...}}; }
template<typename... Args> internal::LiraFixedWeights Lira8(Args... args) { return {8, {args...}}; }

// Usage:
//   CoLira(w_1, ..., w_n)(r_1, ..., r_n)
template<typename... Args> internal::CoLiraFixedWeights CoLira (Args... args) { return {0, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira0(Args... args) { return {0, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira1(Args... args) { return {1, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira2(Args... args) { return {2, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira3(Args... args) { return {3, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira4(Args... args) { return {4, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira5(Args... args) { return {5, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira6(Args... args) { return {6, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira7(Args... args) { return {7, {args...}}; }
template<typename... Args> internal::CoLiraFixedWeights CoLira8(Args... args) { return {8, {args...}}; }
