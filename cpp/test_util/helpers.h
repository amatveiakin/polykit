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
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_lie(), SimpleVectorExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
};

using SimpleVectorExpr = Linear<SimpleVectorExprParam>;
using SimpleVectorCoExpr = Linear<SimpleVectorCoExprParam>;


inline SimpleVectorExpr SV(std::vector<int> data) {
  return SimpleVectorExpr::single(std::move(data));
}

inline SimpleVectorCoExpr CoSV(std::vector<std::vector<int>> data) {
  return SimpleVectorCoExpr::single(std::move(data));
}


// Explicit rules allow to omit template types when calling the function.
inline SimpleVectorCoExpr coproduct(const SimpleVectorExpr& lhs, const SimpleVectorExpr& rhs) {
  return coproduct<SimpleVectorCoExpr>(lhs, rhs);
}
inline SimpleVectorCoExpr comultiply(const SimpleVectorExpr& expr, const std::vector<int>& form) {
  return comultiply<SimpleVectorCoExpr>(expr, form);
}
