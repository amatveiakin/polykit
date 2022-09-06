#include "lib/polylog_gli.h"

#include "gtest/gtest.h"

#include "lib/chern_arrow.h"
#include "lib/itertools.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_gr_space.h"
#include "lib/polylog_qli.h"
#include "lib/zip.h"
#include "test_util/matchers.h"
#include "test_util/space_matchers.h"


// TODO: Deduplicate helper functions against polylog_gli.cpp
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

// Note. Synced with casimir_components in GLi definition.
// TODO: Factor out, sync with CasimirDim3 and with cross_product.
GammaExpr Casimir(const std::vector<int>& points) {
  CHECK(points.size() % 2 == 0);
  const int p = div_int(points.size(), 2);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return (
    + G(args(seq_incl(1, p)))
    + G(args(seq_incl(p+1, 2*p)))
    - G(args(concat({2*p}, seq_incl(1, p-1))))
    - G(args(seq_incl(p, 2*p-1)))
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

GammaExpr GLi_Dim3_alternative(int weight, const std::vector<int>& points) {
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
    ret += tensor_product(CasimirDim3(points), GLi_Dim3_alternative(weight - 1, points));
  }
  return ret.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("GLiVec"), {weight}),
      points
    )
  );
}

GammaExpr GLi_Dim4_Weight3_alternative(const std::vector<int>& points) {
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

GammaExpr GLi_Dim4_Weight4_alternative(const std::vector<int>& points) {
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


TEST(GLiTest, CustomPointOrder) {
  // Should be true for any weight and dimension.
  for (const int weight : range_incl(2, 3)) {
    const std::vector points = {2,1,3,6,5,4};
    EXPECT_EXPR_EQ(
      GLiVec(weight, points),
      substitute_variables_1_based(GLiVec(weight, {1,2,3,4,5,6}), points)
    );
  }
}

TEST(GLiTest, IsTotallyWeaklySeparated) {
  // Should be true for any weight and dimension.
  for (const int dimension : range_incl(2, 4)) {
    for (const int weight : range_incl(3, 4)) {
      const auto points = seq_incl(1, 2 * dimension);
      EXPECT_TRUE(is_totally_weakly_separated(GLiVec(weight, points)));
    }
  }
}

TEST(GLiTest, GLiOfFourPointsIsQLi) {
  for (const int weight : range_incl(2, 5)) {
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      GLiVec(weight, {1,2,3,4}),
      delta_expr_to_gamma_expr(QLiVec(weight, {1,2,3,4}))
    );
  }
}

TEST(GLiTest, EqualsAlternative_Dim3) {
  for (const int weight : range_incl(2, 5)) {
    // TODO: Sync indices
    EXPECT_EXPR_EQ(GLiVec(weight, {1,6,2,3,4,5}), GLi_Dim3_alternative(weight, {1,2,3,4,5,6}));
  }
}

TEST(GLiTest, EqualsAlternative_Dim4_Weight3) {
  EXPECT_EXPR_EQ(GLi3(1,2,3,4,5,6,7,8), GLi_Dim4_Weight3_alternative({1,2,3,4,5,6,7,8}));
}

TEST(GLiTest, EqualsAlternative_Dim4_Weight4) {
  EXPECT_EXPR_EQ(GLi4(1,2,3,4,5,6,7,8), GLi_Dim4_Weight4_alternative({1,2,3,4,5,6,7,8}));
}

TEST(GLiTest, SymmGLiIsSymmetric) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + SymmGLi3({1,2,3,4,5,6})
    + SymmGLi3({2,3,4,5,6,1})
  );
}

TEST(GLiTest, LARGE_AomotoPolylogProperties) {
  // First: any permutation of (1..n) and any permutation of (n+1..2n) multiplies the function
  // by permutation sign.
  // For (1,2,3) and (5,6,7) it's trivial from definition, so checking the rest:
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi3(1,2,3,4,5,6,7,8)
    + GLi3(1,2,4,3,5,6,7,8)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi3(1,2,3,4,5,6,7,8)
    + GLi3(1,2,3,4,5,6,8,7)
  );
  // Second: swap (1..n) and (n+1..2n).
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi3(1,2,3,4,5,6,7,8)
    - GLi3(5,6,7,8,1,2,3,4)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi4(1,2,3,4,5,6,7,8,9,10)
    + GLi4(6,7,8,9,10,1,2,3,4,5)
  );
  // Third:
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi3(1,2,3,4,6,7,8,9)
    - GLi3(1,2,3,5,6,7,8,9)
    + GLi3(1,2,4,5,6,7,8,9)
    - GLi3(1,3,4,5,6,7,8,9)
    + GLi3(2,3,4,5,6,7,8,9)
  );
}

// Part (3) of Theorem 1.5. in https://arxiv.org/pdf/2208.01564v1.pdf
TEST(GLiTest, LARGE_GLiViaLowerDim) {
  // True for any p
  for (const int p : range_incl(3, 4)) {
    const auto lhs = GLiVec(p-1, seq_incl(1, 2*p));
    GammaExpr rhs;
    for (const int i : range_incl(1, p)) {
      for (const int j : range_incl(p+1, 2*p)) {
        const auto points = removed_indices_one_based(seq_incl(1, 2*p), {i, j});
        rhs += neg_one_pow(i+j) * GLiVec(p-1, {j}, points);
      }
    }
    rhs *= neg_one_pow(p - 1);
    std::cout << to_lyndon_basis(lhs - rhs);
  }
}

// Part (4) of Theorem 1.5. in https://arxiv.org/pdf/2208.01564v1.pdf
TEST(GLiTest, LARGE_GLiSumInKernelA) {
  // True for any p
  for (const int p : range_incl(3, 4)) {
    const auto lhs = GLiVec(p-1, seq_incl(1, 2*p));
    GammaExpr rhs;
    for (const int i : range(1, p)) {
      for (const int j : range_incl(p+1, 2*p)) {
        const auto points = removed_indices_one_based(seq_incl(1, 2*p), {i, j});
        rhs += neg_one_pow(i+j) * GLiVec(p-1, {i}, points);
      }
    }
    const auto expr = lhs + neg_one_pow(p - 1) * rhs;
    std::cout << to_lyndon_basis(a_full(expr, 2*p+1));
  }
}

TEST(GLiTest, LARGE_ABEquations) {
  for (const auto p : range_incl(3, 4)) {
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      GLiVec(p, seq_incl(1, 2*p+2)),
      neg_one_pow(p-1) * a_plus(b_minus(GLiVec(p, seq_incl(1, 2*p)), 2*p+1), 2*p+2)
    );
    EXPECT_EXPR_ZERO_AFTER_LYNDON(
      a_full(
        + GLiVec(p, seq_incl(1, 2*p))
        + neg_one_pow(p-1) * a_plus(b_minus_minus(GLiVec(p, seq_incl(1, 2*p-2)), 2*p-1), 2*p),
        2*p+1
      )
    );
    EXPECT_EXPR_ZERO_AFTER_LYNDON(
      b_full(
        + GLiVec(p, seq_incl(1, 2*p))
        + neg_one_pow(p) * b_plus(a_minus_minus(GLiVec(p, seq_incl(1, 2*p-2)), 2*p-1), 2*p),
        2*p+1
      )
    );
  }
}

TEST(GLiTest, LARGE_Comultiplication) {
  for (const int weight : range_incl(2, 4)) {
    for (const int p : range_incl(2, 5)) {
      const int num_points = p * 2;
      if (!are_GLi_args_ok(weight, num_points)) {
        continue;
      }
      const auto points = seq_incl(1, num_points);
      const auto lhs = ncomultiply(GLiVec(weight, points));
      GammaNCoExpr rhs;
      if (weight >= p) {
        rhs -= ncoproduct(GLiVec(weight - 1, points), Casimir(points));
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
              if (are_GLi_args_ok(w_1, args_1.size()) && are_GLi_args_ok(w_2, args_2.size())) {
                rhs += sign * ncoproduct(
                  GLiVec(w_1, i, args_1),
                  GLiVec(w_2, j, args_2)
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

// Simplified formula for (1, n-1) comultiplication component of Aomoto polylogarithm:
// Proposition 2.3 from https://arxiv.org/pdf/math/0011168.pdf
TEST(GLiTest, LARGE_ComultiplicationAomoto) {
  for (const int weight : range_incl(3, 4)) {
    const int p = weight + 1;
    const int num_points = 2 * p;
    const auto points = seq_incl(1, num_points);
    const auto lhs = ncomultiply(GLiVec(weight, points), {1, weight - 1});
    GammaNCoExpr rhs;
    for (const int i : range(p)) {
      for (const int j : range(p, 2 * p)) {
        const int sign = neg_one_pow(points[i] + points[j]);
        rhs += sign * (
          + ncoproduct(
            GLiVec(weight - 1, {points[j]}, removed_indices(points, {i, j})),
            plucker(concat({points[j]}, removed_index(slice(points, 0, p), i)))
          )
          - ncoproduct(
            GLiVec(weight - 1, {points[i]}, removed_indices(points, {i, j})),
            plucker(concat({points[i]}, removed_index(slice(points, p), j - p)))
          )
        );
      }
    }
    EXPECT_EXPR_EQ_AFTER_LYNDON(lhs, -neg_one_pow(weight) * rhs);
  }
}

TEST(GLiTest, LARGE_ABOfAomotoIsZero) {
  for (const int weight : range_incl(3, 4)) {
    const int p = weight + 1;
    const auto expr = GLiVec(weight, seq_incl(1, 2*p));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(a_minus(expr, 2*p+1));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(a_plus(expr, 2*p+1));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(b_minus(expr, 2*p+1));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(b_plus(expr, 2*p+1));
  }
}

TEST(GLiTest, LARGE_ComultiplicationAomotoViaAB) {
  for (const int weight : range_incl(3, 4)) {
    const int p = weight + 1;
    const auto lhs = ncomultiply(GLiVec(weight, seq_incl(1, 2*p)), {1, p-2});
    const auto rhs =
      + a_minus(b_plus(ncoproduct(
        GLiVec(weight-1, seq_incl(1, 2*p-2)),
        plucker({seq_incl(1, p-1)})
      ), 2*p-1), 2*p)
      + a_plus(b_minus(ncoproduct(
        GLiVec(weight-1, seq_incl(1, 2*p-2)),
        plucker({seq_incl(p, 2*p-2)})
      ), 2*p-1), 2*p)
    ;
    EXPECT_EXPR_EQ_AFTER_LYNDON(lhs, -neg_one_pow(p) * rhs);
  }
}

// Presentation for (1, n-1) comultiplication component:
// Proposition 4.4 from https://arxiv.org/pdf/math/0011168.pdf
TEST(GLiTest, LARGE_ComultiplicationViaAB) {
  for (const int n : range_incl(4, 5)) {
    for (const int p : range_incl(3, 4)) {
      const auto gli_large = GLiVec(n-1, seq_incl(1, 2*p));
      const auto gli_small = GLiVec(n-1, seq_incl(1, 2*p-2));
      const auto s = neg_one_pow(p-1);
      const auto lhs = s * ncomultiply(GLiVec(n, seq_incl(1, 2*p)), {1,n-1});
      const auto rhs =
        + s * ncoproduct(gli_large, plucker(seq_incl(1, p)))
        + s * ncoproduct(gli_large, plucker(seq_incl(p+1, 2*p)))
        - ncoproduct(
          s * gli_large + a_minus(b_plus(gli_small, 2*p-1), 2*p),
          plucker(concat(seq_incl(1, p-1), {2*p}))
        )
        - ncoproduct(
          s * gli_large + a_plus(b_minus(gli_small, 2*p-1), 2*p),
          plucker(seq_incl(p, 2*p-1))
        )
        + a_minus(ncoproduct(
          b_plus(gli_small, 2*p-1),
          plucker(concat(seq_incl(1, p-1), {2*p-1}))
        ), 2*p)
        + a_plus(ncoproduct(
          b_minus(gli_small, 2*p-1),
          plucker(seq_incl(p, 2*p-1))
        ), 2*p)
        - b_plus(ncoproduct(
          a_minus(gli_small, 2*p-1),
          plucker(seq_incl(1, p-1))
        ), 2*p)
        - b_minus(ncoproduct(
          a_plus(gli_small, 2*p-1),
          plucker(seq_incl(p, 2*p-2))
        ), 2*p)
        - a_minus(b_plus(ncoproduct(
          gli_small,
          plucker(seq_incl(1, p-1))
        ), 2*p-1), 2*p)
        - a_plus(b_minus(ncoproduct(
          gli_small,
          plucker(seq_incl(p, 2*p-2))
        ), 2*p-1), 2*p)
      ;
      EXPECT_EXPR_EQ(lhs, -rhs);  // TODO: Fix the formula (here and in the article) and remove minus
    }
  }
}

TEST(GLiTest, LARGE_GLi4Of8PointsAreIndependent) {
  const int weight = 4;
  const auto points = to_vector(range_incl(1, 2 * (weight - 1)));
  Gr_Space space;
  for (const auto& args : permutations(points)) {
    if (is_canonical_GLi_arg_order(args)) {
      space.push_back(GLiVec(weight, args));
    }
  }
  const auto rank = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
  EXPECT_EQ(rank, space.size());
}

TEST(GLiTest, LARGE_GLi4ShiftedSumViaLowerDim) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi4(1,2,3,4,5,6,7,8)
    + GLi4(2,3,4,5,6,7,8,1)
    - GLi4[{1}](2,3,5,6,7,8)
    + GLi4[{1}](2,4,5,6,7,8)
    - GLi4[{1}](3,4,5,6,7,8)
    + GLi4[{2}](1,3,5,6,7,8)
    - GLi4[{2}](1,4,5,6,7,8)
    - GLi4[{2}](3,4,5,6,7,1)
    + GLi4[{2}](3,4,5,6,7,8)
    + GLi4[{2}](3,4,5,6,8,1)
    - GLi4[{2}](3,4,5,7,8,1)
    - GLi4[{3}](1,2,5,6,7,8)
    + GLi4[{3}](1,4,5,6,7,8)
    + GLi4[{3}](2,4,5,6,7,1)
    - GLi4[{3}](2,4,5,6,7,8)
    - GLi4[{3}](2,4,5,6,8,1)
    + GLi4[{3}](2,4,5,7,8,1)
    + GLi4[{4}](1,2,5,6,7,8)
    - GLi4[{4}](1,3,5,6,7,8)
    - GLi4[{4}](2,3,5,6,7,1)
    + GLi4[{4}](2,3,5,6,7,8)
    + GLi4[{4}](2,3,5,6,8,1)
    - GLi4[{4}](2,3,5,7,8,1)
    + GLi4[{5}](2,3,4,6,7,1)
    - GLi4[{5}](2,3,4,6,8,1)
    + GLi4[{5}](2,3,4,7,8,1)
    - GLi4[{6}](2,3,4,5,7,1)
    + GLi4[{6}](2,3,4,5,8,1)
    - GLi4[{6}](2,3,4,7,8,1)
    + GLi4[{6}](2,3,5,7,8,1)
    - GLi4[{6}](2,4,5,7,8,1)
    + GLi4[{6}](3,4,5,7,8,1)
    + GLi4[{7}](2,3,4,5,6,1)
    - GLi4[{7}](2,3,4,5,8,1)
    + GLi4[{7}](2,3,4,6,8,1)
    - GLi4[{7}](2,3,5,6,8,1)
    + GLi4[{7}](2,4,5,6,8,1)
    - GLi4[{7}](3,4,5,6,8,1)
    - GLi4[{8}](2,3,4,5,6,1)
    + GLi4[{8}](2,3,4,5,7,1)
    - GLi4[{8}](2,3,4,6,7,1)
    + GLi4[{8}](2,3,5,6,7,1)
    - GLi4[{8}](2,4,5,6,7,1)
    + GLi4[{8}](3,4,5,6,7,1)
    - GLi4[{2,6}](1,3,5,7)
    + GLi4[{2,6}](1,3,5,8)
    + GLi4[{2,6}](1,4,5,7)
    - GLi4[{2,6}](1,4,5,8)
    + GLi4[{2,7}](1,3,5,6)
    - GLi4[{2,7}](1,3,5,8)
    - GLi4[{2,7}](1,4,5,6)
    + GLi4[{2,7}](1,4,5,8)
    - GLi4[{2,8}](1,3,5,6)
    + GLi4[{2,8}](1,3,5,7)
    + GLi4[{2,8}](1,4,5,6)
    - GLi4[{2,8}](1,4,5,7)
    + GLi4[{3,6}](1,2,5,7)
    - GLi4[{3,6}](1,2,5,8)
    - GLi4[{3,6}](1,4,5,7)
    + GLi4[{3,6}](1,4,5,8)
    - GLi4[{3,7}](1,2,5,6)
    + GLi4[{3,7}](1,2,5,8)
    + GLi4[{3,7}](1,4,5,6)
    - GLi4[{3,7}](1,4,5,8)
    + GLi4[{3,8}](1,2,5,6)
    - GLi4[{3,8}](1,2,5,7)
    - GLi4[{3,8}](1,4,5,6)
    + GLi4[{3,8}](1,4,5,7)
    - GLi4[{4,6}](1,2,5,7)
    + GLi4[{4,6}](1,2,5,8)
    + GLi4[{4,6}](1,3,5,7)
    - GLi4[{4,6}](1,3,5,8)
    + GLi4[{4,7}](1,2,5,6)
    - GLi4[{4,7}](1,2,5,8)
    - GLi4[{4,7}](1,3,5,6)
    + GLi4[{4,7}](1,3,5,8)
    - GLi4[{4,8}](1,2,5,6)
    + GLi4[{4,8}](1,2,5,7)
    + GLi4[{4,8}](1,3,5,6)
    - GLi4[{4,8}](1,3,5,7)
  );
}

TEST(GLiTest, GLi3Equation) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + GLi3(1,2,3,4,5,6)
    - GLi3(1,2,3,4,5,7)
    + GLi3(1,2,3,4,6,7)
    - GLi3(1,2,3,5,6,7)
    + GLi3(1,2,4,5,6,7)
    - GLi3(1,3,4,5,6,7)
    + GLi3(2,3,4,5,6,7)
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

TEST(GLiTest, SymmGLi3Equation) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + SymmGLi3({1,2,3,4,5,6})
    - SymmGLi3({1,2,3,4,5,7})
    + SymmGLi3({1,2,3,4,6,7})
    - SymmGLi3({1,2,3,5,6,7})
    + SymmGLi3({1,2,4,5,6,7})
    - SymmGLi3({1,3,4,5,6,7})
    + SymmGLi3({2,3,4,5,6,7})
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

TEST(GLiTest, LARGE_SymmGLi4EquationStub) {
  auto expr =
    + SymmGLi4_wip({1,2,3,4,5,6})
    + SymmGLi4_wip({2,3,4,5,6,7})
    + SymmGLi4_wip({3,4,5,6,7,1})
    + SymmGLi4_wip({4,5,6,7,1,2})
    + SymmGLi4_wip({5,6,7,1,2,3})
    + SymmGLi4_wip({6,7,1,2,3,4})
    + SymmGLi4_wip({7,1,2,3,4,5})
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

TEST(GLiTest, SumOfPermutations_Arg4) {
  GammaExpr p, q;
  for (const auto& [points, sign] : permutations_with_sign({1,2,3,4})) {
    p += sign * tensor_product(absl::MakeConstSpan({
      G(choose_indices_one_based(points, {1,2})),
      G(choose_indices_one_based(points, {2,3})),
    }));
    q += sign * GLiVec(2, points);
  }
  p = to_lyndon_basis(p);
  q = to_lyndon_basis(q);
  EXPECT_EXPR_ZERO(p.dived_int(2) + q.dived_int(24));
}

TEST(GLiTest, LARGE_SumOfPermutations_Arg6) {
  GammaExpr p, q;
  for (const auto& [points, sign] : permutations_with_sign({1,2,3,4,5,6})) {
    p += sign * tensor_product(absl::MakeConstSpan({
      G(choose_indices_one_based(points, {1,2,3})),
      G(choose_indices_one_based(points, {2,3,4})),
      G(choose_indices_one_based(points, {3,4,5})),
    }));
    q += sign * GLiVec(3, points);
  }
  p = to_lyndon_basis(p);
  q = to_lyndon_basis(q);
  EXPECT_EXPR_ZERO(p.dived_int(2) - q.dived_int(240));
}
