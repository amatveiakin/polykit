#pragma once

#include "delta.h"


namespace internal {
struct ProjectionExprParam : public IdentityVectorLinearParamMixin<PVector<unsigned char, 10>> {
  using ObjectT = std::vector<int>;
  using StorageT = PVector<unsigned char, 10>;
  static StorageT object_to_key(const ObjectT& obj) {
    return to_pvector<StorageT>(obj);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return to_vector<int>(key);
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
void print_sorted_by_num_distinct_variables(std::ostream& os, const ProjectionExpr& expr);
