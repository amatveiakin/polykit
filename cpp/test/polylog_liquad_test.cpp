#include "lib/polylog_liquad.h"

#include "gtest/gtest.h"

#include "lib/polylog_lira.h"
#include "lib/polylog_qli.h"
#include "test_util/matchers.h"


TEST(LiQuadTest, EvaluatedExorHasNoMonsters_Arg6_Weight3) {
  EXPECT_EXPR_ZERO(
    keep_monsters(eval_formal_symbols(
      LiQuad(1, {1,2,3,4,5,6}, LiFirstPoint::odd)
    ))
  );
}
TEST(LiQuadTest, LARGE_EvaluatedExorHasNoMonsters_Arg6_Weight4) {
  EXPECT_EXPR_ZERO(
    keep_monsters(eval_formal_symbols(
      LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::odd)
    ))
  );
}

TEST(LiQuadTest, CompareToLegacy_Odd_Arg6_Weight3) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(1, {1,2,3,4,5,6}, LiFirstPoint::odd))
    ),
    QLi3(1,2,3,4,5,6)
  );
}
TEST(LiQuadTest, CompareToLegacy_Odd_Arg6_Weight4) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::odd))
    ),
    QLi4(1,2,3,4,5,6)
  );
}
TEST(LiQuadTest, LARGE_CompareToLegacy_Odd_Arg8_Weight3) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(0, {1,2,3,4,5,6,7,8}, LiFirstPoint::odd))
    ),
    QLi3(1,2,3,4,5,6,7,8)
  );
}

TEST(LiQuadTest, CompareToLegacy_Even_Arg6_Weight3) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(1, {1,2,3,4,5,6}, LiFirstPoint::even))
    ),
    QLiNeg3(1,2,3,4,5,6)
  );
}
TEST(LiQuadTest, LARGE_CompareToLegacy_Even_Arg6_Weight4) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::even))
    ),
    QLiNeg4(1,2,3,4,5,6)
  );
}
