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
TEST(CoequationsTest, LidoSymm4_OldFormula) {
  EXPECT_EXPR_EQ(
    comultiply(LidoSymm4(1,2,3,4,5,6), {2,2}),
    (
      + coproduct(Lido2(1,2,3,4), Lido2(1,4,5,6))
      - coproduct(Lido2(2,3,4,5), Lido2(2,5,6,1))
      + coproduct(Lido2(3,4,5,6), Lido2(3,6,1,2))
    )
  );
}

TEST(CoequationsTest, Lido_Arg6) {
  for (int w = 1; w <= 2; ++w) {
    EXPECT_EXPR_EQ(
      comultiply(LidoVec(2*w, std::vector{1,2,3,4,5,6}), {w,w}),
      (
        + coproduct(LidoVec   (w, std::vector{1,2,3,4}), LidoVec(w, std::vector{1,4,5,6}))
        + coproduct(LidoNegVec(w, std::vector{2,3,4,5}), LidoVec(w, std::vector{1,2,5,6}))
        + coproduct(LidoVec   (w, std::vector{3,4,5,6}), LidoVec(w, std::vector{1,2,3,6}))
      )
    );
  }
}

// TODO: Convert the rest of the tests

  // auto lhs = comultiply(Lido4(1,2,3,4,5,6,7,8), {2,2});
  // auto rhs = (
  //   + coproduct(Lido2   (1,2,3,4), Lido2(1,4,5,6,7,8))
  //   + coproduct(LidoNeg2(2,3,4,5), Lido2(1,2,5,6,7,8))
  //   + coproduct(Lido2   (3,4,5,6), Lido2(1,2,3,6,7,8))
  //   + coproduct(LidoNeg2(4,5,6,7), Lido2(1,2,3,4,7,8))
  //   + coproduct(Lido2   (5,6,7,8), Lido2(1,2,3,4,5,8))
  //   + coproduct(Lido2   (1,2,3,4,5,6), Lido2(1,6,7,8))
  //   + coproduct(LidoNeg2(2,3,4,5,6,7), Lido2(1,2,7,8))
  //   + coproduct(Lido2   (3,4,5,6,7,8), Lido2(1,2,3,8))
  // );
  // auto diff = lhs - rhs;

  // auto lhs = comultiply(Lido6(1,2,3,4,5,6,7,8), {3,3});
  // auto rhs = (
  //   + coproduct(Lido3   (1,2,3,4), Lido3(1,4,5,6,7,8))
  //   + coproduct(LidoNeg3(2,3,4,5), Lido3(1,2,5,6,7,8))
  //   + coproduct(Lido3   (3,4,5,6), Lido3(1,2,3,6,7,8))
  //   + coproduct(LidoNeg3(4,5,6,7), Lido3(1,2,3,4,7,8))
  //   + coproduct(Lido3   (5,6,7,8), Lido3(1,2,3,4,5,8))
  //   + coproduct(Lido3   (1,2,3,4,5,6), Lido3(1,6,7,8))
  //   + coproduct(LidoNeg3(2,3,4,5,6,7), Lido3(1,2,7,8))
  //   + coproduct(Lido3   (3,4,5,6,7,8), Lido3(1,2,3,8))
  // );
  // auto diff = lhs - rhs;


  // auto lhs = comultiply(Lido5(1,2,3,4,5,6), {2,3});
  // auto rhs = (
  //   + coproduct(Lido2   (1,2,3,4), Lido3(1,4,5,6))
  //   + coproduct(LidoNeg2(2,3,4,5), Lido3(1,2,5,6))
  //   + coproduct(Lido2   (3,4,5,6), Lido3(1,2,3,6))
  //   + coproduct(Lido3   (1,2,3,4), Lido2(1,4,5,6))
  //   + coproduct(LidoNeg3(2,3,4,5), Lido2(1,2,5,6))
  //   + coproduct(Lido3   (3,4,5,6), Lido2(1,2,3,6))
  // );
  // auto diff = lhs - rhs;


  // auto lhs = comultiply(Lido5(1,2,3,4,5,6,7,8), {2,3});
  // auto rhs = (
  //   + coproduct(Lido3   (1,2,3,4), Lido2(1,4,5,6,7,8))
  //   + coproduct(LidoNeg3(2,3,4,5), Lido2(1,2,5,6,7,8))
  //   + coproduct(Lido3   (3,4,5,6), Lido2(1,2,3,6,7,8))
  //   + coproduct(LidoNeg3(4,5,6,7), Lido2(1,2,3,4,7,8))
  //   + coproduct(Lido3   (5,6,7,8), Lido2(1,2,3,4,5,8))
  //   + coproduct(Lido3   (1,2,3,4,5,6), Lido2(1,6,7,8))
  //   + coproduct(LidoNeg3(2,3,4,5,6,7), Lido2(1,2,7,8))
  //   + coproduct(Lido3   (3,4,5,6,7,8), Lido2(1,2,3,8))
  //   + coproduct(Lido2   (1,2,3,4), Lido3(1,4,5,6,7,8))
  //   + coproduct(LidoNeg2(2,3,4,5), Lido3(1,2,5,6,7,8))
  //   + coproduct(Lido2   (3,4,5,6), Lido3(1,2,3,6,7,8))
  //   + coproduct(LidoNeg2(4,5,6,7), Lido3(1,2,3,4,7,8))
  //   + coproduct(Lido2   (5,6,7,8), Lido3(1,2,3,4,5,8))
  //   + coproduct(Lido2   (1,2,3,4,5,6), Lido3(1,6,7,8))
  //   + coproduct(LidoNeg2(2,3,4,5,6,7), Lido3(1,2,7,8))
  //   + coproduct(Lido2   (3,4,5,6,7,8), Lido3(1,2,3,8))
  // );
  // auto diff = lhs - rhs;


  // auto lhs = comultiply(LidoNeg2(1,2,3,4,5,6), {1,1});
  // auto rhs = (
  //   + coproduct(LidoNeg1(1,2,3,4), LidoNeg1(1,4,5,6))
  //   + coproduct(Lido1   (2,3,4,5), LidoNeg1(1,2,5,6))
  //   + coproduct(LidoNeg1(3,4,5,6), LidoNeg1(1,2,3,6))
  // );
  // auto diff = lhs - rhs;


  // auto lhs = comultiply(LidoNeg5(1,2,3,4,5,6,7,8), {2,3});
  // auto rhs = (
  //   + coproduct(LidoNeg3(1,2,3,4), LidoNeg2(1,4,5,6,7,8))
  //   + coproduct(Lido3   (2,3,4,5), LidoNeg2(1,2,5,6,7,8))
  //   + coproduct(LidoNeg3(3,4,5,6), LidoNeg2(1,2,3,6,7,8))
  //   + coproduct(Lido3   (4,5,6,7), LidoNeg2(1,2,3,4,7,8))
  //   + coproduct(LidoNeg3(5,6,7,8), LidoNeg2(1,2,3,4,5,8))
  //   + coproduct(LidoNeg3(1,2,3,4,5,6), LidoNeg2(1,6,7,8))
  //   + coproduct(Lido3   (2,3,4,5,6,7), LidoNeg2(1,2,7,8))
  //   + coproduct(LidoNeg3(3,4,5,6,7,8), LidoNeg2(1,2,3,8))
  //   + coproduct(LidoNeg2(1,2,3,4), LidoNeg3(1,4,5,6,7,8))
  //   + coproduct(Lido2   (2,3,4,5), LidoNeg3(1,2,5,6,7,8))
  //   + coproduct(LidoNeg2(3,4,5,6), LidoNeg3(1,2,3,6,7,8))
  //   + coproduct(Lido2   (4,5,6,7), LidoNeg3(1,2,3,4,7,8))
  //   + coproduct(LidoNeg2(5,6,7,8), LidoNeg3(1,2,3,4,5,8))
  //   + coproduct(LidoNeg2(1,2,3,4,5,6), LidoNeg3(1,6,7,8))
  //   + coproduct(Lido2   (2,3,4,5,6,7), LidoNeg3(1,2,7,8))
  //   + coproduct(LidoNeg2(3,4,5,6,7,8), LidoNeg3(1,2,3,8))
  // );
  // auto diff = lhs - rhs;


  // auto lhs = comultiply(Lido3(1,2,3,4,5,6), {1,2});
  // auto rhs = (
  //   + coproduct(Lido1   (1,2,3,4), Lido2(1,4,5,6))
  //   + coproduct(LidoNeg1(2,3,4,5), Lido2(1,2,5,6))
  //   + coproduct(Lido1   (3,4,5,6), Lido2(1,2,3,6))
  //   + coproduct(Lido2   (1,2,3,4), Lido1(1,4,5,6))
  //   + coproduct(LidoNeg2(2,3,4,5), Lido1(1,2,5,6))
  //   + coproduct(Lido2   (3,4,5,6), Lido1(1,2,3,6))
  //   + coproduct(Lido1_2p   (1,2), Lido2(1,2,3,4,5,6))
  //   + coproduct(LidoNeg1_2p(2,3), Lido2(1,2,3,4,5,6))
  //   + coproduct(Lido1_2p   (3,4), Lido2(1,2,3,4,5,6))
  //   + coproduct(LidoNeg1_2p(4,5), Lido2(1,2,3,4,5,6))
  //   + coproduct(Lido1_2p   (5,6), Lido2(1,2,3,4,5,6))
  //   + coproduct(Lido2(1,2,3,4,5,6), Lido1_2p   (1,6))
  // );
  // auto diff = lhs - rhs;

  // auto lhs = comultiply(Lido4(1,2,3,4,5,6), {1,3});
  // auto rhs = (
  //   + coproduct(Lido1   (1,2,3,4), Lido3(1,4,5,6))
  //   + coproduct(LidoNeg1(2,3,4,5), Lido3(1,2,5,6))
  //   + coproduct(Lido1   (3,4,5,6), Lido3(1,2,3,6))
  //   + coproduct(Lido3   (1,2,3,4), Lido1(1,4,5,6))
  //   + coproduct(LidoNeg3(2,3,4,5), Lido1(1,2,5,6))
  //   + coproduct(Lido3   (3,4,5,6), Lido1(1,2,3,6))
  //   + coproduct(Lido1_2p   (1,2), Lido3(1,2,3,4,5,6))
  //   + coproduct(LidoNeg1_2p(2,3), Lido3(1,2,3,4,5,6))
  //   + coproduct(Lido1_2p   (3,4), Lido3(1,2,3,4,5,6))
  //   + coproduct(LidoNeg1_2p(4,5), Lido3(1,2,3,4,5,6))
  //   + coproduct(Lido1_2p   (5,6), Lido3(1,2,3,4,5,6))
  //   + coproduct(Lido3(1,2,3,4,5,6), Lido1_2p   (1,6))
  // );
  // auto diff = lhs - rhs;

  // auto lhs = comultiply(LidoNeg4(1,2,3,4,5,6), {1,3});
  // auto rhs = (
  //   + coproduct(LidoNeg1(1,2,3,4), LidoNeg3(1,4,5,6))
  //   + coproduct(Lido1   (2,3,4,5), LidoNeg3(1,2,5,6))
  //   + coproduct(LidoNeg1(3,4,5,6), LidoNeg3(1,2,3,6))
  //   + coproduct(LidoNeg3(1,2,3,4), LidoNeg1(1,4,5,6))
  //   + coproduct(Lido3   (2,3,4,5), LidoNeg1(1,2,5,6))
  //   + coproduct(LidoNeg3(3,4,5,6), LidoNeg1(1,2,3,6))
  //   + coproduct(LidoNeg1_2p(1,2), LidoNeg3(1,2,3,4,5,6))
  //   + coproduct(Lido1_2p   (2,3), LidoNeg3(1,2,3,4,5,6))
  //   + coproduct(LidoNeg1_2p(3,4), LidoNeg3(1,2,3,4,5,6))
  //   + coproduct(Lido1_2p   (4,5), LidoNeg3(1,2,3,4,5,6))
  //   + coproduct(LidoNeg1_2p(5,6), LidoNeg3(1,2,3,4,5,6))
  //   + coproduct(LidoNeg3(1,2,3,4,5,6), LidoNeg1_2p(1,6))
  // );
  // auto diff = lhs - rhs;




#if RUN_LARGE_TESTS
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
