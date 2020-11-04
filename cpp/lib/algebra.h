#pragma once

#include <sstream>

#include "absl/types/span.h"

#include "linear.h"
#include "word.h"


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

inline ProductAnnotation AnnOperator(std::string op) {
  CHECK(!op.empty());
  return ProductAnnotationOperator{std::move(op)};
}

inline ProductAnnotation AnnNone() {
  return ProductAnnotationNone{};
}


namespace internal {

template<typename LinearT>
static std::string short_linear_description(const LinearT& expr) {
  const auto& annotations = expr.annotations();
  if (annotations.size() == 0) {
    return "<?>";
  } else if (annotations.size() == 1) {
    // TODO: Get this clearer
    std::stringstream ss;
    ss << annotations;
    // TODO: Tidy up: remove leading `+`, etc.
    return trimed(ss.str());
  } else {
    return absl::StrCat("<", annotations.size(), " ",
        en_plural(annotations.size(), "term"), ">");
  }
}

template<typename LinearT>
static std::optional<std::string> product_annotation(
    const ProductAnnotation& annotation,
    const absl::Span<const LinearT>& expressions) {
  if (std::holds_alternative<ProductAnnotationNone>(annotation)) {
    return std::nullopt;
  }
  const bool has_annotations = absl::c_any_of(
    expressions,
    [](const auto& expr) { return !expr.annotations().zero(); }
  );
  if (!has_annotations) {
    return std::nullopt;
  }
  const std::vector<std::string> args = mapped(
    expressions, &short_linear_description<LinearT>);
  return std::visit(overloaded{
    [](const ProductAnnotationNone&) -> std::string {
      FATAL("ProductAnnotationNone");
    },
    [&](const ProductAnnotationFunction& ann) {
      return fmt::function(ann.name, args);
    },
    [&](const ProductAnnotationOperator& ann) {
      // TODO: Inproduct `fmt::braces` and use it here
      return absl::StrCat("(", str_join(args, ann.op), ")");
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


template<typename LinearProdT, typename LinearT, typename MonomProdF>
LinearProdT outer_product(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product,
    const ProductAnnotation& annotation) {
  static_assert(std::is_same_v<
      std::invoke_result_t<MonomProdF, typename LinearT::StorageT, typename LinearT::StorageT>,
      typename LinearProdT::StorageT>);
  LinearProdT ret;
  lhs.foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      const auto ret_key = monom_key_product(lhs_key, rhs_key);
      assert(ret.coeff_for_key(ret_key) == 0);  // outer product must be unique
      ret.add_to_key(ret_key, lhs_coeff * rhs_coeff);
    });
  });
  return LinearProdT(ret).maybe_annotate(internal::product_annotation(annotation, lhs, rhs));
}

template<typename LinearT, typename MonomProdF>
LinearT outer_product(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product,
    const ProductAnnotation& annotation) {
  if (expressions.empty()) {
    return {};
  }
  LinearT ret = expressions.front();
  for (const LinearT& expr : expressions.subspan(1)) {
    ret = outer_product<LinearT>(ret, expr, monom_key_product, AnnNone());
  }
  return ret.maybe_annotate(internal::product_annotation(annotation, expressions));
}


// Similar to outer project, but monom_key_product produces an expr rather than one monom.
template<typename LinearT, typename MonomProdF>
LinearT outer_product_expanding(
    const LinearT& lhs,
    const LinearT& rhs,
    const MonomProdF& monom_key_product,
    const ProductAnnotation& annotation) {
  using LinearStorageT = typename LinearT::StorageT;
  static_assert(std::is_same_v<
      typename std::invoke_result_t<MonomProdF, LinearStorageT, LinearStorageT>::StorageT,
      LinearStorageT>);
  LinearT ret;
  lhs.foreach_key([&](const auto& lhs_key, int lhs_coeff) {
    rhs.foreach_key([&](const auto& rhs_key, int rhs_coeff) {
      const auto& prod = monom_key_product(lhs_key, rhs_key);
      ret += (lhs_coeff * rhs_coeff) * prod.template cast_to<LinearT>();
    });
  });
  return LinearT(ret).maybe_annotate(internal::product_annotation(annotation, lhs, rhs));
}

template<typename LinearT, typename MonomProdF>
LinearT outer_product_expanding(
    const absl::Span<const LinearT>& expressions,
    const MonomProdF& monom_key_product,
    const ProductAnnotation& annotation) {
  if (expressions.empty()) {
    return {};
  }
  LinearT ret = expressions.front();
  for (const LinearT& expr : expressions.subspan(1)) {
    ret = outer_product_expanding(ret, expr, monom_key_product, AnnNone());
  }
  return ret.maybe_annotate(internal::product_annotation(annotation, expressions));
}


template<typename LinearT>
LinearT tensor_product(
    const LinearT& lhs,
    const LinearT& rhs) {
  return outer_product<LinearT>(lhs, rhs, LinearT::Param::monom_tensor_product,
    AnnOperator(fmt::tensor_prod()));
}

template<typename LinearT>
LinearT tensor_product(
    const absl::Span<const LinearT>& expressions) {
  return outer_product(expressions, LinearT::Param::monom_tensor_product,
    AnnOperator(fmt::tensor_prod()));
}
