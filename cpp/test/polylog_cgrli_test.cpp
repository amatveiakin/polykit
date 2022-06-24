#include "lib/polylog_cgrli.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_gr_space.h"
#include "lib/polylog_qli.h"
#include "lib/zip.h"
#include "test_util/matchers.h"
#include "test_util/space_matchers.h"


// TODO: Deduplicate helper functions against polylog_cgrli.cpp
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

GammaExpr AomotoPolylog(const std::vector<int>& points) {
  const int weight = div_int(points.size(), 2) - 1;
  return CGrLiVec(weight, points);
}

// Note. Synced with casimir_components in CGrLi definition.
// TODO: Factor out, sync with CasimirDim3 and with cross_product.
GammaExpr Casimir(const std::vector<int>& points) {
  CHECK(points.size() % 2 == 0);
  const int p = div_int(points.size(), 2);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return (
    + G(args(to_vector(range_incl(1, p))))
    + G(args(to_vector(range_incl(p+1, 2*p))))
    - G(args(concat({2*p}, to_vector(range_incl(1, p-1)))))
    - G(args(to_vector(range_incl(p, 2*p-1))))
  ).annotate(
    fmt::function_num_args(fmt::opname("Cas"), points)
  );
}

GammaExpr CasimirDim3(const std::vector<int>& points) {
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  CHECK_EQ(points.size(), 6);
  return
    + G(args({6,1,2}))
    + G(args({3,4,5}))
    - G(args({6,1,5}))
    - G(args({2,3,4}))
  ;
}

GammaExpr CGrLi_Dim3_alternative(int weight, const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  CHECK_LE(2, weight);
  const auto qli1 = [&](int bonus_arg, const std::vector<int>& main_args) {
    return GrQLiVec(1, choose_indices_one_based(points, {bonus_arg}), choose_indices_one_based(points, main_args));
  };
  const auto qlin = [&](int bonus_arg, const std::vector<int>& main_args) {
    return GrQLiVec(weight - 1, choose_indices_one_based(points, {bonus_arg}), choose_indices_one_based(points, main_args));
  };
  auto ret = (
    + tensor_product(qli1(6, {1,2,3,5}), qlin(3, {4,5,6,2}))
    - tensor_product(qli1(6, {1,2,4,5}), qlin(4, {3,5,6,2}))
    - tensor_product(qli1(1, {6,2,3,5}), qlin(3, {4,5,1,2}))
    + tensor_product(qli1(1, {6,2,4,5}), qlin(4, {3,5,1,2}))
  );
  if (weight > 2) {
    ret += tensor_product(CasimirDim3(points), CGrLi_Dim3_alternative(weight - 1, points));
  }
  return ret.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("CGrLiVec"), {weight}),
      points
    )
  );
}

GammaExpr CGrLi_Dim4_Weight3_alternative(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 8);
  const auto f = [](const std::vector<int>& arguments) {
    CHECK_EQ(arguments.size(), 8);
    const auto args = [&](const std::vector<int>& indices) {
      return choose_indices_one_based(arguments, indices);
    };
    return tensor_product(absl::MakeConstSpan({
      GrQLiVec(1, args({2,3}), args({1,4,7,8})),
      GrQLiVec(1, args({3,7}), args({2,4,5,8})),
      GrQLiVec(1, args({5,7}), args({3,4,6,8})),
    }));
  };
  const auto f1 = [&](const std::vector<int>& arguments) {
    return sum_alternating(f, arguments, choose_indices_one_based(arguments, {1,2,3}));
  };
  return sum_alternating(f1, points, choose_indices_one_based(points, {5,6,7}));
}

GammaExpr CGrLi_Dim4_Weight4_alternative(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 8);
  const auto f = [](const std::vector<int>& arguments) {
    CHECK_EQ(arguments.size(), 8);
    const auto args = [&](const std::vector<int>& bonus_points, const std::vector<int>& main_points) {
      return std::pair{choose_indices_one_based(arguments, bonus_points), choose_indices_one_based(arguments, main_points)};
    };
    const auto a = args({2,3}, {1,4,7,8});
    const auto b = args({3,7}, {2,4,5,8});
    const auto c = args({5,7}, {3,4,6,8});
    return
      + tensor_product(absl::MakeConstSpan({
        GrLogVec(a) + GrLogVec(b) + GrLogVec(c),
        GrQLiVec(1, a),
        GrQLiVec(1, b),
        GrQLiVec(1, c),
      }))
      + tensor_product(absl::MakeConstSpan({
        GrQLiVec(1, a),
        GrLogVec(b) + GrLogVec(c),
        GrQLiVec(1, b),
        GrQLiVec(1, c),
      }))
      + tensor_product(absl::MakeConstSpan({
        GrQLiVec(1, a),
        GrQLiVec(1, b),
        GrLogVec(c),
        GrQLiVec(1, c),
      }))
    ;
  };
  const auto f1 = [&](const std::vector<int>& arguments) {
    return sum_alternating(f, arguments, choose_indices_one_based(arguments, {1,2,3}));
  };
  return sum_alternating(f1, points, choose_indices_one_based(points, {5,6,7}));
}


TEST(CGrLiTest, CustomPointOrder) {
  // Should be true for any weight and dimension.
  for (const int weight : range_incl(2, 3)) {
    const std::vector points = {2,1,3,6,5,4};
    EXPECT_EXPR_EQ(
      CGrLiVec(weight, points),
      substitute_variables(CGrLiVec(weight, {1,2,3,4,5,6}), points)
    );
  }
}

TEST(CGrLiTest, IsTotallyWeaklySeparated) {
  // Should be true for any weight and dimension.
  for (const int dimension : range_incl(2, 4)) {
    for (const int weight : range_incl(3, 4)) {
      const auto points = to_vector(range_incl(1, 2 * dimension));
      EXPECT_TRUE(is_totally_weakly_separated(CGrLiVec(weight, points)));
    }
  }
}

TEST(CGrLiTest, CGrLiOfFourPointsIsQLi) {
  for (const int weight : range_incl(2, 5)) {
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      CGrLiVec(weight, {1,2,3,4}),
      delta_expr_to_gamma_expr(QLiVec(weight, {1,2,3,4}))
    );
  }
}

TEST(CGrLiTest, EqualsAlternative_Dim3) {
  for (const int weight : range_incl(2, 5)) {
    // TODO: Sync indices
    EXPECT_EXPR_EQ(CGrLiVec(weight, {1,6,2,3,4,5}), CGrLi_Dim3_alternative(weight, {1,2,3,4,5,6}));
  }
}

TEST(CGrLiTest, EqualsAlternative_Dim4_Weight3) {
  EXPECT_EXPR_EQ(CGrLi3(1,2,3,4,5,6,7,8), CGrLi_Dim4_Weight3_alternative({1,2,3,4,5,6,7,8}));
}

TEST(CGrLiTest, EqualsAlternative_Dim4_Weight4) {
  EXPECT_EXPR_EQ(CGrLi4(1,2,3,4,5,6,7,8), CGrLi_Dim4_Weight4_alternative({1,2,3,4,5,6,7,8}));
}

TEST(CGrLiTest, SymmCGrLiIsSymmetric) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + SymmCGrLi3({1,2,3,4,5,6})
    + SymmCGrLi3({2,3,4,5,6,1})
  );
}

TEST(CGrLiTest, LARGE_AomotoPolylogProperties) {
  // First: any permutation of (1..n) and any permutation of (n+1..2n) multiplies the function
  // by permutation sign.
  // For (1,2,3) and (5,6,7) it's trivial from definition, so checking the rest:
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + CGrLi3(1,2,3,4,5,6,7,8)
    + CGrLi3(1,2,4,3,5,6,7,8)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + CGrLi3(1,2,3,4,5,6,7,8)
    + CGrLi3(1,2,3,4,5,6,8,7)
  );
  // Second: swap (1..n) and (n+1..2n).
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + CGrLi3(1,2,3,4,5,6,7,8)
    - CGrLi3(5,6,7,8,1,2,3,4)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + CGrLi4(1,2,3,4,5,6,7,8,9,10)
    + CGrLi4(6,7,8,9,10,1,2,3,4,5)
  );
  // Third:
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + CGrLi3(1,2,3,4,6,7,8,9)
    - CGrLi3(1,2,3,5,6,7,8,9)
    + CGrLi3(1,2,4,5,6,7,8,9)
    - CGrLi3(1,3,4,5,6,7,8,9)
    + CGrLi3(2,3,4,5,6,7,8,9)
  );
}

TEST(CGrLiTest, CGrLiViaLowerNumberOfPoints) {
  // Should be true for any weight
  for (const int weight : range_incl(2, 3)) {
    const int p = weight + 1;
    const int num_points = 2 * p;
    const auto points = to_vector(range_incl(1, num_points));
    const auto lhs = CGrLiVec(weight, points);
    GammaExpr rhs;
    for (const int i : range(p)) {
      for (const int j : range(p, 2 * p)) {
        const int sign = neg_one_pow(points[i] + points[j]);
        rhs += sign * CGrLiVec(weight, {points[j]}, removed_indices(points, {i, j}));
      }
    }
    EXPECT_EXPR_EQ_AFTER_LYNDON(lhs, neg_one_pow(weight) * rhs);
  }
}

TEST(CGrLiTest, Comultiplication) {
  for (const int weight : range_incl(2, 4)) {
    for (const int p : range_incl(2, 5)) {
      const int num_points = p * 2;
      if (!are_CGrLi_args_ok(weight, num_points)) {
        continue;
      }
      const auto points = to_vector(range_incl(1, num_points));
      const auto lhs = ncomultiply(CGrLiVec(weight, points));
      GammaNCoExpr rhs;
      if (weight >= p) {
        rhs -= ncoproduct(CGrLiVec(weight - 1, points), Casimir(points));
      }
      for (const int k : range_incl(1, p - 2)) {
        for (const auto& [i, i_complement] : index_splits(slice(points, 0, p - 1), k)) {
          for (const auto& [j, j_complement] : index_splits(slice(points, p, 2 * p - 1), p - k - 1)) {
            // TODO: Fix permutation sign in case of custom point order (should look at indices, not point numbers!)
            const int sign = permutation_sign(concat(i_complement, i)) * permutation_sign(concat(j, j_complement));
            const std::vector points_p = {points.at(p - 1)};
            const std::vector points_2p = {points.at(2 * p - 1)};
            for (const int w_1 : range(1, weight)) {
              const int w_2 = weight - w_1;
              const auto args_1 = concat(i_complement, points_p, j, points_2p);
              const auto args_2 = concat(i, points_p, j_complement, points_2p);
              if (are_CGrLi_args_ok(w_1, args_1.size()) && are_CGrLi_args_ok(w_2, args_2.size())) {
                rhs += sign * ncoproduct(
                  pullback(CGrLiVec(w_1, args_1), i),
                  pullback(CGrLiVec(w_2, args_2), j)
                );
              }
            }
          }
        }
      }
      EXPECT_EXPR_EQ_AFTER_LYNDON(lhs, rhs);
    }
  }
}

TEST(CGrLiTest, ComultiplicationAomoto) {
  // Simplified formula for (1, n-1) comultiplication component of Aomoto polylogarithm,
  // Proposition 2.3 from https://arxiv.org/pdf/math/0011168.pdf
  for (const int weight : range_incl(3, 4)) {
    const int p = weight + 1;
    const int num_points = 2 * p;
    const auto points = to_vector(range_incl(1, num_points));
    const auto lhs = ncomultiply(CGrLiVec(weight, points), {1, weight - 1});
    GammaNCoExpr rhs;
    for (const int i : range(p)) {
      for (const int j : range(p, 2 * p)) {
        const int sign = neg_one_pow(points[i] + points[j]);
        rhs += sign * (
          + ncoproduct(
            CGrLiVec(weight - 1, {points[j]}, removed_indices(points, {i, j})),
            plucker(concat({points[j]}, removed_index(slice(points, 0, p), i)))
          )
          - ncoproduct(
            CGrLiVec(weight - 1, {points[i]}, removed_indices(points, {i, j})),
            plucker(concat({points[i]}, removed_index(slice(points, p), j - p)))
          )
        );
      }
    }
    EXPECT_EXPR_EQ_AFTER_LYNDON(lhs, -neg_one_pow(weight) * rhs);
  }
}

TEST(CGrLiTest, CGrLi3Equation) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + CGrLi3(1,2,3,4,5,6)
    - CGrLi3(1,2,3,4,5,7)
    + CGrLi3(1,2,3,4,6,7)
    - CGrLi3(1,2,3,5,6,7)
    + CGrLi3(1,2,4,5,6,7)
    - CGrLi3(1,3,4,5,6,7)
    + CGrLi3(2,3,4,5,6,7)
    - GrQLi3(1)(2,4,5,6)
    + GrQLi3(1)(2,4,5,7)
    - GrQLi3(1)(2,4,6,7)
    + GrQLi3(1)(3,4,5,6)
    - GrQLi3(1)(3,4,5,7)
    + GrQLi3(1)(3,4,6,7)
    + GrQLi3(2)(1,4,5,6)
    - GrQLi3(2)(1,4,5,7)
    + GrQLi3(2)(1,4,6,7)
    - GrQLi3(2)(3,4,5,6)
    + GrQLi3(2)(3,4,5,7)
    - GrQLi3(2)(3,4,6,7)
    - GrQLi3(3)(1,4,5,6)
    + GrQLi3(3)(1,4,5,7)
    - GrQLi3(3)(1,4,6,7)
    + GrQLi3(3)(2,4,5,6)
    - GrQLi3(3)(2,4,5,7)
    + GrQLi3(3)(2,4,6,7)
  );
}

TEST(CGrLiTest, SymmCGrLi3Equation) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + SymmCGrLi3({1,2,3,4,5,6})
    - SymmCGrLi3({1,2,3,4,5,7})
    + SymmCGrLi3({1,2,3,4,6,7})
    - SymmCGrLi3({1,2,3,5,6,7})
    + SymmCGrLi3({1,2,4,5,6,7})
    - SymmCGrLi3({1,3,4,5,6,7})
    + SymmCGrLi3({2,3,4,5,6,7})
    - GrQLi3(1)(2,3,4,6)
    + GrQLi3(1)(2,3,5,7)
    - GrQLi3(1)(2,4,5,6)
    - GrQLi3(1)(2,4,6,7)
    + GrQLi3(1)(3,4,5,7)
    + GrQLi3(1)(3,5,6,7)
    + GrQLi3(2)(1,3,4,6)
    - GrQLi3(2)(1,3,5,7)
    + GrQLi3(2)(1,4,5,6)
    + GrQLi3(2)(1,4,6,7)
    - GrQLi3(2)(3,4,5,7)
    - GrQLi3(2)(3,5,6,7)
    - GrQLi3(3)(1,2,4,6)
    + GrQLi3(3)(1,2,5,7)
    - GrQLi3(3)(1,4,5,6)
    - GrQLi3(3)(1,4,6,7)
    + GrQLi3(3)(2,4,5,7)
    + GrQLi3(3)(2,5,6,7)
    + GrQLi3(4)(1,2,3,6)
    - GrQLi3(4)(1,2,5,7)
    + GrQLi3(4)(1,3,5,6)
    + GrQLi3(4)(1,3,6,7)
    - GrQLi3(4)(2,3,5,7)
    - GrQLi3(4)(2,5,6,7)
    - GrQLi3(5)(1,2,3,6)
    + GrQLi3(5)(1,2,4,7)
    - GrQLi3(5)(1,3,4,6)
    - GrQLi3(5)(1,3,6,7)
    + GrQLi3(5)(2,3,4,7)
    + GrQLi3(5)(2,4,6,7)
    + GrQLi3(6)(1,2,3,5)
    - GrQLi3(6)(1,2,4,7)
    + GrQLi3(6)(1,3,4,5)
    + GrQLi3(6)(1,3,5,7)
    - GrQLi3(6)(2,3,4,7)
    - GrQLi3(6)(2,4,5,7)
    - GrQLi3(7)(1,2,3,5)
    + GrQLi3(7)(1,2,4,6)
    - GrQLi3(7)(1,3,4,5)
    - GrQLi3(7)(1,3,5,6)
    + GrQLi3(7)(2,3,4,6)
    + GrQLi3(7)(2,4,5,6)
  );
}

TEST(CGrLiTest, LARGE_SymmCGrLi4EquationStub) {
  auto expr =
    + SymmCGrLi4_wip({1,2,3,4,5,6})
    + SymmCGrLi4_wip({2,3,4,5,6,7})
    + SymmCGrLi4_wip({3,4,5,6,7,1})
    + SymmCGrLi4_wip({4,5,6,7,1,2})
    + SymmCGrLi4_wip({5,6,7,1,2,3})
    + SymmCGrLi4_wip({6,7,1,2,3,4})
    + SymmCGrLi4_wip({7,1,2,3,4,5})
  ;
  expr +=
    -4*GrQLi4(1)(2,3,4,5,6,7)
    -4*GrQLi4(2)(3,4,5,6,7,1)
    -4*GrQLi4(3)(4,5,6,7,1,2)
    -4*GrQLi4(4)(5,6,7,1,2,3)
    -4*GrQLi4(5)(6,7,1,2,3,4)
    -4*GrQLi4(6)(7,1,2,3,4,5)
    -4*GrQLi4(7)(1,2,3,4,5,6)
  ;
  EXPECT_POLYLOG_SPACE_CONTAINS(
    GrL_core(4, 3, {1,2,3,4,5,6,7}, false, 0),
    {expr},
    DISAMBIGUATE(to_lyndon_basis)
  );
}
