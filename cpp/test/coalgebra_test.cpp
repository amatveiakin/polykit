// TODO: Test coproduct_is_lie_algebra == false.

#include "lib/coalgebra.h"

#include "gtest/gtest.h"

#include "lib/iterated_integral.h"
#include "lib/polylog_li.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


TEST(CoproductTest, TwoExpressions) {
  EXPECT_EXPR_EQ(
    coproduct(
      +  SV({1})
      -  SV({2})
      ,
      +  SV({3})
      +3*SV({4})
    ),
    (
      +  CoSV({{1}, {3}})
      +3*CoSV({{1}, {4}})
      -  CoSV({{2}, {3}})
      -3*CoSV({{2}, {4}})
    )
  );
}

TEST(ComultiplyTest, Form_1_1) {
  EXPECT_EXPR_EQ(
    comultiply(
      +2*SV({1,2})
      ,
      {1, 1}
    ),
    (
      +2*CoSV({{1}, {2}})
    )
  );
}

TEST(ComultiplyTest, Form_1_2) {
  EXPECT_EXPR_EQ(
    comultiply(
      + SV({1,2,3})
      ,
      {1, 2}
    ),
    (
      + CoSV({{1}, {2,3}})
      - CoSV({{3}, {1,2}})
    )
  );
}

TEST(ComultiplyTest, Form_2_2) {
  EXPECT_EXPR_EQ(
    comultiply(
      + SV({1,3,2,4})
      + SV({4,3,2,1})
      ,
      {2, 2}
    ),
    (
      + CoSV({{1,3}, {2,4}})
      - CoSV({{1,2}, {3,4}})
    )
  );
}

TEST(ComultiplyTest, Zero) {
  EXPECT_EXPR_EQ(
    comultiply(
      + SV({1,1,2,3})
      ,
      {2, 2}
    ),
    SimpleVectorCoExpr{}
  );
}

TEST(CoproductTest, IteratedComultiplication_2_2_2) {
  EXPECT_EXPR_EQ(
    comultiply(
      + SV({4,3,1,2,5,6})
      ,
      {2, 2, 2}
    ),
    (
      + CoSV({{1,2}, {3,4}, {5,6}})
      + CoSV({{1,2}, {5,6}, {3,4}})
    )
  );
}

TEST(CoalgebraUtilTest, FilterCoExpr) {
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
      - coproduct(EFormalSymbolPositive(LiParam(0, {5}, {{1,2}})), EVar(1))
      + coproduct(EFormalSymbolPositive(LiParam(0, {5}, {{1,2}})), EComplementIndexList({1}))
    )
  );
}
