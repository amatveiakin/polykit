#include "lib/delta.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(DeltaExprTest, SubstituteVariables) {
  const auto expr =
    +  D(x1, x2)
    -2*D(x1, -x3)
    +  D(x5, x6)
  ;
  EXPECT_EXPR_EQ(
    substitute_variables(expr, {-x2,x2,x3,x4,x5,x6}),
    +  D(x2, Zero)
    -2*D(x2, x3)
    +  D(x5, x6)
  );
  EXPECT_EXPR_EQ(
    substitute_variables(expr, {x1,-x2,-x3,x4,x5,x6}),
    +  D(x1, -x2)
    -2*D(x1, x3)
    +  D(x5, x6)
  );
  EXPECT_EXPR_EQ(
    substitute_variables(expr, {x1,x2,Inf,x4,x5,x6}),
    +  D(x1, x2)
    +  D(x5, x6)
  );
  EXPECT_EXPR_EQ(
    substitute_variables(expr, {x1,x2,Zero,x4,x5,x6}),
    +  D(x1, x2)
    -2*D(x1, Zero)
    +  D(x5, x6)
  );
  EXPECT_EXPR_EQ(
    substitute_variables(expr, {x1,Zero,Zero,Zero,Zero,Zero}),
    -  D(x1, Zero)
  );
}
