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

ProjectionExpr involute_projected(const DeltaExpr& expr, const std::vector<int>& involution, int axis) {
  CHECK(absl::c_count(involution, axis) == 1);
  const int n = involution.size();
  CHECK_EQ(n, 6);
  absl::flat_hash_map<int, int> inv;
  for (const int i : range(involution.size())) {
    const int a = involution.at(i);
    const int b = involution.at((i + n/2) % n);
    CHECK(!inv.contains(a));
    inv[a] = b;
  }
  const int axis_inv = inv.at(axis);
  return expr.mapped_expanding([&](const std::vector<Delta>& deltas) {
    ProjectionExpr::StorageT proj;
    for (const Delta& d : deltas) {
      CHECK(!d.is_nil());
      if (d == Delta(axis, axis_inv)) {
        proj.push_back(axis_inv);
      } else if (d.contains(axis)) {
        proj.push_back(d.other_point(axis));
      } else if (d.contains(axis_inv)) {
        proj.push_back(inv.at(d.other_point(axis_inv)));
      } else {
        return ProjectionExpr{};
      }
    }
    return ProjectionExpr::single_key(proj);
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
