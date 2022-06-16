#include "chern_arrow.h"


template<typename LinearT>
LinearT chern_arrow_left_impl(const LinearT& expr, int num_dst_points) {
  LinearT ret;
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  for (const int i : range(num_dst_points)) {
    const auto dst_points = removed_index(all_dst_points, i);
    ret += neg_one_pow(i) * substitute_variables(expr, dst_points);
  }
  return ret.copy_annotations_mapped(expr, [&](const std::string& annotation) {
    // TODO: fmt notation for this
    return "←" + annotation;
  });
}

template<typename LinearT>
LinearT chern_arrow_up_impl(const LinearT& expr, int num_dst_points) {
  LinearT ret;
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  for (const int i : range(num_dst_points)) {
    const auto [removed_points, dst_points] = split_indices(all_dst_points, {i});
    ret += neg_one_pow(i) * pullback(substitute_variables(expr, dst_points), removed_points);
  }
  return ret.copy_annotations_mapped(expr, [&](const std::string& annotation) {
    // TODO: fmt notation for this
    return "↑" + annotation;
  });
}

GammaExpr chern_arrow_left(const GammaExpr& expr, int num_dst_points) {
  return chern_arrow_left_impl(expr, num_dst_points);
}
GammaExpr chern_arrow_up(const GammaExpr& expr, int num_dst_points) {
  return chern_arrow_up_impl(expr, num_dst_points);
}

GammaNCoExpr chern_arrow_left(const GammaNCoExpr& expr, int num_dst_points) {
  return chern_arrow_left_impl(expr, num_dst_points);
}
GammaNCoExpr chern_arrow_up(const GammaNCoExpr& expr, int num_dst_points) {
  return chern_arrow_up_impl(expr, num_dst_points);
}
