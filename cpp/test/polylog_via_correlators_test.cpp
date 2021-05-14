#include "lib/polylog_via_correlators.h"

#include "gtest/gtest.h"

#include "lib/iterated_integral.h"
#include "lib/polylog_qli.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "test_util/matchers.h"


DeltaExpr EvalCorrQLi(int weight, const std::vector<int>& points) {
  return eval_formal_symbols(CorrQLi(weight, points));
}
DeltaExpr EvalCorrQLiNeg(int weight, const std::vector<int>& points) {
  return eval_formal_symbols(CorrQLiNeg(weight, points));
}
DeltaExpr EvalCorrQLiNegAlt(int weight, const std::vector<int>& points) {
  return eval_formal_symbols(CorrQLiNegAlt(weight, points));
}
DeltaExpr EvalCorrQLiSymm(int weight, const std::vector<int>& points) {
  return eval_formal_symbols(CorrQLiSymm(weight, points));
}


TEST(CorrQLiTest, EqualToClassicDefinition) {
  // Should be true for any weight and number of args.
  // Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi1(1,2,3,4), EvalCorrQLi(1, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi2(1,2,3,4), EvalCorrQLi(2, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi3(1,2,3,4), EvalCorrQLi(3, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi4(1,2,3,4), EvalCorrQLi(4, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLi5(1,2,3,4)),
                              project_on_x1(EvalCorrQLi(5, {1,2,3,4})));

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi2(1,2,3,4,5,6), EvalCorrQLi(2, {1,2,3,4,5,6}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi3(1,2,3,4,5,6), EvalCorrQLi(3, {1,2,3,4,5,6}));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLi3(1,2,3,4,5,6,7,8)),
                              project_on_x1(EvalCorrQLi(3, {1,2,3,4,5,6,7,8})));
}

TEST(CorrQLiNegTest, EqualToClassicDefinition) {
  // Should be true for any weight and number of args.
  // Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg1(1,2,3,4), EvalCorrQLiNeg(1, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg2(1,2,3,4), EvalCorrQLiNeg(2, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg3(1,2,3,4), EvalCorrQLiNeg(3, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg4(1,2,3,4), EvalCorrQLiNeg(4, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiNeg5(1,2,3,4)),
                              project_on_x1(EvalCorrQLiNeg(5, {1,2,3,4})));

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg2(1,2,3,4,5,6), EvalCorrQLiNeg(2, {1,2,3,4,5,6}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg3(1,2,3,4,5,6), EvalCorrQLiNeg(3, {1,2,3,4,5,6}));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiNeg3(1,2,3,4,5,6,7,8)),
                              project_on_x1(EvalCorrQLiNeg(3, {1,2,3,4,5,6,7,8})));
}

TEST(CorrQLiNegAltTest, EqualToClassicDefinition) {
  // Should be true for any weight and number of args.
  // Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg1(1,2,3,4), EvalCorrQLiNegAlt(1, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg2(1,2,3,4), EvalCorrQLiNegAlt(2, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg3(1,2,3,4), EvalCorrQLiNegAlt(3, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg4(1,2,3,4), EvalCorrQLiNegAlt(4, {1,2,3,4}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiNeg5(1,2,3,4)),
                              project_on_x1(EvalCorrQLiNegAlt(5, {1,2,3,4})));

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg2(1,2,3,4,5,6), EvalCorrQLiNegAlt(2, {1,2,3,4,5,6}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg3(1,2,3,4,5,6), EvalCorrQLiNegAlt(3, {1,2,3,4,5,6}));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiNeg3(1,2,3,4,5,6,7,8)),
                              project_on_x1(EvalCorrQLiNegAlt(3, {1,2,3,4,5,6,7,8})));
}

TEST(CorrQLiSymmTest, EqualToClassicDefinition) {
  // Note:
  //   QLi_w(1,2,3,4) == EvalCorrQLiSymm(w, 4)
  // should be true for w >= 2. Not testing here.
  // With more than 4 args should be true for any weight.
  // Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiSymm2(1,2,3,4,5,6), EvalCorrQLiSymm(2, {1,2,3,4,5,6}));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiSymm3(1,2,3,4,5,6), EvalCorrQLiSymm(3, {1,2,3,4,5,6}));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiSymm3(1,2,3,4,5,6,7,8)),
                              project_on_x1(EvalCorrQLiSymm(3, {1,2,3,4,5,6,7,8})));
}


TEST(CorrQLiSymmTest, GluingTwoArgsReducesWeight) {
  // Should be true for any weight and number of args.
  for (int weight = 1; weight <= 2; weight++) {
    for (int num_args = 4; num_args <= 8; num_args += 2) {
      auto large_args = seq_incl(1, num_args);
      --large_args.back();
      EXPECT_EXPR_EQ_AFTER_LYNDON(
        EvalCorrQLiSymm(weight, large_args),
        -EvalCorrQLiSymm(weight, seq_incl(1, num_args - 2))
      );
    }
  }
}

class SubsetSumFormulaTest : public ::testing::TestWithParam<std::pair<int, int>> {
public:
  int weight() const { return GetParam().first; }
  int total_points() const { return GetParam().second; }
};

// Proves that subset formula for QLi is also true for formal correlator expressions.
TEST_P(SubsetSumFormulaTest, CorrQLiSymm_SubsetSumFormula) {
  CorrExpr expr;
  // Note: starting with 2 args.
  for (int num_args = 2; num_args <= total_points(); num_args += 2) {
    for (const auto& seq : increasing_sequences(total_points(), num_args)) {
      const auto args = mapped(seq, [](int x) { return x + 1; });
      const int sign = neg_one_pow(sum(args) + num_args / 2);
      expr += sign * CorrQLiSymm(weight(), args);
    }
  }
  EXPECT_EXPR_ZERO(expr);
}

INSTANTIATE_TEST_SUITE_P(AllCases, SubsetSumFormulaTest, ::testing::Values(
  std::pair{2, 5},
  std::pair{2, 6},
  std::pair{3, 6},
  std::pair{3, 7}
));

TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg4_FormSymm) {
  for (int w = 1; w <= 2; w++) {
    EXPECT_EXPR_EQ(
      - corr_comultiply(CorrQLi   (2*w, {1,2,3,4}), {w,w}),
      (
        + corr_coproduct(CorrQLi   (w, {1,2}), CorrQLi(w, {1,2,3,4}))
        + corr_coproduct(CorrQLiNeg(w, {2,3}), CorrQLi(w, {1,2,3,4}))
        + corr_coproduct(CorrQLi   (w, {3,4}), CorrQLi(w, {1,2,3,4}))
        + corr_coproduct(CorrQLi   (w, {1,2,3,4}), CorrQLi(w, {1,4}))
      )
    );
  }
}

TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg6_FormSymm) {
  for (int w = 1; w <= 2; w++) {
    EXPECT_EXPR_EQ(
      - corr_comultiply(CorrQLi   (2*w, {1,2,3,4,5,6}), {w,w}),
      (
        + corr_coproduct(CorrQLi   (w, {1,2,3,4}), CorrQLi(w, {1,4,5,6}))
        + corr_coproduct(CorrQLiNeg(w, {2,3,4,5}), CorrQLi(w, {1,2,5,6}))
        + corr_coproduct(CorrQLi   (w, {3,4,5,6}), CorrQLi(w, {1,2,3,6}))
        + corr_coproduct(CorrQLi   (w, {1,2}), CorrQLi(w, {1,2,3,4,5,6}))
        + corr_coproduct(CorrQLiNeg(w, {2,3}), CorrQLi(w, {1,2,3,4,5,6}))
        + corr_coproduct(CorrQLi   (w, {3,4}), CorrQLi(w, {1,2,3,4,5,6}))
        + corr_coproduct(CorrQLiNeg(w, {4,5}), CorrQLi(w, {1,2,3,4,5,6}))
        + corr_coproduct(CorrQLi   (w, {5,6}), CorrQLi(w, {1,2,3,4,5,6}))
        + corr_coproduct(CorrQLi   (w, {1,2,3,4,5,6}), CorrQLi(w, {1,6}))
      )
    );
  }
}

TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg6_Form_1_3) {
  EXPECT_EXPR_EQ(
    - corr_comultiply(CorrQLi   (4, {1,2,3,4,5,6}), {1,3}),
    (
      + corr_coproduct(CorrQLi   (1, {1,2,3,4}), CorrQLi(3, {1,4,5,6}))
      + corr_coproduct(CorrQLiNeg(1, {2,3,4,5}), CorrQLi(3, {1,2,5,6}))
      + corr_coproduct(CorrQLi   (1, {3,4,5,6}), CorrQLi(3, {1,2,3,6}))
      + corr_coproduct(CorrQLi   (3, {1,2,3,4}), CorrQLi(1, {1,4,5,6}))
      + corr_coproduct(CorrQLiNeg(3, {2,3,4,5}), CorrQLi(1, {1,2,5,6}))
      + corr_coproduct(CorrQLi   (3, {3,4,5,6}), CorrQLi(1, {1,2,3,6}))
      + corr_coproduct(CorrQLi   (1, {1,2}), CorrQLi(3, {1,2,3,4,5,6}))
      + corr_coproduct(CorrQLiNeg(1, {2,3}), CorrQLi(3, {1,2,3,4,5,6}))
      + corr_coproduct(CorrQLi   (1, {3,4}), CorrQLi(3, {1,2,3,4,5,6}))
      + corr_coproduct(CorrQLiNeg(1, {4,5}), CorrQLi(3, {1,2,3,4,5,6}))
      + corr_coproduct(CorrQLi   (1, {5,6}), CorrQLi(3, {1,2,3,4,5,6}))
      + corr_coproduct(CorrQLi   (3, {1,2,3,4,5,6}), CorrQLi(1, {1,6}))
    )
  );
}

TEST(CorrFSymbTest, LARGE_FormalSymbolCoEquation_Arg8_Form_2_3) {
  EXPECT_EXPR_EQ(
    - corr_comultiply(CorrQLi   (5, {1,2,3,4,5,6,7,8}), {2,3}),
    (
      + corr_coproduct(CorrQLi   (3, {1,2,3,4}), CorrQLi(2, {1,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(3, {2,3,4,5}), CorrQLi(2, {1,2,5,6,7,8}))
      + corr_coproduct(CorrQLi   (3, {3,4,5,6}), CorrQLi(2, {1,2,3,6,7,8}))
      + corr_coproduct(CorrQLiNeg(3, {4,5,6,7}), CorrQLi(2, {1,2,3,4,7,8}))
      + corr_coproduct(CorrQLi   (3, {5,6,7,8}), CorrQLi(2, {1,2,3,4,5,8}))
      + corr_coproduct(CorrQLi   (3, {1,2,3,4,5,6}), CorrQLi(2, {1,6,7,8}))
      + corr_coproduct(CorrQLiNeg(3, {2,3,4,5,6,7}), CorrQLi(2, {1,2,7,8}))
      + corr_coproduct(CorrQLi   (3, {3,4,5,6,7,8}), CorrQLi(2, {1,2,3,8}))
      + corr_coproduct(CorrQLi   (2, {1,2,3,4}), CorrQLi(3, {1,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(2, {2,3,4,5}), CorrQLi(3, {1,2,5,6,7,8}))
      + corr_coproduct(CorrQLi   (2, {3,4,5,6}), CorrQLi(3, {1,2,3,6,7,8}))
      + corr_coproduct(CorrQLiNeg(2, {4,5,6,7}), CorrQLi(3, {1,2,3,4,7,8}))
      + corr_coproduct(CorrQLi   (2, {5,6,7,8}), CorrQLi(3, {1,2,3,4,5,8}))
      + corr_coproduct(CorrQLi   (2, {1,2,3,4,5,6}), CorrQLi(3, {1,6,7,8}))
      + corr_coproduct(CorrQLiNeg(2, {2,3,4,5,6,7}), CorrQLi(3, {1,2,7,8}))
      + corr_coproduct(CorrQLi   (2, {3,4,5,6,7,8}), CorrQLi(3, {1,2,3,8}))
      + corr_coproduct(CorrQLi   (3, {1,2}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(3, {2,3}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (3, {3,4}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(3, {4,5}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (3, {5,6}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(3, {6,7}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (3, {7,8}), CorrQLi(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (3, {1,2,3,4,5,6,7,8}), CorrQLi(2, {1,8}))
      + corr_coproduct(CorrQLi   (2, {1,2}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(2, {2,3}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (2, {3,4}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(2, {4,5}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (2, {5,6}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLiNeg(2, {6,7}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (2, {7,8}), CorrQLi(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(CorrQLi   (2, {1,2,3,4,5,6,7,8}), CorrQLi(3, {1,8}))
    )
  );
}
