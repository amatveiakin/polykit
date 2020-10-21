#pragma once

#include "linear.h"
#include "word.h"


template<typename ParamT, typename MonomProdF>
Linear<ParamT> outer_product(
    const Linear<ParamT>& lhs,
    const Linear<ParamT>& rhs,
    const MonomProdF& monom_key_product) {
  Linear<ParamT> ret;
  lhs.main().foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.main().foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      const auto& ret_key = monom_key_product(lhs_key, rhs_key);
      assert(ret.main().coeff_for_key(ret_key) == 0);  // outer product must be unique
      ret += (lhs_coeff * rhs_coeff) * Linear<ParamT>::single_key(ret_key);
    });
  });
  return Linear<ParamT>(ret);
}

template<typename ParamT>
Linear<ParamT> tensor_product(
    const Linear<ParamT>& lhs,
    const Linear<ParamT>& rhs) {
  return outer_product(lhs, rhs, concat_words);
}
