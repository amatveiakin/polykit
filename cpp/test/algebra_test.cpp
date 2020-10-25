#include "lib/algebra.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


using StringExpr = Linear<SimpleLinearParam<std::string>>;

std::string concat_strings(std::string a, std::string b) {
  return std::move(a) + std::move(b);
}


TEST(OuterProductTest, TwoExpressions) {
  EXPECT_EXPR_EQ(
    outer_product<StringExpr>(
      +  StringExpr::single("a")
      -  StringExpr::single("b")
      ,
      +  StringExpr::single("c")
      +3*StringExpr::single("d")
      ,
      concat_strings
    ),
    (
      +  StringExpr::single("ac")
      +3*StringExpr::single("ad")
      -  StringExpr::single("bc")
      -3*StringExpr::single("bd")
    )
  );
}

TEST(OuterProductTest, ThreeExpressions) {
  EXPECT_EXPR_EQ(
    outer_product<StringExpr>({
      +  StringExpr::single("a")
      -  StringExpr::single("b")
      ,
      +  StringExpr::single("c")
      +3*StringExpr::single("d")
      ,
      +2*StringExpr::single("e")
      +  StringExpr::single("f")
      }
      ,
      concat_strings
    ),
    (
      +2*StringExpr::single("ace")
      +6*StringExpr::single("ade")
      -2*StringExpr::single("bce")
      -6*StringExpr::single("bde")
      +  StringExpr::single("acf")
      +3*StringExpr::single("adf")
      -  StringExpr::single("bcf")
      -3*StringExpr::single("bdf")
    )
  );
}
