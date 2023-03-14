// TODO: Test that the following is always true:
//   comultiply(cocycle(w, d, n)) == +/- left(cocycle(w, d, n-1)) +/- up(cocycle(w, d-1, n-1))

#include "lib/chern_cocycle.h"

#include <random>

#include "gtest/gtest.h"

#include "lib/chern_arrow.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_gr_space.h"
#include "test_util/matchers.h"
#include "test_util/space_matchers.h"


GammaNCoExpr ChernCocycle_3_2_5(const std::vector<int>& points) {
  const int weight = 3;
  CHECK_EQ(points.size(), 5);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return
    + ncoproduct(GLiVec(weight-1, args({1,2,3,5})), G(args({3,4})))
    - ncoproduct(GLiVec(weight-1, args({1,2,4,5})), G(args({3,4})))
    - ncoproduct(GLiVec(weight-1, args({1,2,3,5})), G(args({3,5})))
    + ncoproduct(GLiVec(weight-1, args({1,2,4,5})), G(args({4,5})))
    + ncoproduct(GLiVec(weight-1, args({1,3,4,5})), G(args({1,2})))
    - ncoproduct(GLiVec(weight-1, args({2,3,4,5})), G(args({1,2})))
    - ncoproduct(GLiVec(weight-1, args({1,3,4,5})), G(args({1,5})))
    + ncoproduct(GLiVec(weight-1, args({2,3,4,5})), G(args({2,5})))
  ;
}

GammaExpr ChernCocycle_3_2_6(const std::vector<int>& points) {
  const int weight = 3;
  CHECK_EQ(points.size(), 6);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return
    - GLiVec(weight, args({2,3,5,6}))
    + GLiVec(weight, args({2,3,4,6}))
    - GLiVec(weight, args({2,3,4,5}))
    + GLiVec(weight, args({1,3,5,6}))
    - GLiVec(weight, args({1,3,4,6}))
    + GLiVec(weight, args({1,3,4,5}))
  ;
}

GammaExpr ChernCocycle_3_3_6(const std::vector<int>& points) {
  const int weight = 3;
  CHECK_EQ(points.size(), 6);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return plucker_dual(
    + GLiVec(weight, args({1,2,3,4,5,6}))
    - pullback(GLiVec(weight, args({2,3,5,6})), args({1}))
    + pullback(GLiVec(weight, args({2,3,4,6})), args({1}))
    - pullback(GLiVec(weight, args({2,3,4,5})), args({1}))
    + pullback(GLiVec(weight, args({1,3,5,6})), args({2}))
    - pullback(GLiVec(weight, args({1,3,4,6})), args({2}))
    + pullback(GLiVec(weight, args({1,3,4,5})), args({2}))
    ,
    points
  );
}

GammaExpr ChernCocycle_4_3_8(const std::vector<int>& points) {
  const int weight = 4;
  CHECK_EQ(points.size(), 8);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return
    + GLiVec(weight, args({2,3,4,6,7,8}))
    - GLiVec(weight, args({2,3,4,5,7,8}))
    + GLiVec(weight, args({2,3,4,5,6,8}))
    - GLiVec(weight, args({2,3,4,5,6,7}))
    - GLiVec(weight, args({1,3,4,6,7,8}))
    + GLiVec(weight, args({1,3,4,5,7,8}))
    - GLiVec(weight, args({1,3,4,5,6,8}))
    + GLiVec(weight, args({1,3,4,5,6,7}))
    + GLiVec(weight, args({1,2,4,6,7,8}))
    - GLiVec(weight, args({1,2,4,5,7,8}))
    + GLiVec(weight, args({1,2,4,5,6,8}))
    - GLiVec(weight, args({1,2,4,5,6,7}))
  ;
}

GammaExpr ChernCocycle_4_4_8(const std::vector<int>& points) {
  const int weight = 4;
  CHECK_EQ(points.size(), 8);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return plucker_dual(
    + GLiVec(weight, args({1,2,3,4,5,6,7,8}))
    - pullback(GLiVec(weight, args({2,3,4,6,7,8})), args({1}))
    + pullback(GLiVec(weight, args({2,3,4,5,7,8})), args({1}))
    - pullback(GLiVec(weight, args({2,3,4,5,6,8})), args({1}))
    + pullback(GLiVec(weight, args({2,3,4,5,6,7})), args({1}))
    + pullback(GLiVec(weight, args({1,3,4,6,7,8})), args({2}))
    - pullback(GLiVec(weight, args({1,3,4,5,7,8})), args({2}))
    + pullback(GLiVec(weight, args({1,3,4,5,6,8})), args({2}))
    - pullback(GLiVec(weight, args({1,3,4,5,6,7})), args({2}))
    - pullback(GLiVec(weight, args({1,2,4,6,7,8})), args({3}))
    + pullback(GLiVec(weight, args({1,2,4,5,7,8})), args({3}))
    - pullback(GLiVec(weight, args({1,2,4,5,6,8})), args({3}))
    + pullback(GLiVec(weight, args({1,2,4,5,6,7})), args({3}))
    ,
    points
  );
}

GammaExpr ChernCocycle_n_n_2n(const std::vector<int>& points) {
  CHECK(points.size() % 2 == 0);
  const int weight = points.size() / 2;
  const int mid = points.size() / 2 - 1;
  GammaExpr ret;
  ret += GLiVec(weight, points);
  for (const int before : range(mid)) {
    for (const int after : range(mid + 1, points.size())) {
      const int sign = neg_one_pow(before + after + weight + 1);
      ret += sign * GLiVec(
        weight,
        choose_indices(points, {before}),
        removed_indices(points, {before, after})
      );
    }
  }
  return plucker_dual(ret, points);
}

GammaExpr ChernCocycle_n_nminus1_2n(const std::vector<int>& points) {
  CHECK(points.size() % 2 == 0);
  const int weight = points.size() / 2;
  const int mid = points.size() / 2 - 1;
  GammaExpr ret;
  for (const int before : range(mid)) {
    for (const int after : range(mid + 1, points.size())) {
      const int sign = neg_one_pow(before + after + weight + 1);
      ret += sign * GLiVec(weight, removed_indices(points, {before, after}));
    }
  }
  return neg_one_pow(weight + 1) * ret;
}


TEST(ChernCocycleTest, EqualsExplicit_Weight3) {
  std::vector points5 = {3,4,5,1,2};
  std::vector points6 = {3,4,5,6,1,2};
  EXPECT_EXPR_EQ(ChernCocycle_3_2_5(points5), ChernCocycle(3, 2, points5));
  EXPECT_EXPR_EQ(-ncoproduct(ChernCocycle_3_2_6(points6)), ChernCocycle(3, 2, points6));
  EXPECT_EXPR_EQ(-ncoproduct(ChernCocycle_3_3_6(points6)), ChernCocycle(3, 3, points6));
}

TEST(ChernCocycleTest, LARGE_EqualsExplicit_Weight4) {
  std::vector points8 = {3,4,5,6,7,8,1,2};
  EXPECT_EXPR_EQ(ncoproduct(ChernCocycle_4_3_8(points8)), ChernCocycle(4, 3, points8));
  EXPECT_EXPR_EQ(ncoproduct(ChernCocycle_4_4_8(points8)), ChernCocycle(4, 4, points8));
}

TEST(ChernCocycleTest, LARGE_EqualsOld) {
  std::minstd_rand rnd;
  for (const int n : range_incl(3, 4)) {
    auto points = seq_incl(1, 2*n);
    absl::c_shuffle(points, rnd);
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      ChernCocycle(n, n, points),
      neg_one_pow(n) * ncoproduct(ChernCocycle_n_n_2n(points))
    );
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      ChernCocycle(n, n-1, points),
      neg_one_pow(n) * ncoproduct(ChernCocycle_n_nminus1_2n(points))
    );
  }
}

TEST(ChernCocycleTest, ArrowEquations_Weight3) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_up(ChernCocycle(3, 3, {1,2,3,4,5,6}), 7));
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_left(ChernCocycle(3, 2, {1,2,3,4,5,6}), 7));
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + ncomultiply(ChernCocycle(3, 2, {1,2,3,4,5}))
    - chern_arrow_left(ChernCocycle(3, 1, {1,2,3,4}), 5)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + ncomultiply(ChernCocycle(3, 2, {1,2,3,4,5,6}))
    + chern_arrow_left(ChernCocycle(3, 2, {1,2,3,4,5}), 6)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + ncomultiply(ChernCocycle(3, 3, {1,2,3,4,5,6}))
    + chern_arrow_up(ChernCocycle(3, 2, {1,2,3,4,5}), 6)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + chern_arrow_left(ChernCocycle(3, 3, {1,2,3,4,5,6}), 7)
    + chern_arrow_up(ChernCocycle(3, 2, {1,2,3,4,5,6}), 7)
  );
}

TEST(ChernCocycleTest, LARGE_ArrowEquations_Weight4) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_up(ChernCocycle(4, 4, {1,2,3,4,5,6,7,8}), 9));
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_left(ChernCocycle(4, 3, {1,2,3,4,5,6,7,8}), 9));
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + chern_arrow_left(ChernCocycle(4, 4, {1,2,3,4,5,6,7,8}), 9)
    + chern_arrow_up(ChernCocycle(4, 3, {1,2,3,4,5,6,7,8}), 9)
  );
}

TEST(ChernCocycleTest, LARGE_ABEquations) {
  for (const int n : range_incl(3, 4)) {
    const auto c_n = ChernCocycle(n, n, seq_incl(1, 2*n));
    const auto c_n1 = ChernCocycle(n, n-1, seq_incl(1, 2*n));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(a_full(c_n1, 2*n+1));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(b_full(c_n1, 2*n+1) + a_full(c_n, 2*n+1));
    EXPECT_EXPR_ZERO_AFTER_LYNDON(b_full(c_n, 2*n+1));
  }
}

TEST(ChernCocycleTest, LARGE_ImageLiesInChernGrLImage_Weight3) {
  const auto expr = chern_arrow_left(ChernCocycle(3, 3, {1,2,3,4,5,6}), 7);
  const auto space = mapped(ChernGrL(3, 2, {1,2,3,4,5,6}), [](const auto& expr) {
    return ncoproduct(chern_arrow_up(expr, 7));
  });
  EXPECT_FALSE(to_lyndon_basis(expr).is_zero());
  EXPECT_POLYLOG_SPACE_CONTAINS(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
}

TEST(ChernCocycleTest, LARGE_Comultiplication_3_1_Dim4) {
  EXPECT_EXPR_ZERO(
    + ncomultiply(ChernCocycle(4, 4, {1,2,3,4,5,6,7,8}), {1,3})
    + chern_arrow_up(
      - ncoproduct(GLi3(1,2,3,4,5,6), plucker({1,2,3}))
      - ncoproduct(GLi3(1,2,3,4,5,6), plucker({4,5,6}))
      + ncoproduct(GLi3(1,2,3,4,5,7), plucker({1,2,3}))
      + ncoproduct(GLi3(1,2,3,4,5,7), plucker({4,5,7}))
      - ncoproduct(GLi3(1,2,3,4,6,7), plucker({1,2,3}))
      - ncoproduct(GLi3(1,2,3,4,6,7), plucker({4,6,7}))
      + ncoproduct(GLi3(1,2,3,5,6,7), plucker({1,2,3}))
      + ncoproduct(GLi3(1,2,3,5,6,7), plucker({5,6,7}))
      - ncoproduct(GLi3(1,2,4,5,6,7), plucker({4,5,6}))
      - ncoproduct(GLi3(1,2,4,5,6,7), plucker({7,1,2}))
      + ncoproduct(GLi3(1,3,4,5,6,7), plucker({4,5,6}))
      + ncoproduct(GLi3(1,3,4,5,6,7), plucker({7,1,3}))
      - ncoproduct(GLi3(2,3,4,5,6,7), plucker({4,5,6}))
      - ncoproduct(GLi3(2,3,4,5,6,7), plucker({7,2,3}))
      + ncoproduct(plucker({1,2,3}), GLi3[{2}](1,4,5,6))
      - ncoproduct(plucker({1,2,3}), GLi3[{3}](1,4,5,6))
      + ncoproduct(plucker({1,2,3}), GLi3[{2}](1,4,6,7))
      - ncoproduct(plucker({1,2,3}), GLi3[{3}](1,4,6,7))
      - ncoproduct(plucker({1,2,3}), GLi3[{2}](1,5,6,7))
      + ncoproduct(plucker({1,2,3}), GLi3[{3}](1,5,6,7))
      - ncoproduct(plucker({1,2,3}), GLi3[{1}](2,4,5,6))
      + ncoproduct(plucker({1,2,3}), GLi3[{3}](2,4,5,6))
      - ncoproduct(plucker({1,2,3}), GLi3[{3}](2,4,5,7))
      - ncoproduct(plucker({1,2,3}), GLi3[{1}](2,4,6,7))
      + ncoproduct(plucker({1,2,3}), GLi3[{3}](2,4,6,7))
      + ncoproduct(plucker({1,2,3}), GLi3[{1}](2,5,6,7))
      + ncoproduct(plucker({1,2,3}), GLi3[{1}](3,4,5,6))
      - ncoproduct(plucker({1,2,3}), GLi3[{2}](3,4,5,6))
      - ncoproduct(plucker({1,2,3}), GLi3[{1}](3,4,5,7))
      + ncoproduct(plucker({1,2,3}), GLi3[{2}](3,4,5,7))
      + ncoproduct(plucker({1,2,3}), GLi3[{1}](3,4,6,7))
      - ncoproduct(plucker({1,2,3}), GLi3[{2}](3,4,6,7))
      + ncoproduct(plucker({1,2,4}), GLi3[{4}](1,3,5,7))
      + ncoproduct(plucker({1,2,4}), GLi3[{2}](1,5,6,7))
      - ncoproduct(plucker({1,2,4}), GLi3[{4}](1,5,6,7))
      - ncoproduct(plucker({1,2,4}), GLi3[{1}](2,5,6,7))
      - ncoproduct(plucker({1,2,5}), GLi3[{5}](1,3,4,7))
      + ncoproduct(plucker({1,2,5}), GLi3[{5}](1,4,6,7))
      + ncoproduct(plucker({1,2,6}), GLi3[{6}](1,3,4,7))
      - ncoproduct(plucker({1,2,6}), GLi3[{6}](1,4,5,7))
      - ncoproduct(plucker({1,2,7}), GLi3[{7}](1,3,4,6))
      + ncoproduct(plucker({1,2,7}), GLi3[{6}](1,4,5,7))
      - ncoproduct(plucker({1,2,7}), GLi3[{5}](1,4,6,7))
      + ncoproduct(plucker({1,2,7}), GLi3[{7}](2,4,5,6))
      - ncoproduct(plucker({1,2,7}), GLi3[{6}](2,4,5,7))
      + ncoproduct(plucker({1,2,7}), GLi3[{5}](2,4,6,7))
      - ncoproduct(plucker({1,3,4}), GLi3[{3}](1,5,6,7))
      + ncoproduct(plucker({1,3,4}), GLi3[{4}](1,5,6,7))
      - ncoproduct(plucker({1,3,5}), GLi3[{5}](1,4,6,7))
      + ncoproduct(plucker({1,3,6}), GLi3[{6}](1,4,5,7))
      - ncoproduct(plucker({1,3,7}), GLi3[{6}](1,4,5,7))
      + ncoproduct(plucker({1,3,7}), GLi3[{5}](1,4,6,7))
      - ncoproduct(plucker({1,3,7}), GLi3[{7}](3,4,5,6))
      + ncoproduct(plucker({1,3,7}), GLi3[{6}](3,4,5,7))
      - ncoproduct(plucker({1,3,7}), GLi3[{5}](3,4,6,7))
      - ncoproduct(plucker({1,5,6}), GLi3[{1}](3,4,6,7))
      - ncoproduct(plucker({1,5,7}), GLi3[{1}](3,4,5,7))
      - ncoproduct(plucker({1,6,7}), GLi3[{1}](2,4,6,7))
      + ncoproduct(plucker({1,6,7}), GLi3[{1}](3,4,6,7))
      - ncoproduct(plucker({2,3,7}), GLi3[{7}](2,4,5,6))
      + ncoproduct(plucker({2,3,7}), GLi3[{6}](2,4,5,7))
      - ncoproduct(plucker({2,3,7}), GLi3[{5}](2,4,6,7))
      + ncoproduct(plucker({2,3,7}), GLi3[{7}](3,4,5,6))
      - ncoproduct(plucker({2,3,7}), GLi3[{6}](3,4,5,7))
      + ncoproduct(plucker({2,3,7}), GLi3[{5}](3,4,6,7))
      + ncoproduct(plucker({2,5,6}), GLi3[{2}](3,4,6,7))
      + ncoproduct(plucker({2,5,7}), GLi3[{2}](3,4,5,7))
      + ncoproduct(plucker({2,6,7}), GLi3[{2}](1,4,6,7))
      - ncoproduct(plucker({2,6,7}), GLi3[{2}](3,4,6,7))
      - ncoproduct(plucker({3,5,6}), GLi3[{3}](2,4,6,7))
      - ncoproduct(plucker({3,5,7}), GLi3[{3}](2,4,5,7))
      - ncoproduct(plucker({3,6,7}), GLi3[{3}](1,4,6,7))
      + ncoproduct(plucker({3,6,7}), GLi3[{3}](2,4,6,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{5}](1,3,4,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{6}](1,3,4,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{4}](1,3,5,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{6}](1,3,5,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{4}](1,3,6,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{5}](1,3,6,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{2}](1,4,5,6))
      - ncoproduct(plucker({4,5,6}), GLi3[{3}](1,4,5,6))
      - ncoproduct(plucker({4,5,6}), GLi3[{2}](1,4,5,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{3}](1,4,5,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{2}](1,4,6,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{3}](1,4,6,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{5}](2,3,4,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{6}](2,3,4,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{4}](2,3,5,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{1}](2,4,5,6))
      + ncoproduct(plucker({4,5,6}), GLi3[{3}](2,4,5,6))
      + ncoproduct(plucker({4,5,6}), GLi3[{1}](2,4,5,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{3}](2,4,5,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{1}](2,4,6,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{3}](2,4,6,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{1}](3,4,5,6))
      - ncoproduct(plucker({4,5,6}), GLi3[{2}](3,4,5,6))
      - ncoproduct(plucker({4,5,6}), GLi3[{1}](3,4,5,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{2}](3,4,5,7))
      + ncoproduct(plucker({4,5,6}), GLi3[{1}](3,4,6,7))
      - ncoproduct(plucker({4,5,6}), GLi3[{2}](3,4,6,7))
      + ncoproduct(plucker({4,5,7}), GLi3[{7}](1,3,4,6))
      - ncoproduct(plucker({4,5,7}), GLi3[{5}](1,3,4,7))
      - ncoproduct(plucker({4,5,7}), GLi3[{7}](1,3,5,6))
      + ncoproduct(plucker({4,5,7}), GLi3[{4}](1,3,5,7))
      - ncoproduct(plucker({4,5,7}), GLi3[{7}](2,3,4,6))
      - ncoproduct(plucker({4,6,7}), GLi3[{7}](1,3,4,6))
      + ncoproduct(plucker({4,6,7}), GLi3[{6}](1,3,4,7))
      , 8
    )
  );
}

TEST(ChernCocycleTest, LARGE_Dual_Comultiplication_3_1_Dim4) {
  // Note: with the old definition (see ChernCocycle_n_n_2n) `plucker_dual` used to
  //   reverse `plucker_dual` in the cocycle definition.
  EXPECT_EXPR_ZERO(
    + ncomultiply(plucker_dual(ChernCocycle(4, 4, {1,2,3,4,5,6,7,8}), {1,2,3,4,5,6,7,8}), {1,3})
    + chern_arrow_left(
      + ncoproduct(plucker({1,2,3,4}), GLi3[{4}](1,2,3,5,6,7))
      - ncoproduct(plucker({1,2,3,5}), GLi3[{5}](1,2,3,4,6,7))
      + ncoproduct(plucker({1,2,3,6}), GLi3[{6}](1,2,3,4,5,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{4}](1,2,3,5,6,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{5}](1,2,3,4,6,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{6}](1,2,3,4,5,7))
      - ncoproduct(plucker({1,4,5,6}), GLi3[{1}](2,3,4,5,6,7))
      + ncoproduct(plucker({2,4,5,6}), GLi3[{2}](1,3,4,5,6,7))
      - ncoproduct(plucker({3,4,5,6}), GLi3[{3}](1,2,4,5,6,7))
      + ncoproduct(plucker({4,5,6,7}), GLi3[{1}](2,3,4,5,6,7))
      - ncoproduct(plucker({4,5,6,7}), GLi3[{2}](1,3,4,5,6,7))
      + ncoproduct(plucker({4,5,6,7}), GLi3[{3}](1,2,4,5,6,7))
      - ncoproduct(plucker({1,2,3,4}), GLi3[{1,4}](2,3,5,6))
      + ncoproduct(plucker({1,2,3,4}), GLi3[{1,4}](2,3,5,7))
      - ncoproduct(plucker({1,2,3,4}), GLi3[{1,4}](2,3,6,7))
      + ncoproduct(plucker({1,2,3,4}), GLi3[{2,4}](1,3,5,6))
      - ncoproduct(plucker({1,2,3,4}), GLi3[{2,4}](1,3,5,7))
      + ncoproduct(plucker({1,2,3,4}), GLi3[{2,4}](1,3,6,7))
      + ncoproduct(plucker({1,2,3,5}), GLi3[{1,5}](2,3,4,6))
      - ncoproduct(plucker({1,2,3,5}), GLi3[{1,5}](2,3,4,7))
      + ncoproduct(plucker({1,2,3,5}), GLi3[{1,5}](2,3,6,7))
      - ncoproduct(plucker({1,2,3,5}), GLi3[{2,5}](1,3,4,6))
      + ncoproduct(plucker({1,2,3,5}), GLi3[{2,5}](1,3,4,7))
      - ncoproduct(plucker({1,2,3,5}), GLi3[{2,5}](1,3,6,7))
      + ncoproduct(plucker({1,2,3,6}), GLi3[{1,4}](2,3,5,6))
      - ncoproduct(plucker({1,2,3,6}), GLi3[{1,5}](2,3,4,6))
      + ncoproduct(plucker({1,2,3,6}), GLi3[{1,6}](2,3,4,7))
      - ncoproduct(plucker({1,2,3,6}), GLi3[{1,6}](2,3,5,7))
      - ncoproduct(plucker({1,2,3,6}), GLi3[{2,4}](1,3,5,6))
      + ncoproduct(plucker({1,2,3,6}), GLi3[{2,5}](1,3,4,6))
      - ncoproduct(plucker({1,2,3,6}), GLi3[{2,6}](1,3,4,7))
      + ncoproduct(plucker({1,2,3,6}), GLi3[{2,6}](1,3,5,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{1,4}](2,3,5,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{1,4}](2,3,6,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{1,5}](2,3,4,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{1,5}](2,3,6,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{1,6}](2,3,4,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{1,6}](2,3,5,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{2,4}](1,3,5,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{2,4}](1,3,6,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{2,5}](1,3,4,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{2,5}](1,3,6,7))
      + ncoproduct(plucker({1,2,3,7}), GLi3[{2,6}](1,3,4,7))
      - ncoproduct(plucker({1,2,3,7}), GLi3[{2,6}](1,3,5,7))
      + ncoproduct(plucker({1,4,5,6}), GLi3[{1,2}](3,4,5,6))
      - ncoproduct(plucker({1,4,5,6}), GLi3[{1,2}](3,4,5,7))
      + ncoproduct(plucker({1,4,5,6}), GLi3[{1,2}](3,4,6,7))
      - ncoproduct(plucker({1,4,5,6}), GLi3[{1,3}](2,4,5,6))
      + ncoproduct(plucker({1,4,5,6}), GLi3[{1,3}](2,4,5,7))
      - ncoproduct(plucker({1,4,5,6}), GLi3[{1,3}](2,4,6,7))
      - ncoproduct(plucker({2,4,5,6}), GLi3[{1,2}](3,4,5,6))
      + ncoproduct(plucker({2,4,5,6}), GLi3[{1,2}](3,4,5,7))
      - ncoproduct(plucker({2,4,5,6}), GLi3[{1,2}](3,4,6,7))
      + ncoproduct(plucker({2,4,5,6}), GLi3[{2,3}](1,4,5,6))
      - ncoproduct(plucker({2,4,5,6}), GLi3[{2,3}](1,4,5,7))
      + ncoproduct(plucker({2,4,5,6}), GLi3[{2,3}](1,4,6,7))
      + ncoproduct(plucker({3,4,5,6}), GLi3[{1,3}](2,4,5,6))
      - ncoproduct(plucker({3,4,5,6}), GLi3[{1,3}](2,4,5,7))
      + ncoproduct(plucker({3,4,5,6}), GLi3[{1,3}](2,4,6,7))
      - ncoproduct(plucker({3,4,5,6}), GLi3[{2,3}](1,4,5,6))
      + ncoproduct(plucker({3,4,5,6}), GLi3[{2,3}](1,4,5,7))
      - ncoproduct(plucker({3,4,5,6}), GLi3[{2,3}](1,4,6,7))
      , 8
    )
  );
}

TEST(ChernCocycleTest, AFull_ChernCocycle_4_1) {
  const auto pl = [](auto... points) {
    return plucker({points...});
  };

  EXPECT_EXPR_ZERO(
    + a_full(ChernCocycle(4, 1, {1,2,3,4,5}), 6)
    // first 3 term: expanded residue([t,1,2,3] ^ [t,4]/[t,3] ^ [t,5]/[t,3] ^ [t,6]/[t,3])
    - ncomultiply(ncoproduct(GLi2(4,1,2,3), pl(4,5) - pl(4,3), pl(4,6) - pl(4,3)))
    + ncomultiply(ncoproduct(GLi2(5,1,2,3), pl(5,4) - pl(5,3), pl(5,6) - pl(5,3)))
    - ncomultiply(ncoproduct(GLi2(6,1,2,3), pl(6,4) - pl(6,3), pl(6,5) - pl(6,3)))
    - ncomultiply(ncoproduct(ChernCocycle(3, 2, {1,3,4,5,6}) - ChernCocycle(3, 2, {2,3,4,5,6}), pl(1,2)))
    + ncomultiply(ncoproduct(ChernCocycle(3, 2, {1,3,4,5,6}), pl(1,3)))
    - ncomultiply(ncoproduct(ChernCocycle(3, 2, {2,3,4,5,6}), pl(2,3)))
    - ncomultiply(ncoproduct(GLi2(3,4,5,6), pl(1,2), pl(2,3)))
    - ncomultiply(ncoproduct(GLi2(3,4,5,6), pl(2,3), pl(3,1)))
    - ncomultiply(ncoproduct(GLi2(3,4,5,6), pl(3,1), pl(1,2)))
  );

  EXPECT_EXPR_ZERO(
    + a_full(ChernCocycle(4, 1, {1,2,3,4,5}), 6)
    - ncomultiply(ncoproduct(GLi2(1,3,4,6), pl(4,5), pl(1,2)))
    + ncomultiply(ncoproduct(GLi2(1,3,4,6), pl(4,5), pl(1,3)))
    + ncomultiply(ncoproduct(GLi2(1,3,4,6), pl(4,6), pl(1,2)))
    - ncomultiply(ncoproduct(GLi2(1,3,4,6), pl(4,6), pl(1,3)))
    + ncomultiply(ncoproduct(GLi2(1,3,5,6), pl(4,5), pl(1,2)))
    - ncomultiply(ncoproduct(GLi2(1,3,5,6), pl(4,5), pl(1,3)))
    - ncomultiply(ncoproduct(GLi2(1,3,5,6), pl(5,6), pl(1,2)))
    + ncomultiply(ncoproduct(GLi2(1,3,5,6), pl(5,6), pl(1,3)))
    - ncomultiply(ncoproduct(GLi2(1,4,5,6), pl(1,3), pl(1,2)))
    + ncomultiply(ncoproduct(GLi2(1,4,5,6), pl(1,6), pl(1,2)))
    - ncomultiply(ncoproduct(GLi2(1,4,5,6), pl(1,6), pl(1,3)))
    + ncomultiply(ncoproduct(GLi2(2,3,4,6), pl(4,5), pl(1,2)))
    - ncomultiply(ncoproduct(GLi2(2,3,4,6), pl(4,5), pl(2,3)))
    - ncomultiply(ncoproduct(GLi2(2,3,4,6), pl(4,6), pl(1,2)))
    + ncomultiply(ncoproduct(GLi2(2,3,4,6), pl(4,6), pl(2,3)))
    - ncomultiply(ncoproduct(GLi2(2,3,5,6), pl(4,5), pl(1,2)))
    + ncomultiply(ncoproduct(GLi2(2,3,5,6), pl(4,5), pl(2,3)))
    + ncomultiply(ncoproduct(GLi2(2,3,5,6), pl(5,6), pl(1,2)))
    - ncomultiply(ncoproduct(GLi2(2,3,5,6), pl(5,6), pl(2,3)))
    + ncomultiply(ncoproduct(GLi2(2,4,5,6), pl(2,3), pl(1,2)))
    - ncomultiply(ncoproduct(GLi2(2,4,5,6), pl(2,6), pl(1,2)))
    + ncomultiply(ncoproduct(GLi2(2,4,5,6), pl(2,6), pl(2,3)))
    - ncomultiply(ncoproduct(GLi2(3,4,5,6), pl(2,3), pl(1,3)))
    + ncomultiply(ncoproduct(GLi2(3,4,5,6), pl(3,6), pl(1,3)))
    - ncomultiply(ncoproduct(GLi2(3,4,5,6), pl(3,6), pl(2,3)))
    + ncomultiply(ncoproduct(GLi2(4,1,2,3), pl(3,4), pl(4,6)))
    + ncomultiply(ncoproduct(GLi2(4,1,2,3), pl(4,5), pl(3,4)))
    - ncomultiply(ncoproduct(GLi2(4,1,2,3), pl(4,5), pl(4,6)))
    - ncomultiply(ncoproduct(GLi2(5,1,2,3), pl(3,5), pl(5,6)))
    - ncomultiply(ncoproduct(GLi2(5,1,2,3), pl(4,5), pl(3,5)))
    + ncomultiply(ncoproduct(GLi2(5,1,2,3), pl(4,5), pl(5,6)))
    + ncomultiply(ncoproduct(GLi2(6,1,2,3), pl(3,6), pl(5,6)))
    + ncomultiply(ncoproduct(GLi2(6,1,2,3), pl(4,6), pl(3,6)))
    - ncomultiply(ncoproduct(GLi2(6,1,2,3), pl(4,6), pl(5,6)))
  );
}
