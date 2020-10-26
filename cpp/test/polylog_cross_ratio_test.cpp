#include "lib/polylog_cross_ratio.h"

#include "gtest/gtest.h"

#include "lib/projection.h"
#include "test_util/matchers.h"


TEST(LiraTest, Lira4_6) {
  auto expr = Lira4(1,2,3,4,5,6);
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
