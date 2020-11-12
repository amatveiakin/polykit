#include "lib/polylog_quadrangle.h"

#include "gtest/gtest.h"

#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "test_util/matchers.h"


TEST(LiQuadTest, EvaluatedExorHasNoMonsters_Arg6_Weight3) {
  EXPECT_EXPR_ZERO(
    theta_expr_keep_monsters(eval_formal_symbols(
      LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::odd)
    ))
  );
}
TEST(LiQuadTest, EvaluatedExorHasNoMonsters_Arg6_Weight4) {
  EXPECT_EXPR_ZERO(
    theta_expr_keep_monsters(eval_formal_symbols(
      LiQuad(3, {1,2,3,4,5,6}, LiFirstPoint::odd)
    ))
  );
}

// TODO: More args and weights

TEST(LiQuadTest, CompareToLegacy_Odd_Arg6_Weight3) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::odd))
    ),
    Lido3(1,2,3,4,5,6)
  );
}
TEST(LiQuadTest, CompareToLegacy_Odd_Arg6_Weight4) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(3, {1,2,3,4,5,6}, LiFirstPoint::odd))
    ),
    Lido4(1,2,3,4,5,6)
  );
}
#if RUN_LARGE_TESTS
TEST(LiQuadTest, CompareToLegacy_Odd_Arg8_Weight3) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(1, {1,2,3,4,5,6,7,8}, LiFirstPoint::odd))
    ),
    Lido3(1,2,3,4,5,6,7,8)
  );
}
#endif

TEST(LiQuadTest, CompareToLegacy_Even_Arg6_Weight3) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::even))
    ),
    LidoNeg3(1,2,3,4,5,6)
  );
}
TEST(LiQuadTest, CompareToLegacy_Even_Arg6_Weight4) {
  EXPECT_EXPR_EQ(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(3, {1,2,3,4,5,6}, LiFirstPoint::even))
    ),
    LidoNeg4(1,2,3,4,5,6)
  );
}
