#pragma once

#include "absl/types/span.h"

#include "linear.h"
#include "word.h"


template<typename LinearProdT, typename LinearT, typename MonomProdF>
LinearProdT outer_product(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product) {
  static_assert(std::is_same_v<
      std::invoke_result_t<MonomProdF, typename LinearT::StorageT, typename LinearT::StorageT>,
      typename LinearProdT::StorageT>);
  LinearProdT ret;
  lhs.foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      const auto ret_key = monom_key_product(lhs_key, rhs_key);
      assert(ret.coeff_for_key(ret_key) == 0);  // outer product must be unique
      ret.add_to_key(ret_key, lhs_coeff * rhs_coeff);
    });
  });
  return LinearProdT(ret);
}

template<typename LinearT, typename MonomProdF>
LinearT outer_product(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product) {
  if (expressions.empty()) {
    return {};
  }
  LinearT ret = expressions.front();
  for (const LinearT& expr : expressions.subspan(1)) {
    ret = outer_product<LinearT>(ret, expr, monom_key_product);
  }
  return ret;
}


// Similar to outer project, but monom_key_product produces an expr rather than one monom.
template<typename LinearT, typename MonomProdF>
LinearT outer_product_expanding(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product) {
  static_assert(std::is_same_v<
      std::invoke_result_t<MonomProdF, typename LinearT::StorageT, typename LinearT::StorageT>,
      LinearT>);
  LinearT ret;
  lhs.foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      const LinearT& prod = monom_key_product(lhs_key, rhs_key);
      ret += (lhs_coeff * rhs_coeff) * prod;
    });
  });
  return LinearT(ret);
}

template<typename LinearT, typename MonomProdF>
LinearT outer_product_expanding(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product) {
  if (expressions.empty()) {
    return {};
  }
  LinearT ret = expressions.front();
  for (const LinearT& expr : expressions.subspan(1)) {
    ret = outer_product_expanding(ret, expr, monom_key_product);
  }
  return ret;
}


template<typename LinearT>
LinearT tensor_product(
    const LinearT& lhs,
    const LinearT& rhs) {
  return outer_product<LinearT>(lhs, rhs, concat_words);
}

template<typename LinearT>
LinearT tensor_product(
    const absl::Span<const LinearT>& expressions) {
  return outer_product(expressions, concat_words);
}
