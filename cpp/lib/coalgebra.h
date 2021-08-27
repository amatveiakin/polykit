// Functions related to comultiplication.
//
// There are three types of coexpressions. They differ in how the parts are reordered:
//   * "Normal": parts are sorted with the change of sign. Applying normal
//     comultiplication twice to the same expressions always yield zero.
//   * "Iterated": parts are converted to Lyndon basis.
//   * "Hopf": parts are fixed in place and cannot be reordered. There is no Hopf
//     comultiplication operation, but such an expression can be constructed by hand.
//
// All functions come in two forms: "n" version means "normal", default is iterative.
// TODO: Change the names if "normal" is indeed normal.
//
// Contains two main functions:
//
//   * `coproduct(expr1, expr2, ..., exprN)` / `ncoproduct(expr1, expr2, ..., exprN)`
//     constructs a formal tuple (expr1, expr2, ..., exprN) that corresponds to one
//     comultiplication term. This is not a mathematical operation, this is merely a
//     tool for constructing the comultiplication.
//     `coproduct_vec(expressions)` / `coproduct_vec(expressions)`
//     same as above, vector form.
//
//   * `comultiply(expression, form)` / `ncomultiply(expression, [form])`
//     computes comultiplication. If `form` is given, only the specified component
//     is computed. The order of elements in `form` does not matter: comultiplication
//     is always ordered accordingly.
//     Example: `comultiply(QLi4(...), {1,3})`
//
// Also contains a type trait `CoExprForExpr` / `NCoExprForExpr`. When defining a new
// coexpression one should extend it like this:
//
//   template<> struct CoExprForExpr<MyExpr> { using type = MyCoExpr; };
//   template<> struct NCoExprForExpr<MyExpr> { using type = MyNCoExpr; };
//
// NCoExprForExpr<MyExpr> enables `ncoproduct` and `comultiply` for `MyExpr`.
// CoExprForExpr<MyExpr> enables `coproduct` in all cases, and `comultiply` if
//   `MyCoExpr` supports Lyndon.

#pragma once

#include "algebra.h"
#include "lyndon.h"


template<typename ExprT>
struct CoExprForExpr {
  using type = void;
};
template<typename ExprT>
using CoExprForExpr_t = typename CoExprForExpr<ExprT>::type;

template<typename ExprT>
struct NCoExprForExpr {
  using type = void;
};
template<typename ExprT>
using NCoExprForExpr_t = typename NCoExprForExpr<ExprT>::type;


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

template<typename T>
auto maybe_to_ncoexpr(T expr) {
  using NCoExprT = NCoExprForExpr_t<T>;
  if constexpr (std::is_void_v<NCoExprT>) {
    return expr;
  } else {
    return to_coexpr<NCoExprT>(expr);
  }
}

template<typename CoExprT>
CoExprT normalize_coproduct(const CoExprT& expr) {
  static_assert(CoExprT::Param::coproduct_is_lie_algebra);
  if constexpr (CoExprT::Param::coproduct_is_iterated) {
    return to_lyndon_basis(expr);
  } else {
    return expr.mapped_key_expanding([](auto term) {
      // Note: the comparator is fixed, because filtering by form in `ncomultiply_impl`
      // assumes shorter element go first.
      const int sign = sort_with_sign(term, DISAMBIGUATE(compare_length_first));
      if (!all_unique_sorted(term)) {
        return CoExprT{};
      }
      return sign * CoExprT::single_key(term);
    });
  }
}

template<typename CoExprT, typename ExprT>
auto abstract_coproduct_vec(const std::vector<ExprT>& expr) {
  constexpr bool is_lie_algebra = CoExprT::Param::coproduct_is_lie_algebra;
  constexpr bool is_iterated = CoExprT::Param::coproduct_is_iterated;
  const auto coexpr = mapped(expr, internal::to_coexpr<CoExprT, ExprT>);
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
    return internal::normalize_coproduct(ret);
  } else {
    return ret;  // `normalize_coproduct` might not compile here, thus `if constexpr`
  }
}
}  // namespace internal


template<typename ExprT>
auto coproduct_vec(const std::vector<ExprT>& expr) {
  return internal::abstract_coproduct_vec<CoExprForExpr_t<ExprT>>(expr);
}

template<typename ExprT>
auto ncoproduct_vec(const std::vector<ExprT>& expr) {
  return internal::abstract_coproduct_vec<NCoExprForExpr_t<ExprT>>(expr);
}

template<typename... Args>
auto coproduct(Args&&... args) {
  return coproduct_vec(std::vector{std::forward<Args>(args)...});
}

template<typename... Args>
auto ncoproduct(Args&&... args) {
  return ncoproduct_vec(std::vector{std::forward<Args>(args)...});
}


template<typename ExprT>
auto comultiply(const ExprT& expr, std::vector<int> form) {
  using CoExprT = CoExprForExpr_t<ExprT>;
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
      << "Comultiplication into three or more unequal parts is not supported: " << dump_to_string(form);
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
      ret += coeff * coproduct(
        make_copart(span.subspan(0, split)),
        make_copart(span.subspan(split))
      );
      if (form[0] != form[1]) {
        const int split = form[1];
        ret -= coeff * coproduct(
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
      ret += coeff * coproduct_vec(parts);
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
    absl::c_sort(form);
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
    ret = ret.filtered([&](const auto& term) {
      const auto sizes = mapped(term, [](const auto& v) -> int { return v.size(); });
      CHECK(absl::c_is_sorted(sizes));
      return sizes == form;
    });
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
  return internal::ncomultiply_impl(internal::maybe_to_ncoexpr(expr), std::move(form));
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
