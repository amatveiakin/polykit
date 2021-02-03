#pragma once

#include "algebra.h"
#include "lyndon.h"


template<typename CoExprT, typename ExprT>
CoExprT coproduct(const ExprT& lhs, const ExprT& rhs) {
  using CoMonomT = typename CoExprT::StorageT;
  constexpr int is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  const auto& lhs_fixed = is_lie_algebra ? to_lyndon_basis(lhs) : lhs;
  const auto& rhs_fixed = is_lie_algebra ? to_lyndon_basis(rhs) : rhs;
  auto ret = outer_product<CoExprT>(
    lhs_fixed,
    rhs_fixed,
    [](const auto& u, const auto& v) {
      return CoMonomT({u, v});
    },
    AnnOperator(is_lie_algebra ? fmt::coprod_lie() : fmt::coprod_hopf())
  );
  if constexpr (is_lie_algebra) {
    return normalize_coproduct(ret);
  } else {
    return ret;  // `normalize_coproduct` might not compile here, thus `if constexpr`
  }
}

// TODO: Should this be exposed publicly?
template<typename CoExprT>
CoExprT normalize_coproduct(const CoExprT& expr) {
  CHECK(CoExprT::Param::coproduct_is_lie_algebra);
  using CoMonomT = typename CoExprT::StorageT;
  CoExprT ret;
  expr.foreach_key([&](const auto& key, int coeff) {
    CHECK_EQ(key.size(), 2);
    const auto& key1 = key[0];
    const auto& key2 = key[1];
    if (key1.size() == key2.size()) {
      if (key1 == key2) {
        // zero: through away
      } else if (key1 < key2) {
        ret.add_to_key(key, coeff);
      } else {
        ret.add_to_key(CoMonomT({key2, key1}), -coeff);
      }
    } else {
      if (key1.size() < key2.size()) {
        ret.add_to_key(key, coeff);
      } else {
        ret.add_to_key(CoMonomT({key2, key1}), -coeff);
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

  using MonomT = typename ExprT::StorageT;
  // TODO: Make sure there is no converting to/from vector form back and forth.
  static auto make_copart = [](auto span) {
    // TODO: Fix: Lyndon is repeated here and in coproduct!
    return to_lyndon_basis(ExprT::single_key(
      ExprT::Param::vector_to_key(typename ExprT::Param::VectorT(span))
    ));
  };
  CoExprT ret;
  expr.foreach_key([&](const MonomT& monom, int coeff) {
    const auto& monom_vec = ExprT::Param::key_to_vector(monom);
    CHECK_EQ(monom_vec.size(), weight);
    const auto span = absl::MakeConstSpan(monom_vec);
    const int split = form.first;
    ret += coeff * coproduct<CoExprT>(
      make_copart(span.subspan(0, split)),
      make_copart(span.subspan(split))
    );
    if (form.first != form.second) {
      const int split = form.second;
      ret -= coeff * coproduct<CoExprT>(
        make_copart(span.subspan(split)),
        make_copart(span.subspan(0, split))
      );
    }
  });
  return ret.copy_annotations_mapped(
    expr, [](const std::string& annotation) {
      return fmt::comult() + annotation;
    }
  );
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
  return filter_coexpr_predicate(
    expr, side,
    [&](const typename CoExprT::ObjectT::value_type& x) {
      return x == value;
    }
  );
}
