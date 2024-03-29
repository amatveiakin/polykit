// Optimization potential. Define minus-minus and plus-plus function directly rather than a diff with full.
// TODO: Notation for full arrows: always a/b, always ←/↑ or depending on which function is called?

#include "chern_arrow.h"


#define IMPL(f_, f_impl_)                                                      \
  GammaExpr f_(const GammaExpr& expr, int num_dst_points) {                    \
    return f_impl_(expr, num_dst_points);                                      \
  }                                                                            \
  GammaNCoExpr f_(const GammaNCoExpr& expr, int num_dst_points) {              \
    return f_impl_(expr, num_dst_points);                                      \
  }


template<typename LinearT>
LinearT a_sum(const LinearT& expr, int num_dst_points, const std::vector<int>& summation_args) {
  const auto all_dst_points = seq_incl(1, num_dst_points);
  LinearT ret;
  for (int idx : summation_args) {
    idx--;
    const int removed_point = all_dst_points.at(idx);
    const int sign = neg_one_pow(removed_point + 1);
    const auto dst_points = removed_index(all_dst_points, idx);
    ret += sign * substitute_variables_1_based(expr, dst_points);
  }
  return ret;
}

template<typename LinearT>
LinearT b_sum(const LinearT& expr, int num_dst_points, const std::vector<int>& summation_args) {
  const auto all_dst_points = seq_incl(1, num_dst_points);
  LinearT ret;
  for (int idx : summation_args) {
    idx--;
    const int removed_point = all_dst_points.at(idx);
    const int sign = neg_one_pow(removed_point + 1);
    const auto dst_points = removed_index(all_dst_points, idx);
    ret += sign * pullback(substitute_variables_1_based(expr, dst_points), {removed_point});
  }
  return ret;
}

std::vector<int> ab_minus_summation_args(int num_dst_points) {
  return (num_dst_points % 2 == 0)
    ? seq_incl(1, num_dst_points / 2)
    : to_vector(range_incl(1, (num_dst_points + 1) / 2))
  ;
}

std::vector<int> ab_plus_summation_args(int num_dst_points) {
  return (num_dst_points % 2 == 0)
    ? seq_incl(num_dst_points / 2 + 1, num_dst_points)
    : to_vector(range_incl((num_dst_points + 1) / 2, num_dst_points))
  ;
}


template<typename LinearT>
LinearT chern_arrow_left_impl(const LinearT& expr, int num_dst_points) {
  return a_full_impl(
    expr, num_dst_points
  ).without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    // TODO: fmt notation for this
    return "←" + annotation;
  });
}

template<typename LinearT>
LinearT chern_arrow_up_impl(const LinearT& expr, int num_dst_points) {
  return b_full_impl(
    expr, num_dst_points
  ).without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    // TODO: fmt notation for this
    return "↑" + annotation;
  });
}

template<typename LinearT>
LinearT a_full_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = seq_incl(1, num_dst_points);
  return a_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function("a", {annotation});
  });
}

template<typename LinearT>
LinearT b_full_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = seq_incl(1, num_dst_points);
  return b_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function("b", {annotation});
  });
}

template<typename LinearT>
LinearT a_minus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_minus_summation_args(num_dst_points);
  return a_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("a", {"-"}), {annotation});
  });
}

template<typename LinearT>
LinearT a_plus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_plus_summation_args(num_dst_points);
  return a_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("a", {"+"}), {annotation});
  });
}

template<typename LinearT>
LinearT b_minus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_minus_summation_args(num_dst_points);
  return b_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("b", {"-"}), {annotation});
  });
}

template<typename LinearT>
LinearT b_plus_impl(const LinearT& expr, int num_dst_points) {
  const auto summation_args = ab_plus_summation_args(num_dst_points);
  return b_sum(expr, num_dst_points, summation_args).copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("b", {"+"}), {annotation});
  });
}

template<typename LinearT>
LinearT a_minus_minus_impl(const LinearT& expr, int num_dst_points) {
  return (
    a_full_impl(expr, num_dst_points) - a_plus_impl(expr, num_dst_points)
  ).without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("a", {"--"}), {annotation});
  });
}

template<typename LinearT>
LinearT a_plus_plus_impl(const LinearT& expr, int num_dst_points) {
  return (
    a_full_impl(expr, num_dst_points) - a_minus_impl(expr, num_dst_points)
  ).without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("a", {"++"}), {annotation});
  });
}

template<typename LinearT>
LinearT b_minus_minus_impl(const LinearT& expr, int num_dst_points) {
  return (
    b_full_impl(expr, num_dst_points) - b_plus_impl(expr, num_dst_points)
  ).without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("b", {"--"}), {annotation});
  });
}

template<typename LinearT>
LinearT b_plus_plus_impl(const LinearT& expr, int num_dst_points) {
  return (
    b_full_impl(expr, num_dst_points) - b_minus_impl(expr, num_dst_points)
  ).without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("b", {"++"}), {annotation});
  });
}

GammaNCoExpr c_minus(const GammaNCoExpr& expr, int /*num_points*/) {
  const auto ret = expr.is_zero()
    ? expr
    : ncoproduct(expr, plucker(seq_incl(1, expr.dimension())));
  return ret.without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("c", {"-"}), {annotation});
  });
}

GammaNCoExpr c_plus(const GammaNCoExpr& expr, int num_points) {
  const auto ret = expr.is_zero()
    ? expr
    : ncoproduct(expr, plucker(seq_incl(num_points - expr.dimension() + 1, num_points)));
  return ret.without_annotations().copy_annotations_mapped(expr, [&](const std::string& annotation) {
    return fmt::function(fmt::super("c", {"+"}), {annotation});
  });
}


IMPL(chern_arrow_left, chern_arrow_left_impl)
IMPL(chern_arrow_up, chern_arrow_up_impl)
IMPL(a_full, a_full_impl)
IMPL(a_minus, a_minus_impl)
IMPL(a_plus, a_plus_impl)
IMPL(b_full, b_full_impl)
IMPL(b_minus, b_minus_impl)
IMPL(b_plus, b_plus_impl)
IMPL(a_minus_minus, a_minus_minus_impl)
IMPL(a_plus_plus, a_plus_plus_impl)
IMPL(b_minus_minus, b_minus_minus_impl)
IMPL(b_plus_plus, b_plus_plus_impl)


template<typename F>
std::vector<F> list_ab_function(ABDoublePlusMinus double_plus_minus) {
  std::vector<F> ret = {
    DISAMBIGUATE(a_full),
    DISAMBIGUATE(a_minus),
    DISAMBIGUATE(a_plus),
    DISAMBIGUATE(b_full),
    DISAMBIGUATE(b_minus),
    DISAMBIGUATE(b_plus),
  };
  if (double_plus_minus == ABDoublePlusMinus::Include) {
    append_vector(ret, std::vector<F>{
      DISAMBIGUATE(a_minus_minus),
      DISAMBIGUATE(a_plus_plus),
      DISAMBIGUATE(b_minus_minus),
      DISAMBIGUATE(b_plus_plus),
    });
  }
  return ret;
}

std::vector<ABFunction> list_ab_function(ABDoublePlusMinus double_plus_minus) {
  return list_ab_function<ABFunction>(double_plus_minus);
}

std::vector<NCoABFunction> list_nco_ab_function(ABDoublePlusMinus double_plus_minus) {
  return list_ab_function<NCoABFunction>(double_plus_minus);
}
