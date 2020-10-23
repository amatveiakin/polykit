#pragma once

#include "linear.h"
#include "shuffle.h"
#include "util.h"
#include "word.h"


// Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
// Such split always exists and is unique (Chen–Fox–Lyndon theorem).
inline std::vector<IntWord> lyndon_factorize(const IntWord& word) {
  const int n = word.size();
  int start = 0;
  int k = start;
  int m = start + 1;
  std::vector<IntWord> ret;
  while (k < n) {
    if (m >= n || word[k] > word[m]) {
      const int l = m - k;
      ret.push_back(IntWord(word.begin() + start, word.begin() + start + l));
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
    ret.push_back(IntWord(word.begin() + start, word.end()));
  }
  return ret;
}


// Converts a linear combination of words to Lyndon basis.
//
// Optimization potential: Cache word_orig => words_expanded.
// Optimization potential: Sort summands and add replacement directly to the expression.
//   Thus avoid processing the same Lyndon word twice.
template<typename ParamT>
Linear<ParamT> to_lyndon_basis(const Linear<ParamT>& expression) {
  using LinearT = Linear<ParamT>;
  static_assert(std::is_same_v<typename ParamT::StorageT, IntWord>);
  bool finished = false;
  LinearT expr = expression.without_annotations();
  while (!finished) {
    LinearT expr_new;
    finished = true;
    expr.foreach_key([&](const IntWord& word_orig, int coeff) {
      const auto& lyndon_words = lyndon_factorize(word_orig);
      CHECK(!lyndon_words.empty());
      if (lyndon_words.size() == 1) {
        expr_new += coeff * LinearT::single_key(word_orig);
        return;
      }
      finished = false;
      const auto& lyndon_expr = LinearT::from_key_collection(lyndon_words);
      int denominator = 1;
      lyndon_expr.foreach_key([&denominator](const auto&, int coeff) {
        denominator *= factorial(coeff);
      });
      LinearT shuffle_expr = shuffle_product(lyndon_words).cast_to<LinearT>();
      shuffle_expr.div_int(denominator);
      CHECK_EQ(shuffle_expr.coeff_for_key(word_orig), 1);
      shuffle_expr.add_to_key(word_orig, -1);
      expr_new += (-coeff) * shuffle_expr;
    });
    expr = std::move(expr_new);
  };
  expr.copy_annotations(expression);
  return expr;
}
