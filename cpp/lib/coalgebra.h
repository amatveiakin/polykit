// Functions related to comultiplication.
//
// Contains two main functions:
//
//   * `coproduct(expr1, expr2, ..., exprN)` constructs a formal tuple
//     (expr1, expr2, ..., exprN) that corresponds to one comultiplication term.
//     This is not a mathematical operation, this is merely a tool for constructing
//     the comultiplication.
//     `coproduct_vec(expressions)` same as above, vector form.
//
//   * `comultiply(expression, form)` computes a specified comultiplication component.
//     `form` must be a pair of integers such that the sum is equal to `expression`
//     weight. The order of elements in `form` does not matter: comultiplication is
//     always ordered so that the smaller weight goes first.
//     Example: `comultiply(QLi4(...), {1,3})`

#pragma once

#include "algebra.h"
#include "lyndon.h"


// TODO: Remove overrides for `coproduct` and `comultiply`. Instead introduce an
//   extendable type trait `CoExprForExpr` and use it to deduce result types.

namespace internal {
template<typename CoExprT, typename ExprT>
CoExprT to_coexpr(ExprT expr) {
  using CoMonomT = typename CoExprT::StorageT;
  constexpr int is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  if constexpr (is_lie_algebra) {
    expr = to_lyndon_basis(expr);
  };
  return expr.template mapped_key<CoExprT>([](const auto& term) {
    return CoMonomT{term};
  });
}

template<typename CoExprT>
CoExprT normalize_coproduct(const CoExprT& expr) {
  static_assert(CoExprT::Param::coproduct_is_lie_algebra);
  return to_lyndon_basis(expr);
}
}  // namespace internal


template<typename CoExprT, typename ExprT>
CoExprT coproduct_vec(const std::vector<ExprT>& expr) {
  constexpr int is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  const auto coexpr = mapped(expr, internal::to_coexpr<CoExprT, ExprT>);
  auto ret = outer_product<CoExprT>(
    coexpr,
    [](const auto& u, const auto& v) {
      return concat(u, v);
    },
    AnnOperator(is_lie_algebra ? fmt::coprod_lie() : fmt::coprod_hopf())
  );
  if constexpr (is_lie_algebra) {
    return internal::normalize_coproduct(ret);
  } else {
    return ret;  // `normalize_coproduct` might not compile here, thus `if constexpr`
  }
}

template<typename CoExprT, typename... Args>
CoExprT coproduct(Args&&... args) {
  return coproduct_vec<CoExprT>(std::vector{std::forward<Args>(args)...});
}

template<typename CoExprT, typename ExprT>
CoExprT comultiply(const ExprT& expr, std::vector<int> form) {
  static_assert(CoExprT::Param::coproduct_is_lie_algebra);
  if (expr.is_zero()) {
    return {};
  }
  const int weight = expr.weight();
  CHECK_EQ(sum(form), weight)
      << "Cannot comultiply an expression of weight " << weight
      << " into components " << str_join(form, " + ") << " = " << sum(form);
  CHECK(form.size() >= 2) << dump_to_string(form);
  CHECK(form.size() == 2 || all_equal(form))
      << "Comultiplication into three or more unequal components is not supported: " << dump_to_string(form);
  absl::c_sort(form);  // avoid unnecessary work in `normalize_coproduct`

  using MonomT = typename ExprT::StorageT;
  // Optimization potential: remove conversion of vector form to key (here) and back
  // (inside to_lyndon_basis inside coproduct). Idea: convert to Lyndon basis here
  // and add a compile-time flag to `coproduct` saying that this is no longer required;
  // note that in this case it might be better to convert the entire expression first
  // (see above).
  static auto make_copart = [](auto span) {
    return ExprT::single_key(
      ExprT::Param::vector_to_key(typename ExprT::Param::VectorT(span.begin(), span.end()))
    );
  };
  CoExprT ret;
  expr.foreach_key([&](const MonomT& monom, int coeff) {
    const auto& monom_vec = ExprT::Param::key_to_vector(monom);
    CHECK_EQ(monom_vec.size(), weight);
    const auto span = absl::MakeConstSpan(monom_vec);
    if (form.size() == 2) {
      const int split = form[0];
      ret += coeff * coproduct<CoExprT>(
        make_copart(span.subspan(0, split)),
        make_copart(span.subspan(split))
      );
      if (form[0] != form[1]) {
        const int split = form[1];
        ret -= coeff * coproduct<CoExprT>(
          make_copart(span.subspan(split)),
          make_copart(span.subspan(0, split))
        );
      }
    } else {
      std::vector<ExprT> parts;
      int part_begin = 0;
      for (const int part_weight : form) {
        parts.push_back(make_copart(span.subspan(part_begin, part_weight)));
        part_begin += part_weight;
      }
      CHECK_EQ(part_begin, span.size());
      ret += coeff * coproduct_vec<CoExprT>(parts);
    }
  });
  return ret.copy_annotations_mapped(
    expr, [](const std::string& annotation) {
      return fmt::comult() + annotation;
    }
  );
}


template<typename CoExprT, typename F>
CoExprT filter_coexpr_predicate(const CoExprT& expr, int component, const F& predicate) {
  return expr.filtered([&](const typename CoExprT::ObjectT& term) {
    return predicate(term.at(component));
  });
}

template<typename CoExprT>
CoExprT filter_coexpr(
    const CoExprT& expr, int component, const typename CoExprT::ObjectT::value_type& value) {
  return filter_coexpr_predicate(
    expr, component,
    [&](const typename CoExprT::ObjectT::value_type& x) {
      return x == value;
    }
  );
}
