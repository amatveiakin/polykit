#pragma once

#include "linear.h"
#include "shuffle.h"
#include "util.h"


// Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
// Such split always exists and is unique (Chen–Fox–Lyndon theorem).
template<typename Container>
inline std::vector<Container> lyndon_factorize(const Container& word) {
  const int n = word.size();
  int start = 0;
  int k = start;
  int m = start + 1;
  std::vector<Container> ret;
  while (k < n) {
    if (m >= n || word[k] > word[m]) {
      const int l = m - k;
      ret.push_back(Container(word.begin() + start, word.begin() + start + l));
      start += l;
      k = start;
      m = start + 1;
    } else if (word[k] < word[m]) {
      k = start;
      m++;
    } else {
      k++;
      m++;
    }
  }
  if (start < n) {
    ret.push_back(Container(word.begin() + start, word.end()));
  }
  return ret;
}


// Converts a linear combination of words to Lyndon basis.
//
// Optimization potential: Cache word_orig => words_expanded.
// Optimization potential: Sort summands and add replacement directly to the expression.
//   Thus avoid processing the same Lyndon word twice.
template<typename LinearT>
LinearT to_lyndon_basis(const LinearT& expression) {
  bool finished = false;
  auto expr = to_vector_expression(expression.without_annotations());
  using VectorLinearT = decltype(expr);
  while (!finished) {
    VectorLinearT expr_new;
    finished = true;
    expr.foreach_key([&](const auto& word_orig, int coeff) {
      const auto& lyndon_words = lyndon_factorize(word_orig);
      CHECK(!lyndon_words.empty());
      if (lyndon_words.size() == 1) {
        expr_new.add_to_key(word_orig, coeff);
        return;
      }
      finished = false;
      const auto& lyndon_expr = VectorLinearT::from_key_collection(lyndon_words);
      int denominator = 1;
      lyndon_expr.foreach_key([&denominator](const auto&, int coeff) {
        denominator *= factorial(coeff);
      });
      auto shuffle_expr = shuffle_product(lyndon_words).template cast_to<VectorLinearT>();
      shuffle_expr.div_int(denominator);
      CHECK_EQ(shuffle_expr.coeff_for_key(word_orig), 1);
      shuffle_expr.add_to_key(word_orig, -1);
      expr_new += (-coeff) * shuffle_expr;
    });
    expr = std::move(expr_new);
  };
  LinearT ret = from_vector_expression<LinearT>(expr);
  return ret.copy_annotations(expression);
}
