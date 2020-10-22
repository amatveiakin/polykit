#include "polylog.h"

#include "algebra.h"


static DeltaExpr Li_4_point(const std::vector<int>& p) {
  assert(p.size() == 4);
  return p[0] % 2 == 1
    ?  neg_cross_ratio(p[0], p[1], p[2], p[3])
    : -neg_cross_ratio(p[1], p[2], p[3], p[0]);
}

static DeltaExpr Li_impl(int weight, const std::vector<int>& points) {
  const int num_points = points.size();
  assert(num_points >= 4 && num_points % 2 == 0);
  const int min_weight = (num_points - 2) / 2;
  assert(weight >= min_weight);
  const auto subsums = [&]() {
    DeltaExpr ret;
    for (int i = 0; i < num_points - 3; ++i) {
      ret += tensor_product(
        Li_4_point(slice(points, i, i+4)),
        Li_impl(weight - 1, concat(slice(points, 0, i+1), slice(points, i+3)))
      );
    }
    return ret;
  };
  if (weight == min_weight) {
    if (num_points == 4) {
      return Li_4_point(points);
    } else {
      return subsums();
    }
  } else {
    DeltaExpr ret = tensor_product(
      cross_ratio(points),
      Li_impl(weight - 1, points)
    );
    if (num_points > 4) {
      ret += subsums();
    }
    return ret;
  }
}


// TODO: Add cache
DeltaExpr Li(int weight, const std::vector<int>& points) {
  return Li_impl(weight, points).annotate_with_function(
    "Li" + to_string(weight), points
  );
}
