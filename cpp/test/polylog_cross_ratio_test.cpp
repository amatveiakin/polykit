#include "lib/polylog_cross_ratio.h"

#include "gtest/gtest.h"

#include "lib/projection.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


TEST(LidoTest, Lido4_Arg6) {
  auto expr = Lido4(1,2,3,4,5,6);
  EXPECT_EQ(expr.size(), 2560);
  auto lyndon = to_lyndon_basis(expr);
  EXPECT_EQ(lyndon.size(), 1272);
  auto lyndon_then_project = project_on_x1(lyndon);
  auto project_then_lyndon = to_lyndon_basis(project_on_x1(expr));
  EXPECT_EXPR_EQ(lyndon_then_project, project_then_lyndon);
  EXPECT_EXPR_EQ(
    terms_with_min_distinct_elements(lyndon_then_project, 4)
    ,
    + W({2, 3, 4, 5})
    - W({2, 3, 4, 6})
    + W({2, 3, 5, 6})
    - W({2, 4, 5, 6})
    + W({3, 4, 5, 6})
  );
}

TEST(LidoTest, LidoSymmEquation_Arg6) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + LidoSymm2(1,2,3,4,5,6)
    + LidoSymm2(2,3,4,5,6,1)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + LidoSymm3(1,2,3,4,5,6)
    - LidoSymm3(2,3,4,5,6,1)
  );
}

TEST(LidoTest, LidoSymmEquation_Arg8) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + LidoSymm3(1,2,3,4,5,6,7,8)
    - LidoSymm3(2,3,4,5,6,7,8,1)
  );
}

TEST(LidoTest, LidoSymm_Arg8_AlternativeFormula) {
  EXPECT_EXPR_EQ(
    + LidoSymm3(1,2,3,4,5,6,7,8)
    ,
    + Lido3(1,2,3,4,5,6,7,8)
    + (
      - LidoSymm3(1,2,3,4,5,6)
      - LidoSymm3(3,4,5,6,7,8)
      - LidoSymm3(5,6,7,8,1,2)
      - LidoSymm3(7,8,1,2,3,4)
      - Lido3(1,2,3,4,5,6)
      - Lido3(3,4,5,6,7,8)
      - Lido3(5,6,7,8,1,2)
      - Lido3(7,8,1,2,3,4)
    ).dived_int(2)
  );
}

TEST(LidoTest, LidoBuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x1(Lido3(1,2,3,4,5,6)),
    LidoVecPr(3, {1,2,3,4,5,6}, project_on_x1)
  );
  EXPECT_EXPR_EQ(
    project_on_x7(Lido3(1,7,3,4,5,6,7,8)),
    LidoVecPr(3, {1,7,3,4,5,6,7,8}, project_on_x7)
  );
}

TEST(LidoTest, LidoSymmBuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x4(LidoSymm3(1,2,3,4,5,6)),
    LidoSymmVecPr(3, {1,2,3,4,5,6}, project_on_x4)
  );
}
