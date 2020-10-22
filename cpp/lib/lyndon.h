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
//   Better yet: sort summands and add replacement directly to the expression.
//   Thus avoid processing the same Lyndon word twice.
// Optimization potential: Don't generate all (N!) results for each (word^N).
template<typename ParamT>
Linear<ParamT> to_lyndon_basis(const Linear<ParamT>& expression) {
  using LinearT = Linear<ParamT>;
  static_assert(std::is_same_v<typename ParamT::StorageT, IntWord>);
  bool finished = false;
  LinearT expr(expression.main());
  while (!finished) {
    LinearT expr_new;
    finished = true;
    expr.main().foreach_key([&](const IntWord& word_orig, int coeff) {
      const auto& lyndon_words = lyndon_factorize(word_orig);
      CHECK(!lyndon_words.empty());
      if (lyndon_words.size() == 1) {
        expr_new += coeff * LinearT::single_key(word_orig);
        return;
      }
      finished = false;
      const auto& lyndon_expr = LinearT::from_key_collection(lyndon_words);
      int denominator = 1;
      lyndon_expr.main().foreach_key([&denominator](const auto&, int coeff) {
        denominator *= factorial(coeff);
      });
      LinearT shuffle_expr = LinearT::from_key_collection(
        shuffle_product(lyndon_words)
      );
      shuffle_expr.div_int(denominator);
      CHECK_EQ(shuffle_expr.main().coeff_for_key(word_orig), 1);
      expr_new += (-coeff) * (shuffle_expr - LinearT::single_key(word_orig));
    });
    expr = std::move(expr_new);
  };
  expr.copy_annotations(expression);
  return expr;
}
