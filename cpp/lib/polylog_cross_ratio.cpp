#include "polylog_cross_ratio.h"

#include "absl/strings/str_cat.h"

#include "algebra.h"
#include "call_cache.h"
#include "check.h"


static DeltaExpr Lido_4_point(const std::vector<int>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0] % 2 == 1
    ?  neg_cross_ratio(p[0], p[1], p[2], p[3])
    : -neg_cross_ratio(p[1], p[2], p[3], p[0]);
}

static DeltaExpr Lido_impl(int weight, const std::vector<int>& points) {
  const int num_points = points.size();
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of Lido points: " << num_points;
  const int min_weight = (num_points - 2) / 2;
  CHECK_GE(weight, min_weight);
  const auto subsums = [&]() {
    DeltaExpr ret;
    for (int i = 0; i < num_points - 3; ++i) {
      ret += tensor_product(
        Lido_4_point(slice(points, i, i+4)),
        Lido_impl(weight - 1, concat(slice(points, 0, i+1), slice(points, i+3)))
      );
    }
    return ret;
  };
  if (weight == min_weight) {
    if (num_points == 4) {
      return Lido_4_point(points);
    } else {
      return subsums();
    }
  } else {
    DeltaExpr ret = tensor_product(
      cross_ratio(points),
      Lido_impl(weight - 1, points)
    );
    if (num_points > 4) {
      ret += subsums();
    }
    return ret;
  }
}

DeltaExpr LidoVec(int weight, const std::vector<X>& points) {
  const int num_points = points.size();
  const auto asc_points = seq_incl(1, num_points);
  static CallCache<DeltaExpr, int, std::vector<int>> call_cache;
  // TODO: Debug delta_expr_substitute performance. It takes about 2/3
  // of total execution time, which is nuts.
  return delta_expr_substitute(
    call_cache.apply(&Lido_impl, weight, asc_points),
    points
  ).annotate(fmt::function(
    fmt::sub_num("Lido", {weight}),
    mapped(points, [](X x){ return to_string(x); })
  ));
}

DeltaExpr LidoVec(int weight, const std::vector<int>& points) {
  return LidoVec(weight, mapped(points, X::Var));
}

DeltaExpr LidoSymmVec(int weight, const std::vector<X>& points) {
  auto [x1, x2, x3, x4, x5, x6] = as_array<6>(points);
  return (
    + LidoVec(weight, {x1,x2,x3,x4,x5,x6})
    - LidoVec(weight, {x1,x2,x3,x4})
    - LidoVec(weight, {x3,x4,x5,x6})
    - LidoVec(weight, {x5,x6,x1,x2})
  ).without_annotations().annotate(fmt::function(
    fmt::sub_num("LidoSymm", {weight}),
    mapped(points, [](X x){ return to_string(x); })
  ));
}

DeltaExpr LidoSymmVec(int weight, const std::vector<int>& points) {
  return LidoSymmVec(weight, mapped(points, X::Var));
}
