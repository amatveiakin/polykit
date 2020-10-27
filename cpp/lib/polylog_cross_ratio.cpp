#include "polylog_cross_ratio.h"

#include "absl/strings/str_cat.h"

#include "algebra.h"
#include "check.h"


static DeltaExpr Lira_4_point(const std::vector<int>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0] % 2 == 1
    ?  neg_cross_ratio(p[0], p[1], p[2], p[3])
    : -neg_cross_ratio(p[1], p[2], p[3], p[0]);
}

static DeltaExpr Lira_impl(int weight, const std::vector<int>& points) {
  const int num_points = points.size();
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of Lira points: " << num_points;
  const int min_weight = (num_points - 2) / 2;
  CHECK_GE(weight, min_weight);
  const auto subsums = [&]() {
    DeltaExpr ret;
    for (int i = 0; i < num_points - 3; ++i) {
      ret += tensor_product(
        Lira_4_point(slice(points, i, i+4)),
        Lira_impl(weight - 1, concat(slice(points, 0, i+1), slice(points, i+3)))
      );
    }
    return ret;
  };
  if (weight == min_weight) {
    if (num_points == 4) {
      return Lira_4_point(points);
    } else {
      return subsums();
    }
  } else {
    DeltaExpr ret = tensor_product(
      cross_ratio(points),
      Lira_impl(weight - 1, points)
    );
    if (num_points > 4) {
      ret += subsums();
    }
    return ret;
  }
}


// Optimization potential: Add cache
DeltaExpr Lira(int weight, const std::vector<int>& points) {
  const int num_points = points.size();
  const auto asc_points = seq_incl(1, num_points);
  return delta_expr_substitute(
    Lira_impl(weight, asc_points),
    points
  ).annotate(function_to_string(
    "Lira_" + to_string(weight), points
  ));
}
