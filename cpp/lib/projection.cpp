#include "projection.h"


ProjectionExpr project_on(int axis, const DeltaExpr& expr) {
  return expr.mapped_expanding([&](const std::vector<Delta>& deltas) {
    ProjectionExpr::StorageT proj;
    for (const Delta& d : deltas) {
      CHECK(!d.is_nil());
      if (d.a() == axis) {
        proj.push_back(d.b());
      } else if (d.b() == axis) {
        proj.push_back(d.a());
      } else {
        return ProjectionExpr{};
      }
    }
    return ProjectionExpr::single_key(proj);
  });
}

ProjectionExpr terms_with_num_distinct_variables(const ProjectionExpr& expr, int num_distinct) {
  return expr.filtered_key([&](const ProjectionExpr::StorageT& term) {
    return num_distinct_elements(term) == num_distinct;
  });
}

ProjectionExpr terms_with_min_distinct_variables(const ProjectionExpr& expr, int min_distinct) {
  return expr.filtered_key([&](const ProjectionExpr::StorageT& term) {
    return num_distinct_elements(term) >= min_distinct;
  });
}

void print_sorted_by_num_distinct_variables(std::ostream& os, const ProjectionExpr& expr) {
  if (expr.zero()) {
    os << expr;
    return;
  }
  std::map<int, ProjectionExpr> num_variables_to_expr;
  expr.foreach_key([&](const auto& term, int coeff) {
    num_variables_to_expr[num_distinct_elements(term)].add_to_key(term, coeff);
  });
  bool first = true;
  for (const auto& [num_variables, expr] : num_variables_to_expr) {
    if (!first) {
      os << "---\n";
    }
    first = false;
    os << num_variables << " variables " << expr;
  }
  if (*current_formatting_config().expression_include_annotations) {
    os << "~~~\n" << expr.annotations();
  }
}
