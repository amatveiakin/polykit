#pragma once

#include "delta.h"


namespace internal {
struct ProjectionExprParam {
  using ObjectT = std::vector<X>;
  using StorageT = PVector<std::byte, 10>;
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  static constexpr int kFormBits = 4;
  static constexpr int kIndexBits = CHAR_BIT - kFormBits;
  static constexpr int kIndexMask = (1 << kIndexBits) - 1;
  static std::byte x_to_key(X x) {
    const int form = static_cast<int>(x.form());
    const int index = x.is_constant() ? 0 : x.idx();
    CHECK_LT(form, 1 << kFormBits);
    CHECK_LT(index, 1 << kIndexBits);
    return std::byte((form << kIndexBits) | index);
  }
  static X key_to_x(std::byte key) {
    const int v = std::to_integer<int>(key);
    const int form = v >> kIndexBits;
    const int index = v & kIndexMask;
    return X(static_cast<XForm>(form), index);
  }
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, x_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, key_to_x);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return fmt::parens(str_join(obj, ", "));
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
};
}  // namespace internal

using ProjectionExpr = Linear<internal::ProjectionExprParam>;

using DeltaProjector = std::function<ProjectionExpr(DeltaExpr)>;


ProjectionExpr project_on(int axis, const DeltaExpr& expr);

inline ProjectionExpr project_on_x1 (const DeltaExpr& expr) { return project_on(1,  expr); }
inline ProjectionExpr project_on_x2 (const DeltaExpr& expr) { return project_on(2,  expr); }
inline ProjectionExpr project_on_x3 (const DeltaExpr& expr) { return project_on(3,  expr); }
inline ProjectionExpr project_on_x4 (const DeltaExpr& expr) { return project_on(4,  expr); }
inline ProjectionExpr project_on_x5 (const DeltaExpr& expr) { return project_on(5,  expr); }
inline ProjectionExpr project_on_x6 (const DeltaExpr& expr) { return project_on(6,  expr); }
inline ProjectionExpr project_on_x7 (const DeltaExpr& expr) { return project_on(7,  expr); }
inline ProjectionExpr project_on_x8 (const DeltaExpr& expr) { return project_on(8,  expr); }
inline ProjectionExpr project_on_x9 (const DeltaExpr& expr) { return project_on(9,  expr); }
inline ProjectionExpr project_on_x10(const DeltaExpr& expr) { return project_on(10, expr); }
inline ProjectionExpr project_on_x11(const DeltaExpr& expr) { return project_on(11, expr); }
inline ProjectionExpr project_on_x12(const DeltaExpr& expr) { return project_on(12, expr); }
inline ProjectionExpr project_on_x13(const DeltaExpr& expr) { return project_on(13, expr); }
inline ProjectionExpr project_on_x14(const DeltaExpr& expr) { return project_on(14, expr); }
inline ProjectionExpr project_on_x15(const DeltaExpr& expr) { return project_on(15, expr); }


ProjectionExpr terms_with_num_distinct_variables(const ProjectionExpr& expr, int num_distinct);
ProjectionExpr terms_with_min_distinct_variables(const ProjectionExpr& expr, int min_distinct);

// For using together with `ProjectionExpr::filter`
inline int count_var(const ProjectionExpr::ObjectT& term, int var) {
  return absl::c_count(term, var);
};

void print_sorted_by_num_distinct_variables(std::ostream& os, const ProjectionExpr& expr);
