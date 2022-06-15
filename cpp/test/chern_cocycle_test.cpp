#include "lib/chern_cocycle.h"

#include "gtest/gtest.h"

#include "lib/chern_arrow.h"
#include "lib/polylog_cgrli.h"
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
    + CGrLiVec(weight, args({1,2,3,4,5,6}))
    - pullback(CGrLiVec(weight, args({2,3,5,6})), args({1}))
    + pullback(CGrLiVec(weight, args({2,3,4,6})), args({1}))
    - pullback(CGrLiVec(weight, args({2,3,4,5})), args({1}))
    + pullback(CGrLiVec(weight, args({1,3,5,6})), args({2}))
    - pullback(CGrLiVec(weight, args({1,3,4,6})), args({2}))
    + pullback(CGrLiVec(weight, args({1,3,4,5})), args({2}))
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
    - CGrLiVec(weight, args({2,3,5,6}))
    + CGrLiVec(weight, args({2,3,4,6}))
    - CGrLiVec(weight, args({2,3,4,5}))
    + CGrLiVec(weight, args({1,3,5,6}))
    - CGrLiVec(weight, args({1,3,4,6}))
    + CGrLiVec(weight, args({1,3,4,5}))
  ;
}

GammaExpr ChernCocycle_4_4_8(const std::vector<int>& points) {
  const int weight = 4;
  CHECK_EQ(points.size(), 8);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return plucker_dual(
    + CGrLiVec(weight, args({1,2,3,4,5,6,7,8}))
    - pullback(CGrLiVec(weight, args({2,3,4,6,7,8})), args({1}))
    + pullback(CGrLiVec(weight, args({2,3,4,5,7,8})), args({1}))
    - pullback(CGrLiVec(weight, args({2,3,4,5,6,8})), args({1}))
    + pullback(CGrLiVec(weight, args({2,3,4,5,6,7})), args({1}))
    + pullback(CGrLiVec(weight, args({1,3,4,6,7,8})), args({2}))
    - pullback(CGrLiVec(weight, args({1,3,4,5,7,8})), args({2}))
    + pullback(CGrLiVec(weight, args({1,3,4,5,6,8})), args({2}))
    - pullback(CGrLiVec(weight, args({1,3,4,5,6,7})), args({2}))
    - pullback(CGrLiVec(weight, args({1,2,4,6,7,8})), args({3}))
    + pullback(CGrLiVec(weight, args({1,2,4,5,7,8})), args({3}))
    - pullback(CGrLiVec(weight, args({1,2,4,5,6,8})), args({3}))
    + pullback(CGrLiVec(weight, args({1,2,4,5,6,7})), args({3}))
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
    + CGrLiVec(weight, args({2,3,4,6,7,8}))
    - CGrLiVec(weight, args({2,3,4,5,7,8}))
    + CGrLiVec(weight, args({2,3,4,5,6,8}))
    - CGrLiVec(weight, args({2,3,4,5,6,7}))
    - CGrLiVec(weight, args({1,3,4,6,7,8}))
    + CGrLiVec(weight, args({1,3,4,5,7,8}))
    - CGrLiVec(weight, args({1,3,4,5,6,8}))
    + CGrLiVec(weight, args({1,3,4,5,6,7}))
    + CGrLiVec(weight, args({1,2,4,6,7,8}))
    - CGrLiVec(weight, args({1,2,4,5,7,8}))
    + CGrLiVec(weight, args({1,2,4,5,6,8}))
    - CGrLiVec(weight, args({1,2,4,5,6,7}))
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
