#pragma once

#include <vector>

#include "algebra.h"
#include "util.h"
#include "word.h"


// Returns shuffle product of two words as a List of words.
// Rules:
//   1 ⧢ v = v
//   u ⧢ 1 = u
//   ua ⧢ vb = (u ⧢ vb)a + (ua ⧢ v)b
WordExpr shuffle_product(const Word& u, const Word& v);

// Returns  w1 ⧢ w2 ⧢ ... ⧢ wn
WordExpr shuffle_product(std::vector<Word> words);


template<typename LinearT>
LinearT shuffle_product_expr(
    const LinearT& lhs,
    const LinearT& rhs) {
  return outer_product_expanding<LinearT>(lhs, rhs,
    static_cast<WordExpr (*)(const Word&, const Word&)>(shuffle_product));
}

template<typename LinearT>
LinearT shuffle_product_expr(
    const absl::Span<const LinearT>& expressions) {
  return outer_product_expanding(expressions,
    static_cast<WordExpr (*)(const Word&, const Word&)>(shuffle_product));
}
