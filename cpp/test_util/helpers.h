#pragma once

#include "lib/coalgebra.h"


struct SimpleVectorExprParam : SimpleLinearParam<std::vector<int>> {
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  static std::string object_to_string(const ObjectT& obj) {
    return fmt::parens(str_join(obj, ", "));
  }
  static int object_to_weight(const ObjectT& obj) {
    return obj.size();
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
};

struct SimpleVectorCoExprParam : SimpleLinearParam<std::vector<std::vector<int>>> {
  using PartExprParam = SimpleVectorExprParam;
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), SimpleVectorExprParam::object_to_string);
  }
  static int object_to_weight(const ObjectT& obj) {
    return sum(mapped(obj, [](const auto& part) { return part.size(); }));
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct SimpleVectorNCoExprParam : SimpleVectorCoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), SimpleVectorExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};

using SimpleVectorExpr = Linear<SimpleVectorExprParam>;
using SimpleVectorCoExpr = Linear<SimpleVectorCoExprParam>;
using SimpleVectorNCoExpr = Linear<SimpleVectorNCoExprParam>;
template<> struct CoExprForExpr<SimpleVectorExpr> { using type = SimpleVectorCoExpr; };
template<> struct NCoExprForExpr<SimpleVectorExpr> { using type = SimpleVectorNCoExpr; };


inline SimpleVectorExpr SV(std::vector<int> data) {
  return SimpleVectorExpr::single(std::move(data));
}

inline SimpleVectorCoExpr CoSV(std::vector<std::vector<int>> data) {
  return SimpleVectorCoExpr::single(std::move(data));
}
