#include "chern_arrow.h"


#define IMPL(f_, f_impl_)                                                      \
  GammaExpr f_(const GammaExpr& expr, int num_dst_points) {                    \
    return f_impl_(expr, num_dst_points);                                      \
  }                                                                            \
  GammaNCoExpr f_(const GammaNCoExpr& expr, int num_dst_points) {              \
    return f_impl_(expr, num_dst_points);                                      \
  }


template<typename LinearT>
LinearT arrow_left_sum(const LinearT& expr, int num_dst_points, const std::vector<int>& summation_args) {
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  LinearT ret;
  for (int idx : summation_args) {
    idx--;
    const int removed_point = all_dst_points.at(idx);
    const int sign = neg_one_pow(removed_point + 1);
    const auto dst_points = removed_index(all_dst_points, idx);
    ret += sign * substitute_variables(expr, dst_points);
  }
  return ret;
}

template<typename LinearT>
LinearT arrow_up_sum(const LinearT& expr, int num_dst_points, const std::vector<int>& summation_args) {
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  LinearT ret;
  for (int idx : summation_args) {
    idx--;
    const int removed_point = all_dst_points.at(idx);
    const int sign = neg_one_pow(removed_point + 1);
    const auto dst_points = removed_index(all_dst_points, idx);
    ret += sign * pullback(substitute_variables(expr, dst_points), {removed_point});
  }
  return ret;
}

std::vector<int> ab_minus_summation_args(int num_dst_points) {
  return (num_dst_points % 2 == 0)
    ? to_vector(range_incl(1, num_dst_points / 2))
    : to_vector(range_incl(1, (num_dst_points + 1) / 2))
  ;
}

std::vector<int> ab_plus_summation_args(int num_dst_points) {
  return (num_dst_points % 2 == 0)
    ? to_vector(range_incl(num_dst_points / 2 + 1, num_dst_points))
    : to_vector(range_incl((num_dst_points + 1) / 2, num_dst_points))
  ;
}


template<typename LinearT>
LinearT chern_arrow_left_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = to_vector(range_incl(1, num_dst_points));
  return arrow_left_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    // TODO: fmt notation for this
    return "←" + annotation;
  });
}

template<typename LinearT>
LinearT chern_arrow_up_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = to_vector(range_incl(1, num_dst_points));
  return arrow_up_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    // TODO: fmt notation for this
    return "↑" + annotation;
  });
}

template<typename LinearT>
LinearT a_minus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_minus_summation_args(num_dst_points);
  return arrow_left_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("a", {"-"}), {annotation});
  });
}

template<typename LinearT>
LinearT a_plus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_plus_summation_args(num_dst_points);
  return arrow_left_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("a", {"+"}), {annotation});
  });
}

template<typename LinearT>
LinearT b_minus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_minus_summation_args(num_dst_points);
  return arrow_up_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("b", {"-"}), {annotation});
  });
}

template<typename LinearT>
LinearT b_plus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_plus_summation_args(num_dst_points);
  return arrow_up_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("b", {"+"}), {annotation});
  });
}


IMPL(chern_arrow_left, chern_arrow_left_impl)
IMPL(chern_arrow_up, chern_arrow_up_impl)
IMPL(a_minus, a_minus_impl)
IMPL(a_plus, a_plus_impl)
IMPL(b_minus, b_minus_impl)
IMPL(b_plus, b_plus_impl)
