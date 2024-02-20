// Functions related to comultiplication.
//
// There are several types of coexpressions. They differ in how the parts are reordered:
//
//   * "Normal": parts are sorted lexicographically, with sign. Applying normal
//     comultiplication twice to the same expressions always yield zero.
//     Function prefix: `n`.
//
//   * "Iterated": parts are converted to Lyndon basis. If comultiplication is applied once
//     (i.e. there are exactly two parts), this is equivalent to "normal" comultiplication.
//     Can meaningfully comultiply an expression into a coexpression with three or more parts,
//     but only if they have the same length.
//     Function prefix: `i`.
//
//   * "Anti-iterated": like iterated, but parts are sorted by descending length instead
//     of ascending. Used by `expand_into_glued_pairs`.
//     Function prefix: `a`.
//
//   * "Hopf": parts are fixed in place and cannot be reordered.
//     Function prefix: none. There is no Hopf comultiplication operation, such an
//     expression can only be constructed by hand.
//
// Contains two main functions:
//
//   * `ncoproduct(expr1, expr2, ..., exprN)` / `icoproduct(expr1, expr2, ..., exprN)`
//     constructs a formal tuple (expr1, expr2, ..., exprN) that corresponds to one
//     comultiplication term. This is not a mathematical operation, this is merely a
//     tool for constructing the comultiplication.
//     `ncoproduct_vec(expressions)` / `icoproduct_vec(expressions)`
//     same as above, vector form.
//
//   * `ncomultiply(expression, form)` / `icomultiply(expression, [form])`
//     computes comultiplication. If `form` is given, only the specified component
//     is computed. The order of elements in `form` does not matter: comultiplication
//     is always ordered accordingly.
//     Example: `ncomultiply(QLi4(...), {1,3})`
//
// Also contains a type trait `NCoExprForExpr` / `ICoExprForExpr`. When defining a new
// coexpression one should extend it like this:
//
//   template<> struct NCoExprForExpr<MyExpr> { using type = MyNCoExpr; };
//   template<> struct ICoExprForExpr<MyExpr> { using type = MyICoExpr; };
//
// NCoExprForExpr<MyExpr> enables `ncoproduct` and `ncomultiply` for `MyExpr`.
// ICoExprForExpr<MyExpr> enables `icoproduct` in all cases, and `icomultiply` if
//   `MyCoExpr` supports Lyndon basis.
//
// Note that Lyndon basis is used in three different places when working with Lie coalgebras:
//   1. The entire expression is automatically transformed to Lyndon basis before it is
//      converted to coexpression.
//   2. After the expression is cut into part, each part is transformed to Lyndon basis again.
//   3. Finally, with iterated comultiplication, Lyndon basis is applied to parts as wholes.

#pragma once

#include "algebra.h"
#include "lyndon.h"
#include "sorting.h"


template<typename ExprT>
struct NCoExprForExpr {
  using type = void;
};
template<typename ExprT>
using NCoExprForExpr_t = typename NCoExprForExpr<ExprT>::type;

template<typename ExprT>
struct ICoExprForExpr {
  using type = void;
};
template<typename ExprT>
using ICoExprForExpr_t = typename ICoExprForExpr<ExprT>::type;

// TODO: Consider whether this could be made the default order for co-expressions.
//   If so, remove this additional co-expression type.
template<typename ExprT>
struct ACoExprForExpr {
  using type = void;
};
template<typename ExprT>
using ACoExprForExpr_t = typename ACoExprForExpr<ExprT>::type;


namespace internal {
template<typename CoExprT, typename ExprT>
auto to_coexpr(ExprT expr) {
  using CoMonomT = typename CoExprT::StorageT;
  constexpr int is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  if constexpr (is_lie_algebra) {
    expr = to_lyndon_basis(expr);
  };
  return expr.template mapped_key<CoExprT>([](const auto& term) {
    return CoMonomT{term};
  });
}

// Usage e.g.: maybe_to_coexpr<NCoExprForExpr_t<T>>
template<typename CoExprT, typename T>
auto maybe_to_coexpr(T expr) {
  if constexpr (std::is_void_v<CoExprT>) {
    return expr;
  } else {
    return to_coexpr<CoExprT>(std::move(expr));
  }
}

template<typename T>
auto maybe_to_ncoexpr(T expr) {
  return maybe_to_coexpr<NCoExprForExpr_t<T>>(std::move(expr));
}
template<typename T>
auto maybe_to_icoexpr(T expr) {
  return maybe_to_coexpr<ICoExprForExpr_t<T>>(std::move(expr));
}
template<typename T>
auto maybe_to_acoexpr(T expr) {
  return maybe_to_coexpr<ACoExprForExpr_t<T>>(std::move(expr));
}

template<typename CoExprT>
CoExprT sort_coproduct_parts(const CoExprT& expr) {
  static_assert(CoExprT::Param::coproduct_is_lie_algebra);
  if constexpr (CoExprT::Param::coproduct_is_iterated) {
    return to_lyndon_basis(expr);
  } else {
    CoExprT sorted_expr;
    for (const auto& [term_ref, coeff] : key_view(expr)) {
      auto term = term_ref;
      // Note: the comparator is fixed, because filtering by form in `ncomultiply_impl`
      // assumes shorter element go first.
      const int sign = sort_with_sign(term, DISAMBIGUATE(compare_length_first));
      if (all_unique_sorted(term)) {
        sorted_expr.add_to_key(term, sign * coeff);
      }
    }
    return sorted_expr.copy_annotations(expr);
  }
}

template<typename CoExprT>
auto coproduct_vec(const std::vector<CoExprT>& coexpr) {
  constexpr bool is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  constexpr bool is_iterated = CoExprT::Param::coproduct_is_iterated;
  auto ret = outer_product<CoExprT>(
    coexpr,
    [](const auto& u, const auto& v) {
      return concat(u, v);
    },
    AnnOperator(
      is_lie_algebra
        ? (is_iterated ? fmt::coprod_iterated() : fmt::coprod_normal())
        : fmt::coprod_hopf()
    )
  );
  if constexpr (is_lie_algebra) {
    return internal::sort_coproduct_parts(ret);
  } else {
    return ret;  // `sort_coproduct_parts` might not compile here, thus `if constexpr`
  }
}
}  // namespace internal

// Fixes the order of multiples between parts and within parts in a manually constructed co-expression.
template<typename CoExprT>
CoExprT normalize_coproduct(const CoExprT& expr) {
  constexpr int is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  if constexpr (is_lie_algebra) {
    return internal::sort_coproduct_parts(expr.mapped_expanding([](const auto& term) {
      return internal::coproduct_vec(mapped(term, [](const auto& part) {
        return internal::to_coexpr<CoExprT>(Linear<typename CoExprT::Param::PartExprParam>::single(part));
      }));
    }));
  } else {
    return internal::sort_coproduct_parts(expr);
  }
}

template<typename ExprT>
auto ncoproduct_vec(const std::vector<ExprT>& expr) {
  return internal::coproduct_vec(mapped(expr, DISAMBIGUATE(internal::maybe_to_ncoexpr)));
}

template<typename ExprT>
auto icoproduct_vec(const std::vector<ExprT>& expr) {
  return internal::coproduct_vec(mapped(expr, DISAMBIGUATE(internal::maybe_to_icoexpr)));
}

template<typename ExprT>
auto acoproduct_vec(const std::vector<ExprT>& expr) {
  return internal::coproduct_vec(mapped(expr, DISAMBIGUATE(internal::maybe_to_acoexpr)));
}

template<typename... Args>
auto ncoproduct(Args&&... args) {
  return internal::coproduct_vec(std::vector{internal::maybe_to_ncoexpr(args)...});
}

template<typename... Args>
auto icoproduct(Args&&... args) {
  return internal::coproduct_vec(std::vector{internal::maybe_to_icoexpr(args)...});
}

template<typename... Args>
auto acoproduct(Args&&... args) {
  return internal::coproduct_vec(std::vector{internal::maybe_to_acoexpr(args)...});
}


template<typename ExprT>
auto icomultiply(const ExprT& expr, std::vector<int> form) {
  using CoExprT = ICoExprForExpr_t<ExprT>;
  static_assert(CoExprT::Param::coproduct_is_lie_algebra);
  if (expr.is_zero()) {
    return CoExprT{};
  }
  const int weight = expr.weight();
  CHECK_EQ(sum(form), weight)
      << "Cannot comultiply an expression of weight " << weight
      << " into parts " << str_join(form, " + ") << " = " << sum(form);
  CHECK(form.size() >= 2) << dump_to_string(form);
  CHECK(form.size() == 2 || all_equal(form))
      << "Iterated comultiplication into three or more unequal parts is not supported: " << dump_to_string(form);
  absl::c_sort(form);  // avoid unnecessary work in `sort_coproduct_parts`

  using MonomT = typename ExprT::StorageT;
  // Optimization potential: remove conversion of vector form to key (here) and back
  // (inside to_lyndon_basis inside coproduct). Idea: convert to Lyndon basis here
  // and add a compile-time flag to `icoproduct` saying that this is no longer required;
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
      ret += coeff * icoproduct(
        make_copart(span.subspan(0, split)),
        make_copart(span.subspan(split))
      );
      if (form[0] != form[1]) {
        const int split = form[1];
        ret -= coeff * icoproduct(
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
      ret += coeff * icoproduct_vec(parts);
    }
  });
  return ret.copy_annotations_mapped(
    expr, [](const std::string& annotation) {
      return fmt::comult() + annotation;
    }
  );
}

namespace internal {
template<typename CoExprT>
auto ncomultiply_impl(const CoExprT& coexpr, std::vector<int> form) {
  if (coexpr.is_zero()) {
    return CoExprT{};
  }
  const int weight = coexpr.weight();
  if (!form.empty()) {
    CHECK_EQ(sum(form), weight)
        << "Cannot comultiply an expression of weight " << weight
        << " into parts " << str_join(form, " + ") << " = " << sum(form);
    CHECK(form.size() == coexpr.element().first.size() + 1) << dump_to_string(form);
  }

  using ExprT = Linear<typename CoExprT::Param::PartExprParam>;
  // using MonomT = typename CoExprT::StorageT;
  using ObjectT = typename CoExprT::ObjectT;
  CoExprT ret;
  // Optimization potential: operate in key space
  // Optimization potential: smarter cut point iteration if form is given.
  coexpr.foreach([&](const ObjectT& parts, int coeff) {
    // const ObjectT parts = CoExprT::Param::key_to_vector(monom);
    for (int i_cut_part : range(parts.size())) {
      const auto& cut_part = parts[i_cut_part];
      for (int cut_pos : range(1, cut_part.size())) {
        std::vector<ExprT> new_parts;
        for (int i_part : range(parts.size())) {
          if (i_part == i_cut_part) {
            new_parts.push_back(ExprT::single(slice(cut_part, 0, cut_pos)));
            new_parts.push_back(ExprT::single(slice(cut_part, cut_pos)));
          } else {
            new_parts.push_back(ExprT::single(parts[i_part]));
          }
        }
        const int sign = neg_one_pow(i_cut_part);
        ret += sign * coeff * ncoproduct_vec(new_parts);
      }
    }
  });
  if (!form.empty()) {
    ret = keep_coexpr_form(ret, form);
  }
  return ret.copy_annotations_mapped(
    coexpr, [](const std::string& annotation) {
      return fmt::comult() + annotation;
    }
  );
}
}  // namespace internal

template<typename T>
auto ncomultiply(const T& expr, std::vector<int> form = {}) {
  return internal::ncomultiply_impl(
    internal::maybe_to_coexpr<NCoExprForExpr_t<T>>(expr),
    std::move(form)
  );
}

// Converts each term
//     x1 * x2 * ... * xn
// into a sum
//   + (x1*x2) @ x3 @ ... @ xn
//   + x1 @ (x2*x3) @ ... @ xn
//     ...
//   + x1 @ x2 @ ... @ (x{n-1}*xn)
//
// Note. This function relies on the fact that longer coproduct parts go first.
// If longer parts went last, this function would be broken: Lyndon guarantees
// that the smallest element goes first, but it does not guarantee that the
// largest elements goes last, and we need pairs to stay in the same position.
//
template<typename ExprT>
auto expand_into_glued_pairs(const ExprT& expr) {
  return expr.mapped_expanding([](const auto& term) {
    ACoExprForExpr_t<ExprT> expanded_expr;
    for (const int i : range(term.size() - 1)) {
      std::vector<ExprT> expanded_term;
      for (const int j : range(term.size() - 1)) {
        if (j < i) {
          expanded_term.push_back(ExprT::single({term[j]}));
        } else if (j == i) {
          expanded_term.push_back(ExprT::single({term[j], term[j+1]}));
        } else {
          expanded_term.push_back(ExprT::single({term[j+1]}));
        }
      }
      expanded_expr += acoproduct_vec(expanded_term);
    }
    return expanded_expr;
  });
}


template<typename CoExprT>
auto keep_coexpr_form(const CoExprT& expr, std::vector<int> form) {
  if (expr.is_zero()) {
    return CoExprT{};
  }
  const int weight = expr.weight();
  CHECK_EQ(sum(form), weight) << dump_to_string(form);
  CHECK_EQ(form.size(), expr.element().first.size()) << dump_to_string(form);
  absl::c_sort(form);
  return expr.filtered([&](const auto& term) {
    const auto sizes = mapped(term, [](const auto& v) -> int { return v.size(); });
    CHECK(absl::c_is_sorted(sizes));
    return sizes == form;
  });
}

template<typename CoExprT, typename F>
CoExprT filter_coexpr_predicate(const CoExprT& expr, int part, const F& predicate) {
  return expr.filtered([&](const typename CoExprT::ObjectT& term) {
    return predicate(term.at(part));
  });
}

template<typename CoExprT>
CoExprT filter_coexpr(
    const CoExprT& expr, int part, const typename CoExprT::ObjectT::value_type& value) {
  return filter_coexpr_predicate(
    expr, part,
    [&](const typename CoExprT::ObjectT::value_type& x) {
      return x == value;
    }
  );
}
