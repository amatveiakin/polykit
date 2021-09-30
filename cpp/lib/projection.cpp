#include "projection.h"


static std::optional<X> projection_result(int axis, X x, X y) {
  if (x.idx() != axis) {
    return std::nullopt;
  }
  SWITCH_ENUM_OR_DIE_WITH_CONTEXT(x.form(), "projection", {
    case XForm::var: return y;
    case XForm::neg_var: return y.negated();
    default: break;
  });
}

ProjectionExpr project_on(int axis, const DeltaExpr& expr) {
  return expr.mapped_expanding([&](const std::vector<Delta>& deltas) {
    ProjectionExpr::ObjectT proj;
    for (const Delta& d : deltas) {
      CHECK(!d.is_nil());
      if (auto ret = projection_result(axis, d.a(), d.b()); ret.has_value()) {
        proj.push_back(ret.value());
      } else if (auto ret = projection_result(axis, d.b(), d.a()); ret.has_value()) {
        proj.push_back(ret.value());
      } else {
        return ProjectionExpr{};
      }
    }
    return ProjectionExpr::single(proj);
  });
}

ProjectionExpr terms_with_num_distinct_variables(const ProjectionExpr& expr, int num_distinct) {
  return expr.filtered_key([&](const ProjectionExpr::StorageT& term) {
    return num_distinct_elements_unsorted(term) == num_distinct;
  });
}

ProjectionExpr terms_with_min_distinct_variables(const ProjectionExpr& expr, int min_distinct) {
  return expr.filtered_key([&](const ProjectionExpr::StorageT& term) {
    return num_distinct_elements_unsorted(term) >= min_distinct;
  });
}

void print_sorted_by_num_distinct_variables(std::ostream& os, const ProjectionExpr& expr) {
  to_ostream_grouped(
    os, expr, std::less<>{},
    [](const auto& term) {
      return num_distinct_elements_unsorted(term);
    },
    std::less<>{},
    [](int num_vars) {
      return absl::StrCat(num_vars, " vars");
    },
    LinearNoContext{}
  );
}
