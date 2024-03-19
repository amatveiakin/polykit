// Shuffle product.
//
// Shuffle rules:
//   1 ⧢ v = v
//   u ⧢ 1 = u
//   ua ⧢ vb = (u ⧢ vb)a + (ua ⧢ v)b
//
// Shuffle can be applied to individual words or to entire linear expressions,
// in which case the expressions must support vector form.

#pragma once

#include <vector>

#include "algebra.h"
#include "util.h"
#include "shuffle_unrolled.h"
#include "shuffle_unrolled_multi.h"


template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product(const MonomT& u, const MonomT& v) {
  using LinearT = BasicLinear<SimpleLinearParam<MonomT>>;
  if (u.empty() && v.empty()) {
    return {};
  }
  if (u.empty()) {
    return LinearT::single_key(v);
  }
  if (v.empty()) {
    return LinearT::single_key(u);
  }
#if UNROLL_SHUFFLE
  {
    auto unrolled_ret = shuffle_product_unrolled(u, v);
    if (!unrolled_ret.is_zero()) {
      return unrolled_ret;
    }
  }
#endif
  const auto a = u.back();
  const auto b = v.back();
  MonomT u_trunc = u;
  MonomT v_trunc = v;
  u_trunc.pop_back();
  v_trunc.pop_back();
  LinearT ret;
  shuffle_product(u, v_trunc).foreach_key([&](MonomT w, int coeff) {
    w.push_back(b);
    ret.add_to_key(w, coeff);
  });
  shuffle_product(u_trunc, v).foreach_key([&](MonomT w, int coeff) {
    w.push_back(a);
    ret.add_to_key(w, coeff);
  });
  return ret;
}

// Returns  w1 ⧢ w2 ⧢ ... ⧢ wn
template<typename MonomT>
BasicLinear<SimpleLinearParam<MonomT>> shuffle_product(std::vector<MonomT> words) {
  // Optimization potential: unroll for 3-4 words.
  using LinearT = BasicLinear<SimpleLinearParam<MonomT>>;
  if (words.size() == 0) {
    return {};
  } else if (words.size() == 1) {
    return LinearT::single_key(words[0]);
  } else if (words.size() == 2) {
    return shuffle_product(words[0], words[1]);
  } else {
#if UNROLL_SHUFFLE_MULTI
      {
        // May change `words` order, but that's ok: shuffle is commutative.
        auto unrolled_ret = shuffle_product_unrolled_multi(words);
        if (!unrolled_ret.is_zero()) {
          return unrolled_ret;
        }
      }
#endif
    MonomT w_tail = words.back();
    words.pop_back();
    const LinearT shuffle_product_head = shuffle_product(words);
    return shuffle_product_head.mapped_key_expanding([&](MonomT w_head) {
      return shuffle_product(w_head, w_tail);
    });
  }
}


template<typename LinearT>
LinearT shuffle_product_expr(
    const LinearT& lhs,
    const LinearT& rhs) {
  static_assert(is_any_linear_v<LinearT>);
  return from_vector_expression<LinearT>(
    outer_product_expanding(
      to_vector_expression(lhs),
      to_vector_expression(rhs),
      [](const auto& u, const auto& v) { return shuffle_product(u, v); },
      []() { return AnnFunctionOp("shuffle"); }
    )
  );
}

template<typename LinearT>
LinearT shuffle_product_expr(
    const absl::Span<const LinearT>& expressions) {
  static_assert(is_any_linear_v<LinearT>);
  return from_vector_expression<LinearT>(
    outer_product_expanding(
      absl::MakeConstSpan(
        mapped(expressions, [](const LinearT& expr) {
          return to_vector_expression(expr);
        })
      ),
      [](const auto& u, const auto& v) { return shuffle_product(u, v); },
      []() { return AnnFunctionOp("shuffle"); }
    )
  );
}
