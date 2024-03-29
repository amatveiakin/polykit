// TODO: More tests for normal comultiplication:
//   - goldens;
//   - coincides with the iterative one if applied once.
// TODO: Tests for Hopf coproduct (coproduct_is_lie_algebra == false).

#include "lib/coalgebra.h"

#include "gtest/gtest.h"

#include "lib/iterated_integral.h"
#include "lib/polylog_li.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


TEST(CoproductTest, TwoExpressions) {
  EXPECT_EXPR_EQ(
    icoproduct(
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
    icomultiply(
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
    icomultiply(
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
    icomultiply(
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

TEST(ComultiplyTest, ZeroResult) {
  EXPECT_EXPR_EQ(
    icomultiply(
      + SV({1,1,2,3})
      ,
      {2, 2}
    ),
    SimpleVectorICoExpr{}
  );
}

TEST(CoproductTest, IteratedComultiplication_2_2_2) {
  EXPECT_EXPR_EQ(
    icomultiply(
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

TEST(NComultiplyTest, IterationGivesZero) {
  EXPECT_EXPR_ZERO(ncomultiply(ncomultiply(
    + SV({1,2,3,4,5,6})
    + SV({2,3,4,5,6,7})
    + SV({8,7,6,5,4,3})
    + SV({1,3,5,7,2,4})
    + SV({4,7,2,8,3,1})
  )));
}

TEST(CoalgebraUtilTest, FilterCoExpr) {
  EXPECT_EXPR_EQ(
    filter_coexpr_predicate(
      CoLi(1,5)({1},{2}),
      0,
      [](const EpsilonPack& pack) {
        return std::visit(overloaded{
          [](const std::vector<Epsilon>& /*product*/) {
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
      - icoproduct(EFormalSymbolPositive(LiParam(0, {5}, {{1,2}})), EVar(1))
      + icoproduct(EFormalSymbolPositive(LiParam(0, {5}, {{1,2}})), EComplementIndexList({1}))
    )
  );
}
