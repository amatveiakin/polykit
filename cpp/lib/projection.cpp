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


bool is_frozen_coord(const Delta& d, int num_vars) {
  const auto [a, b] = delta_points_inv(d);
  const bool same_sign = a.var_sign() == b.var_sign();
  const int diff = std::abs(a.idx() - b.idx());
  return (
    (mod_eq(diff, 1, num_vars) && same_sign) ||
    (mod_eq(diff, -1, num_vars) && !same_sign)
  );
}

DeltaExpr alt_project_on(const Delta& axis, const DeltaExpr& expr, int num_vars) {
  return expr.filtered([&](const auto& term) {
    return absl::c_all_of(term, [&](const Delta& d) {
      return !are_weakly_separated_inv(d, axis) || is_frozen_coord(d, num_vars);
    });
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
    &ProjectionExpr::Param::object_to_string
  );
}
