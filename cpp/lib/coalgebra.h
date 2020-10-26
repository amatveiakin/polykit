// TODO: Normalization parameters describing what can be swapped.
// Right now we apparently have Lie algebras here.

#pragma once

#include "algebra.h"
#include "delta.h"
#include "lyndon.h"
#include "multiword.h"
#include "word.h"


namespace internal {
constexpr const char kCoprodSign[] = " ^ ";

struct WordCoExprParam : SimpleLinearParam<MultiWord> {
  static std::string object_to_string(const MultiWord& word) {
    std::vector<std::string> segment_strings;
    for (const auto& segment : word) {
      segment_strings.push_back(list_to_string(segment));
    }
    return str_join(segment_strings, kCoprodSign);
  }
};

struct DeltaCoExprParam {
  using ObjectT = std::vector<std::vector<Delta>>;
  using StorageT = MultiWord;
  static StorageT object_to_key(const ObjectT& obj) {
    MultiWord ret;
    for (const std::vector<Delta>& deltas : obj) {
      ret.append_segment(DeltaExprParam::object_to_key(deltas));
    }
    return ret;
  }
  static ObjectT key_to_object(const StorageT& key) {
    std::vector<std::vector<Delta>> ret;
    for (auto it = key.begin(); it != key.end(); ++it) {
      ret.push_back(mapped(*it, [](int ch) {
        return delta_alphabet_mapping.from_alphabet(ch);
      }));
    }
    return ret;
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, kCoprodSign, DeltaExprParam::object_to_string);
  }
};
}  // namespace internal

using WordCoExpr = Linear<internal::WordCoExprParam>;
using DeltaCoExpr = Linear<internal::DeltaCoExprParam>;


template<typename CoExprT, typename ExprT>
CoExprT coproduct(const ExprT& lhs, const ExprT& rhs) {
  static_assert(std::is_same_v<typename ExprT::StorageT, Word>);
  static_assert(std::is_same_v<typename CoExprT::StorageT, MultiWord>);
  const auto lhs_lyndon = to_lyndon_basis(lhs);
  const auto rhs_lyndon = to_lyndon_basis(rhs);
  auto ret = outer_product<CoExprT>(
    lhs_lyndon,
    rhs_lyndon,
    [](const Word& u, const Word& v) {
      MultiWord prod;
      prod.append_segment(u);
      prod.append_segment(v);
      return prod;
    });
  return normalize_coproduct(ret);
}

// Explicit rules allow to omit template types when calling the function.
DeltaCoExpr coproduct(const DeltaExpr& lhs, const DeltaExpr& rhs) {
  return coproduct<DeltaCoExpr>(lhs, rhs);
}
WordCoExpr coproduct(const WordExpr& lhs, const WordExpr& rhs) {
  return coproduct<WordCoExpr>(lhs, rhs);
}


// TODO: Should the be exposed publicly?
template<typename CoExprT>
CoExprT normalize_coproduct(const CoExprT& expr) {
  static_assert(std::is_same_v<typename CoExprT::StorageT, MultiWord>);
  CoExprT ret;
  expr.foreach_key([&](const MultiWord& key, int coeff) {
    CHECK_EQ(key.num_segments(), 2);
    const auto& key1 = key.segment(0);
    const auto& key2 = key.segment(1);
    if (key1.size() == key2.size()) {
      if (key1 < key2) {
        ret += coeff * CoExprT::single_key(key);
      } else {
        MultiWord key_swapped;
        key_swapped.append_segment(key2);
        key_swapped.append_segment(key1);
        ret -= coeff * CoExprT::single_key(key_swapped);
      }
    }
  });
  return ret;
}


// Optimization potential: convert expr to Lyndon basis first to minimize
// the number of times individual terms need to be converted.
template<typename CoExprT, typename ExprT>
CoExprT comultiply(const ExprT& expr, std::pair<int, int> form) {
  if (expr.zero()) {
    return {};
  }
  const int weight = expr.weight();
  CHECK_EQ(form.first + form.second, weight);

  CoExprT ret;
  expr.foreach_key([&](const Word& word, int coeff) {
    CHECK_EQ(word.size(), weight);
    const int split = form.first;
    ret += coeff * coproduct<CoExprT>(
      to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(0, split)))),
      to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(split))))
    );
    if (form.first != form.second) {
      // TODO: Consider moving this logic to coproduct/normalize_coproduct
      const int split = form.second;
      ret -= coeff * coproduct<CoExprT>(
        to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(split)))),
        to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(0, split))))
      );
    }
  });
  return ret;
}

// Explicit rules allow to omit template types when calling the function.
DeltaCoExpr comultiply(const DeltaExpr& expr, std::pair<int, int> form) {
  return comultiply<DeltaCoExpr>(expr, form);
}
WordCoExpr comultiply(const WordExpr& expr, std::pair<int, int> form) {
  return comultiply<WordCoExpr>(expr, form);
}
