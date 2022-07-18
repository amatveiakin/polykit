
#include "lib/chern_arrow.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"


static std::vector exprs_odd_num_points = {
  GLi2[{7}](1,2,3,4,5,6),
  GrQLi2(7)(1,2,3,4,5,6),
  GrLi(5)(1,2,3,4),
  G({1,2,3,4,5}),
  tensor_product(G({1,2,3}), G({3,4,5})),
};

static std::vector exprs_even_num_points = {
  GLi2(1,2,3,4,5,6),
  GrQLi2()(1,2,3,4,5,6),
  GrLi(5,6)(1,2,3,4),
  G({1,2,3,4,5,6}),
  tensor_product(G({1,2,3,4}), G({3,4,5,6})),
};

// Identities that should be true for all symbols.
class ChernArrowIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};
class OddNumPointsIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};
class EvenNumPointsIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};

TEST_P(ChernArrowIdentityTest, LeftLeft) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(chern_arrow_left(chern_arrow_left(expr(), n+1), n+2));
}

TEST_P(ChernArrowIdentityTest, UpUp) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(chern_arrow_up(chern_arrow_up(expr(), n+1), n+2));
}

TEST_P(ChernArrowIdentityTest, LeftUp) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(
    + chern_arrow_left(chern_arrow_up(expr(), n+1), n+2)
    + chern_arrow_up(chern_arrow_left(expr(), n+1), n+2)
  );
}

TEST_P(OddNumPointsIdentityTest, ABOdd) {
  const auto& x = expr();
  const int n = detect_num_variables(x);
  EXPECT_EXPR_ZERO(a_plus(b_plus(x, n+1), n+2) + b_plus(a_plus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_minus(b_minus(x, n+1), n+2) + b_minus(a_minus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_minus(a_minus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_plus(a_plus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(b_minus(b_minus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(b_plus(b_plus(x, n+1), n+2));
  EXPECT_EXPR_EQ(chern_arrow_left(x, n+1), a_minus(x, n+1) + a_plus(x, n+1));
  EXPECT_EXPR_EQ(chern_arrow_up(x, n+1), b_minus(x, n+1) + b_plus(x, n+1));
}

TEST_P(EvenNumPointsIdentityTest, ABEven) {
  const auto& x = expr();
  const int n = detect_num_variables(x);
  EXPECT_EXPR_ZERO(a_plus(a_minus(x, n+1), n+2) + a_minus(a_plus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_plus(b_minus(x, n+1), n+2) + b_minus(a_plus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_minus(b_plus(x, n+1), n+2) + b_plus(a_minus(x, n+1), n+2));
  EXPECT_EXPR_ZERO(a_minus(chern_arrow_left(x, n+1), n+2) + a_plus(chern_arrow_left(x, n+1), n+2));
  EXPECT_EXPR_EQ(a_minus(chern_arrow_left(x, n+1), n+2), a_minus(a_plus(x, n+1), n+2));
}

INSTANTIATE_TEST_SUITE_P(AllCases, ChernArrowIdentityTest, testing::ValuesIn(
  concat(exprs_odd_num_points, exprs_even_num_points)
));
INSTANTIATE_TEST_SUITE_P(AllCases, OddNumPointsIdentityTest, testing::ValuesIn(exprs_odd_num_points));
INSTANTIATE_TEST_SUITE_P(AllCases, EvenNumPointsIdentityTest, testing::ValuesIn(exprs_even_num_points));
