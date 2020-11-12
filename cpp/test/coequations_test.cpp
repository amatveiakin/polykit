#include "gtest/gtest.h"

#include "lib/coalgebra.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/summation.h"
#include "test_util/matchers.h"


TEST(CoequationsTest, I_6) {
  EXPECT_EXPR_EQ(
    comultiply(I(1,2,3,4,5,6), {2,2}),
    (
      + coproduct(I(1,2,3,4), I(1,4,5,6))
      + coproduct(I(2,3,4,5), I(1,2,5,6))
      + coproduct(I(3,4,5,6), I(1,2,3,6))
    )
  );
}

// Note: also true for Lido4.
TEST(CoequationsTest, LidoSymm4) {
  EXPECT_EXPR_EQ(
    comultiply(LidoSymm4(1,2,3,4,5,6), {2,2}),
    (
      + coproduct(Lido2(1,2,3,4), Lido2(1,4,5,6))
      - coproduct(Lido2(2,3,4,5), Lido2(2,5,6,1))
      + coproduct(Lido2(3,4,5,6), Lido2(3,6,1,2))
    )
  );
}

#if 0  // Large tests
TEST(CoequationsTest, LidoSymm5) {
  EXPECT_EXPR_EQ(
    comultiply(LidoSymm5(1,2,3,4,5,6), {2,3}),
    sum_looped(
      [](X x1, X x2, X x3, X x4, X x5, X x6) {
        return coproduct(Lido3(x1,x2,x3,x4), Lido2(x1,x4,x5,x6));
      },
      6
    )
  );
}
TEST(CoequationsTest, LidoSymm6) {
  EXPECT_EXPR_EQ(
    comultiply(LidoSymm6(1,2,3,4,5,6), {3,3}),
    (
      + coproduct(Lido3(1,2,3,4), Lido3(1,4,5,6))
      - coproduct(Lido3(2,3,4,5), Lido3(2,5,6,1))
      + coproduct(Lido3(3,4,5,6), Lido3(3,6,1,2))
    )
  );
}
#endif
