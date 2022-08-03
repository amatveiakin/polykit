#include "lib/loops.h"

#include "gtest/gtest.h"

#include "lib/polylog_liquad.h"
#include "lib/summation.h"
#include "test_util/matchers.h"


static constexpr auto cycle = loop_expr_cycle;
template<typename... Args> static LoopExpr Q(Args... args) { return loops_Q(std::vector{args...}); }
template<typename... Args> static LoopExpr S(Args... args) { return loops_S(std::vector{args...}); }
template<typename... Args> static auto tp(Args... args) { return tensor_product(absl::MakeConstSpan({args...})); }


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

TEST(LoopsTest, CutLoopsEquivalentToLiQuad) {
  const int num_points = 9;  // should work for any odd number
  auto theta_expr = sum_looped_vec(
    [&](const auto& args) {
      return LiQuad(num_points / 2 - 1, args);
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

// Formulas from "3.5. Proof of the higher Gangl formula in weights four and six"
//   in https://arxiv.org/pdf/2208.01564v1.pdf
TEST(LoopsTest, DFunctionExpanded) {
  const auto normalize = [](const auto& expr) {
    return to_canonical_permutation(to_lyndon_basis(
      remove_duplicate_loops(remove_loops_with_duplicates(fully_normalize_loops(expr)))
    ));
  };

  const auto d1 = -S(0,1,0,1,2,3,4,5,6);  // `a` in the old notation
  EXPECT_EXPR_ZERO(normalize(
    - d1
    + tp(Q(0,1,2,3), Q(0,1,3,4), S(0,1,4,5,6))
    - tp(Q(0,1,2,3), S(0,1,3,4,5), Q(0,1,5,6))
    + tp(Q(0,1,5,6), Q(0,1,4,5), S(0,1,2,3,4))
  ));

  const auto d2 = -S(0,1,0,2,0,3,4,5,6);  // `m` in the old notation
  EXPECT_EXPR_ZERO(normalize(
    - d2
    + tp(Q(0,1,5,6), S(0,1,2,4,5), Q(0,2,3,4))
    + tp(Q(0,1,5,6), Q(0,1,2,5), S(0,2,3,4,5))
    - tp(Q(0,2,3,4), Q(0,1,2,4), S(0,1,4,5,6))
  ));

  const auto d3 = -S(0,1,0,2,3,0,4,5,6);  // `n` in the old notation
  EXPECT_EXPR_ZERO(normalize(
    - d3
    + tp(Q(1,0,3,2), Q(1,0,4,3), S(1,0,4,6,5))
    - tp(Q(0,4,2,3), Q(0,5,1,6), S(0,5,1,4,2))
    - tp(Q(0,5,1,6), Q(0,4,2,3), S(0,4,2,5,1))
    + tp(Q(1,0,3,2), Q(1,0,5,6), S(1,0,5,3,4))
    - tp(Q(1,0,5,6), Q(1,0,3,2), S(1,0,3,5,4))
    - tp(Q(3,0,1,2), Q(3,0,5,4), S(3,0,5,1,6))
    + tp(Q(3,0,1,2), Q(3,0,6,1), S(3,0,6,5,4))
    + tp(Q(3,0,5,4), Q(3,0,1,2), S(3,0,1,5,6))
    - tp(Q(4,0,2,3), Q(4,0,1,2), S(4,0,1,6,5))
    + tp(Q(5,0,1,6), Q(5,0,2,1), S(5,0,2,4,3))
    + tp(Q(5,0,1,6), Q(5,0,3,4), S(5,0,3,1,2))
    - tp(Q(5,0,3,4), Q(5,0,1,6), S(5,0,1,3,2))
  ));

  const auto d4 = -S(0,1,2,0,3,4,0,5,6);  // `o` in the old notation
  EXPECT_EXPR_ZERO(normalize(
    - d4
    + tp(Q(0,3,1,2), Q(0,6,4,5), S(0,6,4,3,1))
    + tp(Q(0,4,2,3), Q(0,5,1,6), S(0,5,1,4,2))
    + tp(Q(0,5,1,6), Q(0,4,2,3), S(0,4,2,5,1))
    + tp(Q(0,5,3,4), Q(0,6,2,1), S(0,6,2,5,3))
    + tp(Q(0,6,2,1), Q(0,5,3,4), S(0,5,3,6,2))
    + tp(Q(0,6,4,5), Q(0,3,1,2), S(0,3,1,6,4))
    + tp(Q(1,0,3,2), Q(1,0,5,6), S(1,0,5,3,4))
    - tp(Q(1,0,5,6), Q(1,0,3,2), S(1,0,3,5,4))
    + tp(Q(1,0,5,6), Q(1,0,4,5), S(1,0,4,3,2))
    + tp(Q(2,0,4,3), Q(2,0,5,4), S(2,0,5,6,1))
    - tp(Q(2,0,4,3), Q(2,0,6,1), S(2,0,6,4,5))
    + tp(Q(2,0,6,1), Q(2,0,4,3), S(2,0,4,6,5))
    - tp(Q(3,0,1,2), Q(3,0,5,4), S(3,0,5,1,6))
    + tp(Q(3,0,1,2), Q(3,0,6,1), S(3,0,6,5,4))
    + tp(Q(3,0,5,4), Q(3,0,1,2), S(3,0,1,5,6))
    + tp(Q(4,0,2,3), Q(4,0,6,5), S(4,0,6,2,1))
    - tp(Q(4,0,6,5), Q(4,0,1,6), S(4,0,1,3,2))
    - tp(Q(4,0,6,5), Q(4,0,2,3), S(4,0,2,6,1))
    + tp(Q(5,0,1,6), Q(5,0,3,4), S(5,0,3,1,2))
    - tp(Q(5,0,3,4), Q(5,0,1,6), S(5,0,1,3,2))
    - tp(Q(5,0,3,4), Q(5,0,2,3), S(5,0,2,6,1))
    - tp(Q(6,0,2,1), Q(6,0,3,2), S(6,0,3,5,4))
    - tp(Q(6,0,2,1), Q(6,0,4,5), S(6,0,4,2,3))
    + tp(Q(6,0,4,5), Q(6,0,2,1), S(6,0,2,4,3))
  ));

  const auto d5 = d2 + d3 - d1 + cycle(d1, {{1,3}, {4,6}});
  EXPECT_EXPR_ZERO(normalize(
    - d5
    + tp(Q(0,1,5,6), S(0,1,2,3,5), Q(0,3,4,5))
    - tp(Q(0,1,5,6), Q(0,1,4,5), S(0,1,2,3,4))
    - tp(Q(0,3,4,5), Q(0,3,5,6), S(0,1,2,3,6))
  ));

  const auto d6 = d2 - cycle(d2, {{1,5}});
  EXPECT_EXPR_ZERO(normalize(
    - d6
    + tp(Q(0,5,1,6), Q(0,5,1,2), S(0,5,2,3,4))
    - tp(Q(0,1,5,6), Q(0,1,5,2), S(0,1,2,3,4))
    - tp(Q(0,4,2,3), Q(0,4,2,1), S(0,4,1,5,6))
    + tp(Q(0,4,2,3), Q(0,4,2,5), S(0,4,5,1,6))
  ));

  const auto d7 = d5 + cycle(d5, {{0,5}});
  EXPECT_EXPR_ZERO(normalize(
    - d7
    + tp(Q(0,1,5,6), Q(0,1,5,4), S(0,1,4,2,3))
    + tp(Q(0,3,5,4), Q(0,3,5,6), S(0,3,6,1,2))
    - tp(Q(1,5,0,6), Q(1,5,0,4), S(1,5,4,2,3))
    - tp(Q(3,5,0,4), Q(3,5,0,6), S(3,5,6,1,2))
  ));

  const auto d8 =
    + d4
    - cycle(d2, {{1,5}, {2,4}})
    - cycle(d2, {{2,6}, {3,5}})
    - cycle(d2, {{1,3}, {4,6}})
    - d1
    - d5
    + cycle(d1, {{1,3}, {4,6}})
    + cycle(d5, {{1,2,3,4,5,6}})
    - cycle(d1, {{1,2}, {3,6}, {4,5}})
    - cycle(d5, {{1,2}, {3,6}, {4,5}})
  ;
  EXPECT_EXPR_ZERO(normalize(
    - d8
    + tp(Q(0,1,3,2), Q(0,1,3,4), S(0,1,4,5,6))
    + tp(Q(0,3,1,2), Q(0,3,1,6), S(0,3,6,4,5))
    + tp(Q(0,4,6,5), Q(0,4,6,1), S(0,4,1,2,3))
    + tp(Q(0,6,4,5), Q(0,6,4,3), S(0,6,3,1,2))
  ));
}
