#include "lib/polylog_via_correlators.h"

#include "gtest/gtest.h"

#include "lib/polylog_cross_ratio.h"
#include "lib/projection.h"
#include "test_util/matchers.h"


TEST(LidoViaCorrTest, EqualsToClassicDefinition) {
  // Should be true for any weight and number of args.
  // Note. Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(Lido1(1,2,3,4), LidoViaCorr(1, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(Lido2(1,2,3,4), LidoViaCorr(2, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(Lido3(1,2,3,4), LidoViaCorr(3, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(Lido4(1,2,3,4), LidoViaCorr(4, 4));
  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(Lido5(1,2,3,4)),
                              project_on_x1(LidoViaCorr(5, 4)));

  EXPECT_EXPR_EQ_AFTER_LYNDON(Lido2(1,2,3,4,5,6), LidoViaCorr(2, 6));
  EXPECT_EXPR_EQ_AFTER_LYNDON(Lido3(1,2,3,4,5,6), LidoViaCorr(3, 6));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(Lido3(1,2,3,4,5,6,7,8)),
                              project_on_x1(LidoViaCorr(3, 8)));
}

TEST(LidoSymmViaCorrTest, EqualsToClassicDefinition) {
  // Note:
  //   Lido_w(1,2,3,4) == LidoSymmViaCorr(w, 4)
  // should be true for w >= 2. Not testing here.
  // With 6 or 8 args should be true for any weight.
  // Note. Projections are used to speed up lyndon.

  EXPECT_EXPR_EQ_AFTER_LYNDON(LidoSymm2(1,2,3,4,5,6), LidoSymmViaCorr(2, 6));
  EXPECT_EXPR_EQ_AFTER_LYNDON(LidoSymm3(1,2,3,4,5,6), LidoSymmViaCorr(3, 6));

  EXPECT_EXPR_EQ_AFTER_LYNDON(project_on_x1(LidoSymm3(1,2,3,4,5,6,7,8)),
                              project_on_x1(LidoSymmViaCorr(3, 8)));
}

TEST(LidoSymmViaCorrTest, GluingTwoArgsReducesWeight) {
  // Should be true for any weight and number of args.
  for (int weight = 1; weight <= 2; weight++) {
    for (int num_args = 4; num_args <= 8; num_args += 2) {
      auto large_args = seq_incl(1, num_args);
      --large_args.back();
      EXPECT_EXPR_EQ_AFTER_LYNDON(
        delta_expr_substitute(
          LidoSymmViaCorr(weight + 1, num_args),
          mapped(large_args, X::Var)
        ),
        -LidoSymmViaCorr(weight + 1, num_args - 2)
      );
    }
  }
}
