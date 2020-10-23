#include "lib/coalgebra.h"

#include "lib/iterated_integral.h"
#include "test_util/matchers.h"

#include "gtest/gtest.h"


TEST(CoproductTest, TwoExpressions) {
  EXPECT_EXPR_EQ(
    coproduct(
      +  WordExpr::single(Word{1})
      -  WordExpr::single(Word{2})
      ,
      +  WordExpr::single(Word{3})
      +3*WordExpr::single(Word{4})
    ),
    (
      +  WordCoExpr::single(MultiWord({{1}, {3}}))
      +3*WordCoExpr::single(MultiWord({{1}, {4}}))
      -  WordCoExpr::single(MultiWord({{2}, {3}}))
      -3*WordCoExpr::single(MultiWord({{2}, {4}}))
    )
  );
}

TEST(ComultiplyTest, Form_1_1) {
  EXPECT_EXPR_EQ(
    comultiply(
      +2*WordExpr::single(Word{1,2})
      ,
      {1, 1}
    ),
    (
      +2*WordCoExpr::single(MultiWord({{1}, {2}}))
    )
  );
}

TEST(ComultiplyTest, Form_2_2) {
  EXPECT_EXPR_EQ(
    comultiply(
      + WordExpr::single(Word{1,3,2,4})
      + WordExpr::single(Word{4,3,2,1})
      ,
      {2, 2}
    ),
    (
      + WordCoExpr::single(MultiWord({{1,3}, {2,4}}))
      - WordCoExpr::single(MultiWord({{1,2}, {3,4}}))
    )
  );
}

TEST(ComultiplyTest, I_6) {
  EXPECT_EXPR_EQ(
    comultiply(
      + I(1,2,3,4,5,6)
      ,
      {2, 2}
    ),
    (
      + coproduct(I(1,2,3,4), I(1,4,5,6))
      + coproduct(I(2,3,4,5), I(1,2,5,6))
      + coproduct(I(3,4,5,6), I(1,2,3,6))
    )
  );
}

TEST(ComultiplyTest, Zero) {
  EXPECT_EXPR_EQ(
    comultiply(
      + WordExpr::single(Word{1,1,2,3})
      ,
      {2, 2}
    ),
    WordCoExpr{}
  );
}
