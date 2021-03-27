#include "lib/loops.h"

#include "gtest/gtest.h"

#include "lib/polylog_liquad.h"
#include "lib/summation.h"
#include "test_util/matchers.h"


// Note that conversion the other way round is, generally speaking, non-reversible
// due to the fact that `lira_expr_to_loop_expr` doesn't group five-term relations
// together.
TEST(LoopsTest, LoopsLiraConversionReversible) {
  const int num_args = 8;  // should work for any even number
  auto expr = theta_expr_to_lira_expr_without_products(
    LiQuad(num_args / 2 - 1, seq_incl(1, num_args))
  );
  EXPECT_EXPR_EQ(
    expr,
    loop_expr_to_lira_expr(lira_expr_to_loop_expr(expr))
  );
}

TEST(LoopsTest, CurLoopsEquivalentToLiQuad) {
  const int num_points = 9;  // should work for any odd number
  auto theta_expr = sum_looped_vec(
    [&](SpanX args) {
      return LiQuad(num_points / 2 - 1, args.as_int());
    },
    num_points,
    seq_incl(1, num_points - 1)
  );

  auto lira_expr = theta_expr_to_lira_expr_without_products(theta_expr.without_annotations());

  auto loop_expr = reverse_loops(cut_loops(seq_incl(1, num_points)));
  auto loop_lira_expr = loop_expr_to_lira_expr(loop_expr);

  lira_expr = lira_expr_sort_args(lira_expr);
  loop_lira_expr = lira_expr_sort_args(loop_lira_expr);

  EXPECT_EXPR_EQ_AFTER_LYNDON(
    lira_expr,
    -loop_lira_expr
  );
}
