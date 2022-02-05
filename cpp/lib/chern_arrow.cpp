#include "chern_arrow.h"


GammaExpr chern_arrow_left(const GammaExpr& expr, int num_dst_points) {
  GammaExpr ret;
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  for (const int i : range(num_dst_points)) {
    const auto dst_points = removed_index(all_dst_points, i);
    ret += neg_one_pow(i) * substitute_variables(expr, dst_points);
  }
  return ret;
}

GammaExpr chern_arrow_up(const GammaExpr& expr, int num_dst_points) {
  GammaExpr ret;
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  for (const int i : range(num_dst_points)) {
    const auto [removed_points, dst_points] = split_indices(all_dst_points, {i});
    ret += neg_one_pow(i) * pullback(substitute_variables(expr, dst_points), removed_points);
  }
  return ret;
}
