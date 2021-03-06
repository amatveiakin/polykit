// Traditional polylogarithm defined as an iterated integral of cumulative products:
//
//     1_Li_1...1 (x1, ..., xn) = I(0, 1, x1, x1*x2, ..., x1*...*xn)
//     ^    ^^^^^  ^^^^^^^^^^^
//     |   weights    points
// foreweight
//
// The number of weights must be equal to the number of points. Increasing weight k
// by n means adding n zeros before xk. Increasing foreweight by n means adding n zeros
// in the beginning.
//
// This file also contains a function called CoLi, which is a specific form of Li
// comultiplication. It combines symbols and formal symbols and is used as part of a
// proof in https://arxiv.org/pdf/2012.05599.pdf (by Rudenko).
//
// The functions come in two forms (examples use Li, but the same is true for CoLi):
//   * Simple form: `Li(w_1, ..., w_n)({p_1_1, ..., p_1_k}, ..., {p_n_1, ..., p_n_l})`
//     where w_i are weights and each argument is itself a product of x_i. E.g.:
//     `Li(1,3)({1},{2,3})` == 1_Li_1_3(x1, x2*x3) == I(0, 1, x1, 0, 0, x1*x2*x3)
//     Use `LiN` instead of `Li` for foreweight equal to N.
//   * Vector form: `LiVec(foreweight, vector_of_weights, vector_of_arguments)`

#pragma once

#include "coalgebra.h"
#include "epsilon.h"
#include "polylog_li_param.h"


EpsilonExpr LiVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);
EpsilonExpr LiVec(const LiParam& param);

EpsilonCoExpr CoLiVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points);
EpsilonCoExpr CoLiVec(const LiParam& param);


namespace internal {
class LiFixedWeights {
public:
  LiFixedWeights(int foreweight, std::vector<int> weights)
    : foreweight_(foreweight), weights_(std::move(weights)) {}
  template<typename... Args>
  EpsilonExpr operator()(std::initializer_list<Args>... args) const {
    return LiVec(foreweight_, weights_, std::vector({std::vector(args)...}));
  }
private:
  int foreweight_;
  std::vector<int> weights_;
};

class CoLiFixedWeights {
public:
  CoLiFixedWeights(int foreweight, std::vector<int> weights)
    : foreweight_(foreweight), weights_(std::move(weights)) {}
  template<typename... Args>
  EpsilonCoExpr operator()(std::initializer_list<Args>... args) const {
    return CoLiVec(foreweight_, weights_, std::vector({std::vector(args)...}));
  }
private:
  int foreweight_;
  std::vector<int> weights_;
};
}  // namespace internal


// Usage:
//   Li(w_1, ..., w_n)({p_1_1, ..., p_1_k}, ..., {p_n_1, ..., p_n_l})
template<typename... Args> internal::LiFixedWeights Li (Args... args) { return {1, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li1(Args... args) { return {1, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li2(Args... args) { return {2, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li3(Args... args) { return {3, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li4(Args... args) { return {4, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li5(Args... args) { return {5, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li6(Args... args) { return {6, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li7(Args... args) { return {7, {args...}}; }
template<typename... Args> internal::LiFixedWeights Li8(Args... args) { return {8, {args...}}; }

// Usage:
//   CoLi(w_1, ..., w_n)({p_1_1, ..., p_1_k}, ..., {p_n_1, ..., p_n_l})
template<typename... Args> internal::CoLiFixedWeights CoLi (Args... args) { return {1, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi1(Args... args) { return {1, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi2(Args... args) { return {2, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi3(Args... args) { return {3, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi4(Args... args) { return {4, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi5(Args... args) { return {5, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi6(Args... args) { return {6, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi7(Args... args) { return {7, {args...}}; }
template<typename... Args> internal::CoLiFixedWeights CoLi8(Args... args) { return {8, {args...}}; }
