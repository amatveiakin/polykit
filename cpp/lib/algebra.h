// Outer product functions.
//
// Outer product is a generic function that takes two linear expressions and a
// monom product function and applies monom product to every element from the
// cartesian product of the sets of terms of the two expression. That is,
// given expressions
//   A = a1 x1 + ... + an xn
//   B = b1 y1 + ... + bm ym
// and a monom product function f, we define outer_product(A, B, f) as
//   (a1+b1) f(x1,y1) + (a1+b2) f(x1,y2) + (a2+b1) f(x2,y1) + ... + (an+bm) f(xn,ym)
// For an example of an outer product see tensor_product below.
//
// The `annotation` argument describes annotations produced. See linear.h for
// details on annotations.
//
// For performance reasons, monom product function is expected to operate in key
// space. See linear.h for details on object space vs key space.
//
// This file contains three functions:
//
//   * `outer_product(lhs, rhs, monom_key_product, annotation)`
//     `outer_product(expressions_vector, monom_key_product, annotation)`
//     Generic form, as described above. The second form takes takes multiple
//     expressions and computes outer product left to right.
//
//   * `outer_product_expanding` is similar to `outer_product` outer project,
//     but `monom_key_product` produces an expression rather than one monom.
//
//   * `tensor_product` is a special case of an outer product. It uses a function
//     called `monom_tensor_product` defined as part of linear expression params.
//     Semantically it concatenates two monoms together.
//
// All functions can take either `Linear` or `BasicLinear`. The latter version has a
// annotation argument (which is ignored) to facilitate writing generic product functions,
// e.g. `tensor_product` doesn't have to support `Linear` or `BasicLinear` separately:
// it always passes an annotation, and it's discarded if not needed. In order to avoid
// computing annotation object when not needed, we take a generator function rather than
// a value, so that the function could be optimized away when it's not called; this is
// conceptually similar to things like `Option::unwrap_or_else` in Rust.

#pragma once

#include <sstream>

#include "absl/types/span.h"

#include "linear.h"


struct ProductAnnotationNone {};
struct ProductAnnotationFunction {
  std::string name;
};
struct ProductAnnotationOperator {
  std::string op;
};

using ProductAnnotation = std::variant<
  ProductAnnotationNone,
  ProductAnnotationFunction,
  ProductAnnotationOperator
>;

inline ProductAnnotation AnnFunction(std::string name) {
  CHECK(!name.empty());
  return ProductAnnotationFunction{std::move(name)};
}

inline ProductAnnotation AnnFunctionOp(const std::string& name) {
  CHECK(!name.empty());
  return ProductAnnotationFunction{fmt::opname(name)};
}

inline ProductAnnotation AnnOperator(std::string op) {
  CHECK(!op.empty());
  return ProductAnnotationOperator{std::move(op)};
}

inline ProductAnnotation AnnNone() {
  return ProductAnnotationNone{};
}


namespace internal {

template<typename LinearT>
static std::optional<std::string> product_annotation(
    const ProductAnnotation& annotation,
    const absl::Span<const LinearT>& expressions) {
  if (std::holds_alternative<ProductAnnotationNone>(annotation)) {
    return std::nullopt;
  }
  const bool has_annotations = absl::c_any_of(
    expressions,
    [](const auto& expr) { return !expr.annotations().empty(); }
  );
  if (!has_annotations) {
    return std::nullopt;
  }
  const std::vector<std::string> args = mapped(
    expressions, [](const auto& expr) { return annotations_one_liner(expr.annotations()); }
  );
  return std::visit(overloaded{
    [](const ProductAnnotationNone&) -> std::string {
      FATAL("ProductAnnotationNone");
    },
    [&](const ProductAnnotationFunction& ann) {
      return fmt::function(ann.name, args, HSpacing::sparse);
    },
    [&](const ProductAnnotationOperator& ann) {
      return fmt::parens(str_join(args, ann.op));
    },
  }, annotation);
}

template<typename LinearT>
static std::optional<std::string> product_annotation(
    const ProductAnnotation& annotation,
    const LinearT& lhs,
    const LinearT& rhs) {
  return product_annotation(annotation, absl::MakeConstSpan({lhs, rhs}));
}

}  // namespace internal


template<typename LinearProdT, typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_basic_linear_v<LinearT> && is_basic_linear_v<LinearProdT>, LinearProdT>
outer_product(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product,
    const AnnotationsF& = nullptr) {
  LinearProdT ret;
  lhs.foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      const auto ret_key = monom_key_product(lhs_key, rhs_key);
      ASSERT(ret.coeff_for_key(ret_key) == 0);  // outer product must be unique
      ret.add_to_key(ret_key, lhs_coeff * rhs_coeff);
    });
  });
  return ret;
}
template<typename LinearProdT, typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_linear_v<LinearT> && is_linear_v<LinearProdT>, LinearProdT>
outer_product(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product,
    const AnnotationsF& get_annotation) {
  return outer_product<typename LinearProdT::Basic>(
    lhs.main(), rhs.main(), monom_key_product, nullptr
  ).maybe_annotate(
    internal::product_annotation(get_annotation(), lhs, rhs)
  );
}

template<typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_basic_linear_v<LinearT>, LinearT>
outer_product(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product,
    const AnnotationsF& = nullptr) {
  if (expressions.empty()) {
    return {};
  }
  LinearT ret = expressions.front();
  for (const LinearT& expr : expressions.subspan(1)) {
    ret = outer_product<LinearT>(ret, expr, monom_key_product, nullptr);
  }
  return ret;
}
template<typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_linear_v<LinearT>, LinearT>
outer_product(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product,
    const AnnotationsF& get_annotation) {
  const ProductAnnotation annotation = get_annotation();
  if (expressions.size() == 1 && std::holds_alternative<ProductAnnotationOperator>(annotation)) {
    // Keep annotations intact rather than collapsing into a one-liner
    return expressions.front();
  }
  const auto basic_expressions = mapped(expressions, [](const auto& expr) { return expr.main(); });
  return outer_product(
    absl::MakeConstSpan(basic_expressions), monom_key_product, nullptr
  ).maybe_annotate(
    internal::product_annotation(annotation, expressions)
  );
}


// Similar to `outer_product`, but `monom_key_product` produces an expr rather than one monom.
template<typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_basic_linear_v<LinearT>, LinearT>
outer_product_expanding(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product,
    const AnnotationsF& = nullptr) {
  LinearT ret;
  lhs.foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      // TODO: Remove `cast_to` here, push it to the users where necessary.
      const auto& prod = monom_key_product(lhs_key, rhs_key);
      ret += (lhs_coeff * rhs_coeff) * prod.template cast_to<LinearT>();
    });
  });
  return ret;
}
template<typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_linear_v<LinearT>, LinearT>
outer_product_expanding(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product,
    const AnnotationsF& get_annotation) {
  return outer_product_expanding(
    lhs.main(), rhs.main(), monom_key_product, nullptr
  ).maybe_annotate(
    internal::product_annotation(get_annotation(), lhs, rhs)
  );
}

template<typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_basic_linear_v<LinearT>, LinearT>
outer_product_expanding(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product,
    const AnnotationsF& = nullptr) {
  if (expressions.empty()) {
    return {};
  }
  LinearT ret = expressions.front();
  for (const LinearT& expr : expressions.subspan(1)) {
    ret = outer_product_expanding(ret, expr, monom_key_product, nullptr);
  }
  return ret;
}
template<typename LinearT, typename MonomProdF, typename AnnotationsF>
std::enable_if_t<is_linear_v<LinearT>, LinearT>
outer_product_expanding(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product,
    const AnnotationsF& get_annotation) {
  const auto basic_expressions = mapped(expressions, [](const auto& expr) { return expr.main(); });
  return outer_product_expanding(
    absl::MakeConstSpan(basic_expressions), monom_key_product, nullptr
  ).maybe_annotate(
    internal::product_annotation(get_annotation(), expressions)
  );
}


template<typename LinearT>
LinearT tensor_product(
    const LinearT& lhs,
    const LinearT& rhs) {
  static_assert(is_any_linear_v<LinearT>);
  return outer_product<LinearT>(lhs, rhs, LinearT::Param::monom_tensor_product,
    []() { return AnnOperator(fmt::tensor_prod()); });
}

template<typename LinearT>
LinearT tensor_product(
    const absl::Span<const LinearT>& expressions) {
  static_assert(is_any_linear_v<LinearT>);
  return outer_product(expressions, LinearT::Param::monom_tensor_product,
    []() { return AnnOperator(fmt::tensor_prod()); });
}
