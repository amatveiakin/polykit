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
// Algorithm. Put expression terms into a priority queue, largest term first.
// Since shuffle equation for Lyndon factorization always produces smaller words
// than the original word, this guarantees we'll process every word only once.
// For each term in the queue:
//   * If the term is already a Lyndon word, move it to the list of known Lyndon
//     words.
//   * If the term is not a Lyndon word, factorize it into Lyndon words and apply
//     shuffle equation. Replace the term with shuffle result. Put known Lyndon
//     words directly to Lyndon words list - no need to process them again. Put
//     everything else into the common queue.
// Continue until the queue is empty. Now the list of known Lyndon words contains
// entire expression in Lyndon basis.
template<typename LinearT>
LinearT to_lyndon_basis(const LinearT& expression) {
  auto expr = to_vector_expression(expression.without_annotations());
  using VectorLinearT = decltype(expr);
  using VectorT = typename VectorLinearT::StorageT;
  // Optimization potential: Replace std::map with a heap. Note: std::make_heap and
  //   std::pop_heap can be used as-is, but std::push_heap needs to be replaced with
  //   a custom implementation that supports data merges.
  std::map<VectorT, int, std::greater<>> terms_to_convert(
    expr.main().data().begin(), expr.main().data().end()
  );
  // Not using a linear to avoid discarding terms with coeff == 0 in the process.
  absl::flat_hash_map<VectorT, int> terms_converted;

  while (!terms_to_convert.empty()) {
    const auto [word_orig, coeff] = *terms_to_convert.begin();
    terms_to_convert.erase(terms_to_convert.begin());

    const auto& lyndon_words = lyndon_factorize(word_orig);
    CHECK(!lyndon_words.empty());
    if (lyndon_words.size() == 1) {
      terms_converted[word_orig] += coeff;
      continue;
    }

    const auto& lyndon_expr = VectorLinearT::from_key_collection(lyndon_words);
    int denominator = 1;
    lyndon_expr.foreach_key([&denominator](const auto&, int coeff) {
      denominator *= factorial(coeff);
    });

    auto shuffle_expr = shuffle_product(lyndon_words).template cast_to<VectorLinearT>();
    shuffle_expr.div_int(denominator);
    CHECK_EQ(shuffle_expr.coeff_for_key(word_orig), 1);
    shuffle_expr.add_to_key(word_orig, -1);

    for (const auto& [key, inner_coeff] : key_view(&shuffle_expr)) {
      const auto it = terms_converted.find(key);
      const int addition = -coeff * inner_coeff;
      if (it != terms_converted.end()) {
        it->second += addition;
      } else {
        terms_to_convert[key] += addition;
      }
    }
  };

  LinearT ret = from_vector_expression<LinearT>(VectorLinearT(
    typename VectorLinearT::BasicLinearMain(std::move(terms_converted)), LinearAnnotation()
  ));
  return ret.copy_annotations(expression);
}
