#include "polylog_gr_space.h"

#include "itertools.h"
#include "parallel_util.h"
#include "polylog_cgrli.h"
#include "polylog_grli.h"
#include "polylog_grqli.h"
#include "polylog_qli.h"
#include "polylog_type_ac_space.h"
#include "vector_space.h"


// Permutes elements such that
//   cross_ratio(one_minus_cross_ratio(p)) == 1 - cross_ratio(p)
template<typename Container>
static Container one_minus_cross_ratio(Container p) {
  CHECK_EQ(4, p.size());
  std::swap(p[1], p[2]);
  return p;
}


Gr_Space gr_free_lie_coalgebra(int weight, int dimension, const std::vector<int>& args) {
  const auto coords = combinations(args, dimension);
  return mapped(get_lyndon_words(coords, weight), [](const auto& word) {
    return GammaExpr::single(mapped(word, convert_to<Gamma>));
  });
}

Gr_Space GrFx(int dimension, const std::vector<int>& args) {
  return mapped(combinations(args, dimension), DISAMBIGUATE(G));
}

Gr_Space GrL_core(
  int weight, int dimension, const std::vector<int>& args,
  bool include_one_minus_cross_ratio, int num_fixed_points
) {
  CHECK_LE(2, dimension);
  CHECK_LE(num_fixed_points, args.size());
  const auto& [main_args, fixed_p] = split_slice(args, args.size() - num_fixed_points);
  const auto& fixed_points = fixed_p;  // workaround: lambdas cannot capture structured bindings
  Gr_Space ret;
  for (const auto& [bonus_p, qli_points] : index_splits(main_args, dimension - 2)) {
    const auto& bonus_points = bonus_p;  // workaround: lambdas cannot capture structured bindings
    append_vector(
      ret,
      mapped_expanding(combinations(qli_points, 4 - num_fixed_points), [&](auto p) {
        const auto qli_args = concat(p, fixed_points);
        std::vector ret = {GrQLiVec(weight, bonus_points, qli_args)};
        if (include_one_minus_cross_ratio) {
          ret.push_back(GrQLiVec(weight, bonus_points, one_minus_cross_ratio(qli_args)));
        }
        return ret;
      })
    );
  }
  return ret;
}

Gr_Space GrL1(int dimension, const std::vector<int>& args) {
  return GrL_core(1, dimension, args, true, 2);  // equivalent to zero fixed points
}

Gr_Space GrL2(int dimension, const std::vector<int>& args) {
  return GrL_core(2, dimension, args, false, 1);  // equivalent to zero fixed points
}

Gr_Space GrL3(int dimension, const std::vector<int>& args) {
  const int weight = 3;
  Gr_Space ret = GrL_core(weight, dimension, args, true, 0);
  if (dimension >= weight) {
    for (const auto& [bonus_p, qli_points] : index_splits(args, dimension - weight)) {
      const auto& bonus_points = bonus_p;  // workaround: lambdas cannot capture structured bindings
      append_vector(
        ret,
        mapped(combinations(qli_points, 6), [&](auto p) {
          return GrLiVec(bonus_points, p);
        })
      );
    }
  }
  return ret;
}

Gr_Space GrL4_Dim3(const std::vector<int>& args) {
  Gr_Space ret;
  for (const auto& [bonus_p, lower_dim_points] : index_splits(args, 1)) {
    const auto& bonus_points = bonus_p;  // workaround: lambdas cannot capture structured bindings
    append_vector(ret, mapped(L4(lower_dim_points), [&](const auto& expr) {
      return pullback(expr, bonus_points);
    }));
  }
  append_vector(
    ret,
    mapped(permutations(args, 6), [&](auto p) {
      return CGrLi(4, p);
    })
  );
  return ret;
}

Gr_Space GrL(int weight, int dimension, const std::vector<int>& args) {
  switch (weight) {
    case 1: return GrL1(dimension, args);
    case 2: return GrL2(dimension, args);
    case 3: return GrL3(dimension, args);
    case 4: {
      if (dimension == 3) {
        return GrL4_Dim3(args);
      }
      break;
    }
  }
  FATAL(absl::StrCat("Unsupported weight&dimension for GrL: ", weight, "&", dimension));
}


Gr_Space CGrL_Dim3_naive_test_space(int weight, const std::vector<int>& points) {
  Gr_Space space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CL(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  for (const auto& args : combinations(points, 6)) {
    for (const int shift : {0, 1, 2}) {
      space.push_back(CGrLi(weight, rotated_vector(args, shift)));
    }
  }
  return space;
}

Gr_Space CGrL3_Dim3_test_space(const std::vector<int>& points) {
  const int weight = 3;
  Gr_Space space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CL(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  {
    const int fixed_points_idx = 0;
    const auto fixed_args = choose_indices(points, {fixed_points_idx});
    const auto var_args_pool = removed_index(points, fixed_points_idx);
    for (const auto& var_args : combinations(var_args_pool, 5)) {
      space.push_back(CGrLi(weight, concat(fixed_args, var_args)));
    }
  }
  return space;
}

Gr_Space CGrL_Dim4_naive_test_space(int weight, const std::vector<int>& points) {
  Gr_Space space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CGrL_Dim3_naive_test_space(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  for (const auto& args : combinations(points, 6)) {
    space.push_back(plucker_dual(QLiVec(weight, args), args));
  }
  for (const auto& args : combinations(points, 8)) {
    for (const int shift : range(args.size() / 2)) {
      space.push_back(CGrLi(weight, rotated_vector(args, shift)));
    }
  }
  return space;
}

// TODO: Consider always using plucker dual when dimension > num_points/2
Gr_Space CGrL_test_space(int weight, int dimension, const std::vector<int>& points) {
  if (dimension >= points.size() - 1) {
    return {};
  }
  CHECK_LE(1, weight);
  CHECK_LE(2, dimension);
  switch (weight) {
    case 1: return GrL1(dimension, points);
    case 2: return GrL2(dimension, points);
    default: break;  // in weight > 2 cluster polylogs are different from non-cluster
  }
  switch (dimension) {
    case 2: return mapped(CL(weight, points), DISAMBIGUATE(delta_expr_to_gamma_expr));
    case 3: return weight == 3 ? CGrL3_Dim3_test_space(points) : CGrL_Dim3_naive_test_space(weight, points);
    case 4: return CGrL_Dim4_naive_test_space(weight, points);
  }
  if (dimension * 2 > points.size()) {
    const int dual_dim = points.size() - dimension;
    const auto dual_space = CGrL_test_space(weight, dual_dim, points);
    return mapped(dual_space, [&](const auto& expr) {
      return plucker_dual(expr, points);
    });
  }
  FATAL(absl::StrCat("Unsupported weight&dimension for CGrL: ", weight, "&", dimension));
}

Gr_Space ChernGrL(int weight, int dimension, const std::vector<int>& points, int depth) {
  Gr_Space space;
  if (dimension > 2) {
    for (const int bonus_point_idx : range(points.size())) {
      const auto bonus_args = choose_indices(points, {bonus_point_idx});
      const auto main_args = removed_index(points, bonus_point_idx);
      append_vector(space, mapped(ChernGrL(weight, dimension - 1, main_args, depth), [&](const auto& expr) {
        return pullback(expr, bonus_args);
      }));
    }
  }
  if (weight >= dimension - 1 && depth >= dimension - 1) {
    for (const auto& args : combinations(points, dimension * 2)) {
      for (const int shift : range(args.size() / 2)) {
        space.push_back(CGrLi(weight, rotated_vector(args, shift)));
      }
    }
  }
  return space;
}


Gr_NCoSpace simple_co_GrL(int weight, int num_coparts, int dimension, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  return co_space(weight, num_coparts, [&](const int w) {
    return mapped(GrL(w, dimension, points), [&](const auto& expr) {
      // Precompute Lyndon basis to speed up coproduct.
      return to_lyndon_basis(normalize_remove_consecutive(expr));
    });
  });
}

Gr_NCoSpace simple_co_CGrL_test_space(int weight, int dimension, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  return co_space(weight, 2, [&](int w) {
    return normalize_space_remove_consecutive([&]() {
      // if (w == 1) {
      //   return GrFx(dimension, points);
      // }
      return CGrL_test_space(w, dimension, points);
    }(), dimension, num_points);
  });
}

Gr_NCoSpace wedge_ChernGrL(int weight, int dimension, const std::vector<int>& points) {
  // Precompute Lyndon to speed up coproduct.
  const auto chern_space = mapped(ChernGrL(weight - 1, dimension, points), DISAMBIGUATE(to_lyndon_basis));
  const auto fx_space = mapped(GrFx(dimension, points), DISAMBIGUATE(to_lyndon_basis));
  return filtered(
    mapped_parallel(cartesian_product(chern_space, fx_space), applied(DISAMBIGUATE(ncoproduct))),
    DISAMBIGUATE(is_totally_weakly_separated)
  );
}
