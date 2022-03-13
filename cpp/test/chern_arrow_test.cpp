
#include "lib/chern_arrow.h"

#include "gtest/gtest.h"

#include "test_util/helpers.h"
#include "test_util/matchers.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"


int detect_num_variables(const GammaExpr& expr) {
  int max_var = 0;
  for (const auto& [term, coeff] : expr) {
    for (const Gamma& g : term) {
      max_var = std::max(max_var, max_value(g.index_vector()));
    }
  }
  CHECK_GT(max_var, 0);
  return max_var;
}

// Identities that should be true for all symbols.
class ChernArrowIdentityTest : public testing::TestWithParam<GammaExpr> {
public:
  const GammaExpr& expr() const { return GetParam(); }
};

TEST_P(ChernArrowIdentityTest, LeftLeft) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(chern_arrow_left(chern_arrow_left(expr(), n+1), n+2));
}

TEST_P(ChernArrowIdentityTest, UpUp) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(chern_arrow_left(chern_arrow_left(expr(), n+1), n+2));
}

TEST_P(ChernArrowIdentityTest, LeftUp) {
  const int n = detect_num_variables(expr());
  EXPECT_EXPR_ZERO(
    + chern_arrow_left(chern_arrow_up(expr(), n+1), n+2)
    + chern_arrow_up(chern_arrow_left(expr(), n+1), n+2)
  );
}

INSTANTIATE_TEST_SUITE_P(AllCases, ChernArrowIdentityTest, testing::Values(
  GrQLi2(1)(2,3,4,5,6,7),
  GrLiVec({1}, {2,3,4,5}),
  G({1,2,3,4,5})
));