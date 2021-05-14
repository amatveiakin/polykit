#pragma once

#include "compare.h"
#include "linear.h"
#include "shuffle.h"
#include "util.h"


// Splits the word into a sequence of nonincreasing Lyndon words using Duval algorithm.
// Such split always exists and is unique (Chen–Fox–Lyndon theorem).
//
// TODO: Consider returning a vector of `Span`s instead: (a) this could be faster;
//   (b) this way Linear::StorageT == std::array would be supported. Open question:
//   how to implement shuffle afterwards?
template<typename Container, typename Compare>
inline std::vector<Container> lyndon_factorize(const Container& word, const Compare& comp) {
  const int n = word.size();
  int start = 0;
  int k = start;
  int m = start + 1;
  std::vector<Container> ret;
  while (k < n) {
    if (m >= n || comp(word[m], word[k])) {
      const int l = m - k;
      ret.push_back(Container(word.begin() + start, word.begin() + start + l));
      start += l;
      k = start;
      m = start + 1;
    } else if (comp(word[k], word[m])) {
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
//
// Optimization potential: Unroll Lyndon for words of 3-4 characters (consider
//   autogeneration, as for shuffle).
// Optimization potential: In case of words of 1-2 characters there is no need
//   to build the priority queue. In order to keep the ability to compute Lyndon
//   basis for a mixed-weight expression, we could check length when filling the
//   queue.
template<typename LinearT>
LinearT to_lyndon_basis(const LinearT& expression) {
  auto expr = to_vector_expression(expression.without_annotations());
  using VectorLinearT = decltype(expr);
  // Optimization potential:
  //   * Replace std::map with absl::btree_map.
  //   * Replace std::map with a heap.
  //     Note: std::make_heap and std::pop_heap can be used as-is, but std::push_heap
  //     needs to be replaced with a custom implementation that supports data merges.
  std::map terms_to_convert{
    expr.main().data().begin(),
    expr.main().data().end(),
    cmp::lexicographical(cmp::greater_from_less(&LinearT::Param::lyndon_compare))
  };
  VectorLinearT terms_converted;

  while (!terms_to_convert.empty()) {
    const auto [word, coeff] = *terms_to_convert.begin();
    terms_to_convert.erase(terms_to_convert.begin());
    if (coeff == 0) {
      continue;
    }

    if (word.size() == 1) {
      terms_converted.add_to_key(word, coeff);
      continue;
    } else if (word.size() == 2) {
      if (LinearT::Param::lyndon_compare(word[0], word[1])) {
        terms_converted.add_to_key(word, coeff);
      } else if (LinearT::Param::lyndon_compare(word[1], word[0])) {
        terms_converted.add_to_key({word[1], word[0]}, -coeff);
      } else {
        // though away: zero
      }
      continue;
    }

    const auto& lyndon_words = lyndon_factorize(word, &LinearT::Param::lyndon_compare);
    CHECK(!lyndon_words.empty());
    if (lyndon_words.size() == 1) {
      terms_converted.add_to_key(word, coeff);
      continue;
    }

    const auto& lyndon_expr = VectorLinearT::from_key_collection(lyndon_words);
    int denominator = 1;
    lyndon_expr.foreach_key([&denominator](const auto&, int coeff) {
      denominator *= factorial(coeff);
    });

    auto shuffle_expr = shuffle_product(lyndon_words).template cast_to<VectorLinearT>();
    shuffle_expr.div_int(denominator);
    CHECK_EQ(shuffle_expr.coeff_for_key(word), 1);
    shuffle_expr.add_to_key(word, -1);
    for (const auto& [key, inner_coeff] : key_view(&shuffle_expr)) {
      ASSERT(terms_converted.coeff_for_key(key) == 0);
      terms_to_convert[key] -= coeff * inner_coeff;
    }
  };

  LinearT ret = from_vector_expression<LinearT>(terms_converted);
  return ret.copy_annotations(expression);
}
