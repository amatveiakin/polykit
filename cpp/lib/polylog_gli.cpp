// TODO: Rename GLi to GLi

#include "polylog_gli.h"

#include "call_cache.h"
#include "itertools.h"
#include "polylog_grqli.h"
#include "zip.h"


std::vector<int> apply_permutation(
  const std::vector<int>& order_before,
  const std::vector<int>& order_after,
  const std::vector<int>& points
) {
  absl::flat_hash_map<int, int> replacements;
  for (const auto& [before, after] : zip(order_before, order_after)) {
    replacements[before] = after;
  }
  return mapped(points, [&](const auto& p) {
    return value_or(replacements, p, p);
  });
}

template<typename F>
auto sum_alternating(const F& f, const std::vector<int>& points, const std::vector<int>& alternating) {
  decltype(f(points)) ret;
  for (const auto& [p, sign] : permutations_with_sign(alternating)) {
    ret += sign * f(apply_permutation(alternating, p, points));
  }
  return ret;
}

// Example for dimension == 4.
// Input:
//   qli_components:            (1-A), (1-B), (1-C)
//   qli_components (reversed): (1-C), (1-B), (1-A)
//   casimir_components:            ABC, BC, C
//   casimir_components (reversed): C, BC, ABC
//
// Table, non-recursive part:
//     (1-C)           (1-B)             (1-A)      }  q
//       C              BC                ABC       }  r
// ------------------------------------------------
//     (1-C)        (1-B)*(1-C)    (1-A)*(1-B)*(1-C)
//    C*(1-C)         ... [#]             ...
//   C*C*(1-C)          ...
//      ...
//
// For each cell where row>0, col>0:
//   t[row, col] = r[col] * t[row-1, col] + q[col] * t[row, col-1]
// e.g. for cell [#]:
//   BC * ((1-B)*(1-C)) + (1-B) * (C*(1-C)) = BC*(1-B)*(1-C) + (1-B)*C*(1-C)
//
// The answer is in the right-most column, the row depends on the weight.
GammaExpr GLi_component(
  int target_row,
  int target_col,
  absl::Span<const GammaExpr> qli_components,
  absl::Span<const GammaExpr> casimir_components
) {
  CallCache<GammaExpr, int, int> cache;
  const std::function<GammaExpr(int, int)> impl = [&](int row, int col) {
    if (row == 0) {
      return tensor_product(qli_components.subspan(qli_components.size() - col - 1));
    }
    if (col == 0) {
      return tensor_product(absl::MakeConstSpan(
        concat(std::vector(row, casimir_components.back()), {qli_components.back()})
      ));
    }
    return (
      + tensor_product(
        casimir_components.at(casimir_components.size() - col - 1),
        cache.apply(impl, row-1, col)
      )
      + tensor_product(
        qli_components.at(qli_components.size() - col - 1),
        cache.apply(impl, row, col-1)
      )
    );
  };
  return impl(target_row, target_col);
};

// Computes Grassmannian polylogarithm of dimension n, weight n-1 on 2n points.
GammaExpr GLiVec(int weight, const std::vector<int>& points) {
  // TODO: Define via GLiVec(ascending points) + substitute variables.
  //   Directly substituting duplicate points is not equivalent (why?)
  //   ... or maybe this shouldn't work (consult Danya)
  // TODO: What does this mean for operations on GammaExpr in general?
  //   Should other functions (like GrQLi) do the same?
  CHECK(all_unique_unsorted(points)) << "Unimplemented: duplicate GLi points: " << dump_to_string(points);
  CHECK(are_GLi_args_ok(weight, points.size())) << weight << ", " << dump_to_string(points);
  const int n = div_int(points.size(), 2);
  const auto inc_arg = [&](int& a) {
    if (a == n || a == 2*n) {
      return;  // 2 and 2n are fixed
    }
    ++a;
    if (a == n) {
      ++a;
    }
    CHECK_LT(a, 2*n);
  };
  const auto f = [&](const std::vector<int>& arguments) {
    std::vector<int> args_indices = concat(reversed(seq_incl(2, n-1)), {1, n, n+1, 2*n});
    std::vector<std::pair<std::vector<int>, std::vector<int>>> sub_arguments;
    for (const int i_component : range(n - 1)) {
      if (i_component != 0) {
        for (int& a : args_indices) {
          inc_arg(a);
        }
      }
      const auto& [bonus_indices, main_indices] = split_slice(args_indices, n - 2);
      sub_arguments.push_back({
        choose_indices_one_based(arguments, bonus_indices),
        choose_indices_one_based(arguments, main_indices)
      });
    }
    const std::vector<GammaExpr> qli_components = mapped(sub_arguments, [](const auto& args) {
      return GrQLiVec(1, args);
    });
    const std::vector<GammaExpr> log_components = mapped(sub_arguments, [](const auto& args) {
      return GrLogVec(args);
    });
    const std::vector<GammaExpr> casimir_components = mapped(range(n - 1), [&](const int k) {
      return sum(slice(log_components, k));
    });
    return GLi_component(weight-n+1, n-2, qli_components, casimir_components);
  };
  const auto f1 = [&](const std::vector<int>& arguments) {
    return sum_alternating(f, arguments, slice(arguments, 0, n-1));
  };
  return sum_alternating(f1, points, slice(points, n, 2*n-1)).without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("GLi"), {weight}),
      points
    )
  );
}

GammaExpr GLiVec(int weight, const std::vector<int>& pb_points, const std::vector<int>& li_points) {
  return pullback(GLiVec(weight, li_points), pb_points).without_annotations().annotate(
    fmt::function(
      fmt::sub_num(fmt::opname("GLi"), {weight}),
      {str_join_skip_empty(
        std::array{
          str_join(sorted(pb_points), ","),
          str_join(li_points, ",")
        },
        " / "
      )}
    )
  );
}


GammaExpr SymmGLi3(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  constexpr int weight = 3;
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  auto expr =
    + GLiVec(weight, args({1,2,3,4,5,6}))
  ;
  expr +=
    - GrQLiVec(weight, args({2}), args({3,4,5,6}))
    + GrQLiVec(weight, args({5}), args({1,2,3,6}))
    + GrQLiVec(weight, args({2}), args({1,3,4,5}))
    - GrQLiVec(weight, args({5}), args({1,2,4,6}))
    - GrQLiVec(weight, args({2}), args({1,3,4,6}))
    + GrQLiVec(weight, args({5}), args({1,3,4,6}))
    + GrQLiVec(weight, args({3}), args({1,2,4,6}))
    - GrQLiVec(weight, args({6}), args({1,3,4,5}))
    + GrQLiVec(weight, args({1}), args({3,4,5,6}))
    - GrQLiVec(weight, args({4}), args({1,2,3,6}))
  ;
  expr -=
    - GrQLiVec(weight, args({1}), args({2,3,4,6}))
    + GrQLiVec(weight, args({2}), args({1,3,4,5}))
    - GrQLiVec(weight, args({3}), args({2,4,5,6}))
    + GrQLiVec(weight, args({4}), args({1,3,5,6}))
    - GrQLiVec(weight, args({5}), args({1,2,4,6}))
    + GrQLiVec(weight, args({6}), args({1,2,3,5}))
  ;
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("SymmGLi"), {weight}),
      points
    )
  );
}

GammaExpr SymmGLi4_wip(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  constexpr int weight = 4;
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  auto expr =
    + GLiVec(weight, args({1,2,3,4,5,6}))
    - GLiVec(weight, args({2,3,4,5,6,1}))
    + GLiVec(weight, args({3,4,5,6,1,2}))
    - GLiVec(weight, args({4,5,6,1,2,3}))
    + GLiVec(weight, args({5,6,1,2,3,4}))
    - GLiVec(weight, args({6,1,2,3,4,5}))
  ;
  expr -=
    + GLiVec(weight, args({1,6,5,4,3,2}))
    - GLiVec(weight, args({6,5,4,3,2,1}))
    + GLiVec(weight, args({5,4,3,2,1,6}))
    - GLiVec(weight, args({4,3,2,1,6,5}))
    + GLiVec(weight, args({3,2,1,6,5,4}))
    - GLiVec(weight, args({2,1,6,5,4,3}))
  ;
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("SymmGLi"), {weight}),
      points
    )
  );
}

GammaExpr AlternatedGLiVec(const std::vector<int>& points) {
  CHECK(points.size() % 2 == 0) << dump_to_string(points);
  const int n = div_int(points.size(), 2);
  GammaExpr expr;
  for (const int i : range(0, n - 1)) {
    for (const int j : range(0, n - 1)) {
      const auto args = removed_indices(points, {i, n + j});
      expr += (neg_one_pow(i) * neg_one_pow(j)) * GLiVec(n - 1, args);
    }
  }
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::opname("AlternatedGLi"),
      points
    )
  );
}

bool are_GLi_args_ok(int weight, int num_points) {
  return num_points % 2 == 0 && weight >= num_points / 2 - 1;
}

bool is_canonical_GLi_arg_order(const std::vector<int>& points) {
  const int p = div_int(points.size(), 2);
  return
    absl::c_is_sorted(slice(points, 0, p - 1)) &&
    absl::c_is_sorted(slice(points, p, 2 * p - 1)) &&
    points[0] < points[p]
  ;
}
