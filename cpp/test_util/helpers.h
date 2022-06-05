#pragma once

#include "cpp/lib/coalgebra.h"


struct SimpleVectorExprParam : SimpleLinearParam<std::vector<int>> {
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::monostate element_uniformity_marker(int) { return {}; }
  static std::string object_to_string(const ObjectT& obj) {
    return fmt::parens(str_join(obj, ", "));
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
};

struct SimpleVectorICoExprParam : SimpleLinearParam<std::vector<std::vector<int>>> {
  using PartExprParam = SimpleVectorExprParam;
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  CO_DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), SimpleVectorExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct SimpleVectorNCoExprParam : SimpleVectorICoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), SimpleVectorExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};

using SimpleVectorExpr = Linear<SimpleVectorExprParam>;
using SimpleVectorICoExpr = Linear<SimpleVectorICoExprParam>;
using SimpleVectorNCoExpr = Linear<SimpleVectorNCoExprParam>;
template<> struct ICoExprForExpr<SimpleVectorExpr> { using type = SimpleVectorICoExpr; };
template<> struct NCoExprForExpr<SimpleVectorExpr> { using type = SimpleVectorNCoExpr; };


inline SimpleVectorExpr SV(std::vector<int> data) {
  return SimpleVectorExpr::single(std::move(data));
}

inline SimpleVectorICoExpr CoSV(std::vector<std::vector<int>> data) {
  return SimpleVectorICoExpr::single(std::move(data));
}
