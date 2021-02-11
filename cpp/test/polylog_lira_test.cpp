#include "lib/polylog_lira.h"

#include "gtest/gtest.h"

#include "lib/polylog_li.h"
#include "test_util/matchers.h"


TEST(LiraTest, DifferentSubstitutionsEquivalent) {
  auto cr_a  = CR(2,1,7,8);
  auto cr_b1 = CR(3,4,5,6);
  auto cr_b2 = CR(3,6,1,2);
  auto separate_vars = epsilon_expr_to_theta_expr(
    Li(2,2)({1},{2,3}), {
    {cr_a}, {cr_b1}, {cr_b2}
  });
  auto multiply_first = epsilon_expr_to_theta_expr(
    Li(2,2)({1},{2}), {
    {cr_a}, {cr_b1 * cr_b2}
  });
  auto lira = Lira(2,2)(cr_a, cr_b1 * cr_b2);
  EXPECT_EXPR_EQ(separate_vars, multiply_first);
  EXPECT_EXPR_EQ(lira, multiply_first);
}

TEST(CoLiraTest, DifferentSubstitutionsEquivalent) {
  auto cr_a  = CR(2,1,7,8);
  auto cr_b1 = CR(3,4,5,6);
  auto cr_b2 = CR(3,6,1,2);
  auto separate_vars = epsilon_coexpr_to_theta_coexpr(
    CoLi(2,2)({1},{2,3}), {
    {cr_a}, {cr_b1}, {cr_b2}
  });
  auto multiply_first = epsilon_coexpr_to_theta_coexpr(
    CoLi(2,2)({1},{2}), {
    {cr_a}, {cr_b1 * cr_b2}
  });
  auto lira = CoLira(2,2)(cr_a, cr_b1 * cr_b2);
  EXPECT_EXPR_EQ(separate_vars, multiply_first);
  EXPECT_EXPR_EQ(lira, multiply_first);
}

TEST(LiraTest, HandwrittenLiQuad_StartEven_HasNoMonsters) {
  const int foreweight = 2;  // Should be true for any foreweight
  EXPECT_EXPR_ZERO(
    theta_expr_keep_monsters(
      + epsilon_expr_to_theta_expr(LiVec(foreweight, {1,1}, {{1},{2}}), {
        {CR(3,4,7,2)},
        {CR(5,6,7,4)},
      })
      - epsilon_expr_to_theta_expr(LiVec(foreweight, {1,1}, {{1},{2}}), {
        {CR(3,6,7,2)},
        {CR(3,4,5,6)},
      })
      + epsilon_expr_to_theta_expr(LiVec(foreweight, {1,1}, {{1},{2}}), {
        {CR(5,6,7,2)},
        {CR(3,4,5,2)},
      })
      + epsilon_expr_to_theta_expr(LiVec(foreweight, {2}, {{1,2}}), {
        {CR(5,6,7,2)},
        {CR(3,4,5,2)},
      })
    )
  );
}

TEST(CoLiraTest, HandwrittenCoLiQuad_StartOdd_HasNoMonsters) {
  EXPECT_EXPR_ZERO(
    theta_coexpr_keep_monsters(
      + CoLira(1,1)(CR(1,2,3,6), CR(3,4,5,6))
      - CoLira(1,1)(CR(1,2,5,6), CR(3,4,5,2))
      + CoLira(1,1)(CR(1,4,5,6), CR(1,2,3,4))
    )
  );
};
