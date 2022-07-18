#include "lib/chern_cocycle.h"

#include "gtest/gtest.h"

#include "lib/chern_arrow.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_gr_space.h"
#include "test_util/matchers.h"
#include "test_util/space_matchers.h"


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


TEST(ChernCocycleTest, EqualsExplicit_Weight3) {
  std::vector points6 = {3,4,5,6,1,2};
  EXPECT_EXPR_EQ(ncoproduct(ChernCocycle_3_3_6(points6)), ChernCocycle(3, 3, points6));
  EXPECT_EXPR_EQ(ncoproduct(ChernCocycle_3_2_6(points6)), ChernCocycle(3, 2, points6));
}

TEST(ChernCocycleTest, LARGE_EqualsExplicit_Weight4) {
  std::vector points8 = {3,4,5,6,7,8,1,2};
  EXPECT_EXPR_EQ(ncoproduct(ChernCocycle_4_4_8(points8)), ChernCocycle(4, 4, points8));
  EXPECT_EXPR_EQ(ncoproduct(ChernCocycle_4_3_8(points8)), ChernCocycle(4, 3, points8));
}

TEST(ChernCocycleTest, InteractionWithArrows_Weight3) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_up(ChernCocycle(3, 3, {1,2,3,4,5,6}), 7));
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_left(ChernCocycle(3, 2, {1,2,3,4,5,6}), 7));
}

TEST(ChernCocycleTest, LARGE_InteractionWithArrows_Weight4) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_up(ChernCocycle(4, 4, {1,2,3,4,5,6,7,8}), 9));
  EXPECT_EXPR_ZERO_AFTER_LYNDON(chern_arrow_left(ChernCocycle(4, 3, {1,2,3,4,5,6,7,8}), 9));
}

TEST(ChernCocycleTest, DimensionMinusOneEquation_Weight3) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + chern_arrow_left(ChernCocycle(3, 3, {1,2,3,4,5,6}), 7)
    + chern_arrow_up(ChernCocycle(3, 2, {1,2,3,4,5,6}), 7)
  );
}

TEST(ChernCocycleTest, LARGE_DimensionMinusOneEquation_Weight4) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + chern_arrow_left(ChernCocycle(4, 4, {1,2,3,4,5,6,7,8}), 9)
    + chern_arrow_up(ChernCocycle(4, 3, {1,2,3,4,5,6,7,8}), 9)
  );
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

TEST(ChernCocycleTest, LARGE_ReversedDual_Comultiplication_3_1_Dim4) {
  // Note: `plucker_dual` here reverses `plucker_dual` in `ChernCocycle` definition.
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
