#include "lib/coalgebra.h"

#include "gtest/gtest.h"

#include "lib/iterated_integral.h"
#include "lib/polylog.h"
#include "test_util/matchers.h"


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

// TODO[formal-symbol-coproduct]: This should pass!
#if 0
TEST(CoalgebraUtilTest, FilterCoexpr) {
  EXPECT_EXPR_EQ(
    filter_coexpr_predicate(
      CoLi(1,5)({1},{2}),
      0,
      [](const EpsilonPack& pack) {
        return std::visit(overloaded{
          [](const std::vector<Epsilon>& product) {
            return false;
          },
          [](const LiParam& formal_symbol) {
            return formal_symbol.points().size() == 1 &&
              formal_symbol.points().front().size() == 2 &&
              formal_symbol.weights().front() >= 5;
          },
        }, pack);
      }
    ),
    (
      - coproduct(EFormalSymbolPositive(LiParam(1, {5}, {{1,2}})), EVar(1))
      + coproduct(EFormalSymbolPositive(LiParam(1, {5}, {{1,2}})), EComplementIndexList({1}))
    )
  );
}
#endif
