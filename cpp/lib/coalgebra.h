// TODO: Normalization parameters describing what can be swapped.
// Right now we apparently have Lie algebras here.

#pragma once

#include "algebra.h"
#include "delta.h"
#include "epsilon.h"
#include "lyndon.h"
#include "multiword.h"
#include "theta.h"
#include "word.h"


constexpr int kThetaCoExprComponents = 2;

namespace internal {
struct WordCoExprParam : SimpleLinearParam<MultiWord> {
  static std::string object_to_string(const MultiWord& word) {
    std::vector<std::string> segment_strings;
    for (const auto& segment : word) {
      segment_strings.push_back(list_to_string(segment));
    }
    return str_join(segment_strings, fmt::coprod_lie());
  }
  static constexpr bool coproduct_is_lie_algebra = true;
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
      ret.push_back(DeltaExprParam::key_to_object(Word(*it)));
    }
    return ret;
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_lie(), DeltaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
};

struct EpsilonCoExprParam {
  using ObjectT = std::vector<EpsilonPack>;
  using StorageT = MultiWord;
  static StorageT object_to_key(const ObjectT& obj) {
    MultiWord ret;
    for (const EpsilonPack& pack : obj) {
      ret.append_segment(epsilon_pack_to_key(pack));
    }
    return ret;
  }
  static ObjectT key_to_object(const StorageT& key) {
    std::vector<EpsilonPack> ret;
    for (auto it = key.begin(); it != key.end(); ++it) {
      ret.push_back(key_to_epsilon_pack(Word(*it)));
    }
    return ret;
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_hopf());
  }
  static constexpr bool coproduct_is_lie_algebra = false;
};

struct ThetaCoExprParam {
  using ObjectT = std::array<ThetaPack, kThetaCoExprComponents>;
  using StorageT = std::array<MultiWord, kThetaCoExprComponents>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_array(obj, ThetaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped_array(key, ThetaExprParam::key_to_object);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_hopf());
  }
  static constexpr bool coproduct_is_lie_algebra = false;
};

}  // namespace internal

using WordCoExpr = Linear<internal::WordCoExprParam>;
using DeltaCoExpr = Linear<internal::DeltaCoExprParam>;
using EpsilonCoExpr = Linear<internal::EpsilonCoExprParam>;
using ThetaCoExpr = Linear<internal::ThetaCoExprParam>;


template<typename CoExprT, typename ExprT>
CoExprT coproduct(const ExprT& lhs, const ExprT& rhs) {
  static_assert(std::is_same_v<typename ExprT::StorageT, Word>);
  static_assert(std::is_same_v<typename CoExprT::StorageT, MultiWord>);
  constexpr int is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  const auto& lhs_fixed = is_lie_algebra ? to_lyndon_basis(lhs) : lhs;
  const auto& rhs_fixed = is_lie_algebra ? to_lyndon_basis(rhs) : rhs;
  auto ret = outer_product<CoExprT>(
    lhs_fixed,
    rhs_fixed,
    [](const Word& u, const Word& v) {
      MultiWord prod;
      prod.append_segment(u);
      prod.append_segment(v);
      return prod;
    },
    AnnOperator(is_lie_algebra ? fmt::coprod_lie() : fmt::coprod_hopf())
  );
  return is_lie_algebra ? normalize_coproduct(ret) : ret;
}

// Explicit rules allow to omit template types when calling the function.
inline DeltaCoExpr coproduct(const DeltaExpr& lhs, const DeltaExpr& rhs) {
  return coproduct<DeltaCoExpr>(lhs, rhs);
}
inline WordCoExpr coproduct(const WordExpr& lhs, const WordExpr& rhs) {
  return coproduct<WordCoExpr>(lhs, rhs);
}
inline EpsilonCoExpr coproduct(const EpsilonExpr& lhs, const EpsilonExpr& rhs) {
  return coproduct<EpsilonCoExpr>(lhs, rhs);
}


// TODO: Should this be exposed publicly?
template<typename CoExprT>
CoExprT normalize_coproduct(const CoExprT& expr) {
  static_assert(std::is_same_v<typename CoExprT::StorageT, MultiWord>);
  CHECK(CoExprT::Param::coproduct_is_lie_algebra);
  CoExprT ret;
  expr.foreach_key([&](const MultiWord& key, int coeff) {
    CHECK_EQ(key.num_segments(), 2);
    const auto& key1 = key.segment(0);
    const auto& key2 = key.segment(1);
    if (key1.size() == key2.size()) {
      if (key1 == key2) {
        // zero: through away
      } else if (key1 < key2) {
        ret += coeff * CoExprT::single_key(key);
      } else {
        MultiWord key_swapped;
        key_swapped.append_segment(key2);
        key_swapped.append_segment(key1);
        ret -= coeff * CoExprT::single_key(key_swapped);
      }
    } else {
      if (key1.size() < key2.size()) {
        ret += coeff * CoExprT::single_key(key);
      } else {
        MultiWord key_swapped;
        key_swapped.append_segment(key2);
        key_swapped.append_segment(key1);
        ret -= coeff * CoExprT::single_key(key_swapped);
      }
    }
  });
  return ret.copy_annotations(expr);
}


// Optimization potential: convert expr to Lyndon basis first to minimize
// the number of times individual terms need to be converted.
template<typename CoExprT, typename ExprT>
CoExprT comultiply(const ExprT& expr, std::pair<int, int> form) {
  CHECK(CoExprT::Param::coproduct_is_lie_algebra);
  if (expr.zero()) {
    return {};
  }
  const int weight = expr.weight();
  CHECK_EQ(form.first + form.second, weight);
  sort_two(form.first, form.second);  // avoid unnecessary work in `normalize_coproduct`

  CoExprT ret;
  expr.foreach_key([&](const Word& word, int coeff) {
    CHECK_EQ(word.size(), weight);
    const int split = form.first;
    ret += coeff * coproduct<CoExprT>(
      to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(0, split)))),
      to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(split))))
    );
    if (form.first != form.second) {
      const int split = form.second;
      ret -= coeff * coproduct<CoExprT>(
        to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(split)))),
        to_lyndon_basis(ExprT::single_key(Word(word.span().subspan(0, split))))
      );
    }
  });
  return ret.copy_annotations(expr);
}

// Explicit rules allow to omit template types when calling the function.
inline DeltaCoExpr comultiply(const DeltaExpr& expr, std::pair<int, int> form) {
  return comultiply<DeltaCoExpr>(expr, form);
}
inline WordCoExpr comultiply(const WordExpr& expr, std::pair<int, int> form) {
  return comultiply<WordCoExpr>(expr, form);
}
inline EpsilonCoExpr comultiply(const EpsilonExpr& expr, std::pair<int, int> form) {
  return comultiply<EpsilonCoExpr>(expr, form);
}


template<typename CoExprT, typename F>
CoExprT filter_coexpr_predicate(const CoExprT& expr, int side, const F& predicate) {
  return expr.filtered([&](const typename CoExprT::ObjectT& term) {
    return predicate(term.at(side));
  });
}

template<typename CoExprT>
CoExprT filter_coexpr(
    const CoExprT& expr, int side, const typename CoExprT::ObjectT::value_type& value) {
  return filter_coexpr_predicate(expr, side,
    [&](const typename CoExprT::ObjectT::value_type& x) {
      return x == value;
    });
}
