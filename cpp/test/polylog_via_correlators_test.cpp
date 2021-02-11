#include "lib/polylog_via_correlators.h"

#include "gtest/gtest.h"

#include "lib/polylog_qli.h"
#include "lib/projection.h"
#include "test_util/matchers.h"


TEST(QLiViaCorrTest, EqualsToClassicDefinition) {
  // Should be true for any weight and number of args.
  // Note. Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi1(1,2,3,4), QLiViaCorr(1, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi2(1,2,3,4), QLiViaCorr(2, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi3(1,2,3,4), QLiViaCorr(3, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi4(1,2,3,4), QLiViaCorr(4, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLi5(1,2,3,4)),
                              project_on_x1(QLiViaCorr(5, 4)));

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi2(1,2,3,4,5,6), QLiViaCorr(2, 6));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLi3(1,2,3,4,5,6), QLiViaCorr(3, 6));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLi3(1,2,3,4,5,6,7,8)),
                              project_on_x1(QLiViaCorr(3, 8)));
}

TEST(QLiNegViaCorrTest, EqualsToClassicDefinition) {
  // Should be true for any weight and number of args.
  // Note. Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg1(1,2,3,4), QLiNegViaCorr(1, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg2(1,2,3,4), QLiNegViaCorr(2, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg3(1,2,3,4), QLiNegViaCorr(3, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg4(1,2,3,4), QLiNegViaCorr(4, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiNeg5(1,2,3,4)),
                              project_on_x1(QLiNegViaCorr(5, 4)));

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg2(1,2,3,4,5,6), QLiNegViaCorr(2, 6));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiNeg3(1,2,3,4,5,6), QLiNegViaCorr(3, 6));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiNeg3(1,2,3,4,5,6,7,8)),
                              project_on_x1(QLiNegViaCorr(3, 8)));
}

TEST(QLiSymmViaCorrTest, EqualsToClassicDefinition) {
  // Note:
  //   QLi_w(1,2,3,4) == QLiSymmViaCorr(w, 4)
  // should be true for w >= 2. Not testing here.
  // With 6 or 8 args should be true for any weight.
  // Note. Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiSymm2(1,2,3,4,5,6), QLiSymmViaCorr(2, 6));
  EXPECT_EXPR_EQ_AFTER_LYNDON(QLiSymm3(1,2,3,4,5,6), QLiSymmViaCorr(3, 6));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(QLiSymm3(1,2,3,4,5,6,7,8)),
                              project_on_x1(QLiSymmViaCorr(3, 8)));
}

TEST(QLiSymmViaCorrTest, GluingTwoArgsReducesWeight) {
  // Should be true for any weight and number of args.
  for (int weight = 1; weight <= 2; weight++) {
    for (int num_args = 4; num_args <= 8; num_args += 2) {
      auto large_args = seq_incl(1, num_args);
      --large_args.back();
      EXPECT_EXPR_EQ_AFTER_LYNDON(
        delta_expr_substitute(
          QLiSymmViaCorr(weight, num_args),
          mapped(large_args, X::Var)
        ),
        -QLiSymmViaCorr(weight, num_args - 2)
      );
    }
  }
}

TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg4_FormSymm) {
  for (int w = 1; w <= 2; w++) {
    EXPECT_EXPR_EQ(
      - corr_comultiply(PosCorrFSymb(2*w, {1,2,3,4}), {w,w}),
      (
        + corr_coproduct(PosCorrFSymb(w, {1,2}), PosCorrFSymb(w, {1,2,3,4}))
        + corr_coproduct(NegCorrFSymb(w, {2,3}), PosCorrFSymb(w, {1,2,3,4}))
        + corr_coproduct(PosCorrFSymb(w, {3,4}), PosCorrFSymb(w, {1,2,3,4}))
        + corr_coproduct(PosCorrFSymb(w, {1,2,3,4}), PosCorrFSymb(w, {1,4}))
      )
    );
  }
}

TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg6_FormSymm) {
  for (int w = 1; w <= 2; w++) {
    EXPECT_EXPR_EQ(
      - corr_comultiply(PosCorrFSymb(2*w, {1,2,3,4,5,6}), {w,w}),
      (
        + corr_coproduct(PosCorrFSymb(w, {1,2,3,4}), PosCorrFSymb(w, {1,4,5,6}))
        + corr_coproduct(NegCorrFSymb(w, {2,3,4,5}), PosCorrFSymb(w, {1,2,5,6}))
        + corr_coproduct(PosCorrFSymb(w, {3,4,5,6}), PosCorrFSymb(w, {1,2,3,6}))
        + corr_coproduct(PosCorrFSymb(w, {1,2}), PosCorrFSymb(w, {1,2,3,4,5,6}))
        + corr_coproduct(NegCorrFSymb(w, {2,3}), PosCorrFSymb(w, {1,2,3,4,5,6}))
        + corr_coproduct(PosCorrFSymb(w, {3,4}), PosCorrFSymb(w, {1,2,3,4,5,6}))
        + corr_coproduct(NegCorrFSymb(w, {4,5}), PosCorrFSymb(w, {1,2,3,4,5,6}))
        + corr_coproduct(PosCorrFSymb(w, {5,6}), PosCorrFSymb(w, {1,2,3,4,5,6}))
        + corr_coproduct(PosCorrFSymb(w, {1,2,3,4,5,6}), PosCorrFSymb(w, {1,6}))
      )
    );
  }
}

TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg6_Form_1_3) {
  EXPECT_EXPR_EQ(
    - corr_comultiply(PosCorrFSymb(4, {1,2,3,4,5,6}), {1,3}),
    (
      + corr_coproduct(PosCorrFSymb(1, {1,2,3,4}), PosCorrFSymb(3, {1,4,5,6}))
      + corr_coproduct(NegCorrFSymb(1, {2,3,4,5}), PosCorrFSymb(3, {1,2,5,6}))
      + corr_coproduct(PosCorrFSymb(1, {3,4,5,6}), PosCorrFSymb(3, {1,2,3,6}))
      + corr_coproduct(PosCorrFSymb(3, {1,2,3,4}), PosCorrFSymb(1, {1,4,5,6}))
      + corr_coproduct(NegCorrFSymb(3, {2,3,4,5}), PosCorrFSymb(1, {1,2,5,6}))
      + corr_coproduct(PosCorrFSymb(3, {3,4,5,6}), PosCorrFSymb(1, {1,2,3,6}))
      + corr_coproduct(PosCorrFSymb(1, {1,2}), PosCorrFSymb(3, {1,2,3,4,5,6}))
      + corr_coproduct(NegCorrFSymb(1, {2,3}), PosCorrFSymb(3, {1,2,3,4,5,6}))
      + corr_coproduct(PosCorrFSymb(1, {3,4}), PosCorrFSymb(3, {1,2,3,4,5,6}))
      + corr_coproduct(NegCorrFSymb(1, {4,5}), PosCorrFSymb(3, {1,2,3,4,5,6}))
      + corr_coproduct(PosCorrFSymb(1, {5,6}), PosCorrFSymb(3, {1,2,3,4,5,6}))
      + corr_coproduct(PosCorrFSymb(3, {1,2,3,4,5,6}), PosCorrFSymb(1, {1,6}))
    )
  );
}

#if RUN_LARGE_TESTS
TEST(CorrFSymbTest, FormalSymbolCoEquation_Arg8_Form_2_3) {
  EXPECT_EXPR_EQ(
    - corr_comultiply(PosCorrFSymb(5, {1,2,3,4,5,6,7,8}), {2,3}),
    (
      + corr_coproduct(PosCorrFSymb(3, {1,2,3,4}), PosCorrFSymb(2, {1,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(3, {2,3,4,5}), PosCorrFSymb(2, {1,2,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {3,4,5,6}), PosCorrFSymb(2, {1,2,3,6,7,8}))
      + corr_coproduct(NegCorrFSymb(3, {4,5,6,7}), PosCorrFSymb(2, {1,2,3,4,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {5,6,7,8}), PosCorrFSymb(2, {1,2,3,4,5,8}))
      + corr_coproduct(PosCorrFSymb(3, {1,2,3,4,5,6}), PosCorrFSymb(2, {1,6,7,8}))
      + corr_coproduct(NegCorrFSymb(3, {2,3,4,5,6,7}), PosCorrFSymb(2, {1,2,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {3,4,5,6,7,8}), PosCorrFSymb(2, {1,2,3,8}))
      + corr_coproduct(PosCorrFSymb(2, {1,2,3,4}), PosCorrFSymb(3, {1,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(2, {2,3,4,5}), PosCorrFSymb(3, {1,2,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {3,4,5,6}), PosCorrFSymb(3, {1,2,3,6,7,8}))
      + corr_coproduct(NegCorrFSymb(2, {4,5,6,7}), PosCorrFSymb(3, {1,2,3,4,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {5,6,7,8}), PosCorrFSymb(3, {1,2,3,4,5,8}))
      + corr_coproduct(PosCorrFSymb(2, {1,2,3,4,5,6}), PosCorrFSymb(3, {1,6,7,8}))
      + corr_coproduct(NegCorrFSymb(2, {2,3,4,5,6,7}), PosCorrFSymb(3, {1,2,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {3,4,5,6,7,8}), PosCorrFSymb(3, {1,2,3,8}))
      + corr_coproduct(PosCorrFSymb(3, {1,2}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(3, {2,3}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {3,4}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(3, {4,5}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {5,6}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(3, {6,7}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {7,8}), PosCorrFSymb(2, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(3, {1,2,3,4,5,6,7,8}), PosCorrFSymb(2, {1,8}))
      + corr_coproduct(PosCorrFSymb(2, {1,2}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(2, {2,3}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {3,4}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(2, {4,5}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {5,6}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(NegCorrFSymb(2, {6,7}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {7,8}), PosCorrFSymb(3, {1,2,3,4,5,6,7,8}))
      + corr_coproduct(PosCorrFSymb(2, {1,2,3,4,5,6,7,8}), PosCorrFSymb(3, {1,8}))
    )
  );
}
#endif



#if 0
  for (int weight = 1; weight <= 5; weight++) {
    for (int num_args = 4; num_args <= 10; num_args += 2) {
      if (weight * 2 + 2 < num_args) {
        continue;
      }
      std::cout << "weight " << weight << ", num_args " << num_args << ": ";
      std::cout << prnt::header_only(
        to_lyndon_basis(
          - QLiVec(weight, seq_incl(1, num_args))
          + QLiViaCorr(weight, num_args)
        )
      );
    }
  }
  for (int weight = 1; weight <= 5; weight++) {
    for (int num_args = 4; num_args <= 10; num_args += 2) {
      if (weight * 2 + 2 < num_args) {
        continue;
      }
      std::cout << "weight " << weight << ", num_args " << num_args << ": ";
      std::cout << prnt::header_only(
        to_lyndon_basis(
          - QLiNegVec(weight, seq_incl(1, num_args))
          + QLiNegViaCorr(weight, num_args)
        )
      );
    }
  }
#endif

#if 0
  for (int weight = 1; weight <= 5; weight++) {
    for (int num_args = 4; num_args <= 10; num_args += 2) {
      auto new_args = mapped(concat(seq_incl(2,num_args), std::vector{1}), X::Var);
      auto lhs = to_lyndon_basis(delta_expr_substitute(QLiViaCorr(weight, num_args), new_args));
      auto rhs = to_lyndon_basis(QLiNegViaCorr(weight, num_args));
      auto diff = lhs + rhs;
      std::cout << "weight = " << weight << ", num_args = " << num_args << ", sum = " << prnt::header_only(diff);
    }
  }
#endif
