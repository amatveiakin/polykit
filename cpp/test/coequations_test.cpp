#include "gtest/gtest.h"

#include "lib/coalgebra.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_qli.h"
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

// Note: also true for QLi4.
TEST(CoequationsTest, QLiSymm4_OldFormula) {
  EXPECT_EXPR_EQ(
    comultiply(QLiSymm4(1,2,3,4,5,6), {2,2}),
    (
      + coproduct(QLi2(1,2,3,4), QLi2(1,4,5,6))
      - coproduct(QLi2(2,3,4,5), QLi2(2,5,6,1))
      + coproduct(QLi2(3,4,5,6), QLi2(3,6,1,2))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiSymm_Arg6) {
  for (int wr : range_incl(2, 3)) {
    EXPECT_EXPR_EQ(
      comultiply(QLiSymmVec(wr+1, {1,2,3,4,5,6}), {1,wr}),
      (
        + sum_looped_vec([&](const std::vector<X>& args) {
          return coproduct(
            QLiVec    (1,  choose_indices_one_based(args, {1,2,3,4})),
            QLiSymmVec(wr, choose_indices_one_based(args, {1,4,5,6}))
          );
        }, 6, {1,2,3,4,5,6}, (wr % 2 == 0 ? SumSign::plus : SumSign::alternating))
        + coproduct(cross_ratio(std::vector{1,2,3,4,5,6}), QLiSymmVec(wr, {1,2,3,4,5,6}))
      )
    );
  }
}

TEST(CoequationsTest, QLi_Arg6) {
  for (int w : range_incl(1, 2)) {
    EXPECT_EXPR_EQ(
      comultiply(QLiVec(2*w, {1,2,3,4,5,6}), {w,w}),
      (
        + coproduct(QLiVec   (w, {1,2,3,4}), QLiVec(w, {1,4,5,6}))
        + coproduct(QLiNegVec(w, {2,3,4,5}), QLiVec(w, {1,2,5,6}))
        + coproduct(QLiVec   (w, {3,4,5,6}), QLiVec(w, {1,2,3,6}))
      )
    );
  }
}

TEST(CoequationsTest, QLiNeg_Arg6) {
  for (int w : range_incl(1, 2)) {
    EXPECT_EXPR_EQ(
      comultiply(QLiNegVec(2*w, {1,2,3,4,5,6}), {w,w}),
      (
        + coproduct(QLiNegVec(w, {1,2,3,4}), QLiNegVec(w, {1,4,5,6}))
        + coproduct(QLiVec   (w, {2,3,4,5}), QLiNegVec(w, {1,2,5,6}))
        + coproduct(QLiNegVec(w, {3,4,5,6}), QLiNegVec(w, {1,2,3,6}))
      )
    );
  }
}

TEST(CoequationsTest, QLi3_Arg6) {
  EXPECT_EXPR_EQ(
    comultiply(QLi3(1,2,3,4,5,6), {1,2}),
    (
      + coproduct(QLi1   (1,2,3,4), QLi2(1,4,5,6))
      + coproduct(QLiNeg1(2,3,4,5), QLi2(1,2,5,6))
      + coproduct(QLi1   (3,4,5,6), QLi2(1,2,3,6))
      + coproduct(QLi2   (1,2,3,4), QLi1(1,4,5,6))
      + coproduct(QLiNeg2(2,3,4,5), QLi1(1,2,5,6))
      + coproduct(QLi2   (3,4,5,6), QLi1(1,2,3,6))
      + coproduct(QLi1   (1,2), QLi2(1,2,3,4,5,6))
      + coproduct(QLiNeg1(2,3), QLi2(1,2,3,4,5,6))
      + coproduct(QLi1   (3,4), QLi2(1,2,3,4,5,6))
      + coproduct(QLiNeg1(4,5), QLi2(1,2,3,4,5,6))
      + coproduct(QLi1   (5,6), QLi2(1,2,3,4,5,6))
      + coproduct(QLi2(1,2,3,4,5,6), QLi1   (1,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi4_Arg6_Form1_3) {
  EXPECT_EXPR_EQ(
    comultiply(QLi4(1,2,3,4,5,6), {1,3}),
    (
      + coproduct(QLi1   (1,2,3,4), QLi3(1,4,5,6))
      + coproduct(QLiNeg1(2,3,4,5), QLi3(1,2,5,6))
      + coproduct(QLi1   (3,4,5,6), QLi3(1,2,3,6))
      + coproduct(QLi3   (1,2,3,4), QLi1(1,4,5,6))
      + coproduct(QLiNeg3(2,3,4,5), QLi1(1,2,5,6))
      + coproduct(QLi3   (3,4,5,6), QLi1(1,2,3,6))
      + coproduct(QLi1   (1,2), QLi3(1,2,3,4,5,6))
      + coproduct(QLiNeg1(2,3), QLi3(1,2,3,4,5,6))
      + coproduct(QLi1   (3,4), QLi3(1,2,3,4,5,6))
      + coproduct(QLiNeg1(4,5), QLi3(1,2,3,4,5,6))
      + coproduct(QLi1   (5,6), QLi3(1,2,3,4,5,6))
      + coproduct(QLi3(1,2,3,4,5,6), QLi1   (1,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiNeg4_Arg6_Form1_3) {
  EXPECT_EXPR_EQ(
    comultiply(QLiNeg4(1,2,3,4,5,6), {1,3}),
    (
      + coproduct(QLiNeg1(1,2,3,4), QLiNeg3(1,4,5,6))
      + coproduct(QLi1   (2,3,4,5), QLiNeg3(1,2,5,6))
      + coproduct(QLiNeg1(3,4,5,6), QLiNeg3(1,2,3,6))
      + coproduct(QLiNeg3(1,2,3,4), QLiNeg1(1,4,5,6))
      + coproduct(QLi3   (2,3,4,5), QLiNeg1(1,2,5,6))
      + coproduct(QLiNeg3(3,4,5,6), QLiNeg1(1,2,3,6))
      + coproduct(QLiNeg1(1,2), QLiNeg3(1,2,3,4,5,6))
      + coproduct(QLi1   (2,3), QLiNeg3(1,2,3,4,5,6))
      + coproduct(QLiNeg1(3,4), QLiNeg3(1,2,3,4,5,6))
      + coproduct(QLi1   (4,5), QLiNeg3(1,2,3,4,5,6))
      + coproduct(QLiNeg1(5,6), QLiNeg3(1,2,3,4,5,6))
      + coproduct(QLiNeg3(1,2,3,4,5,6), QLiNeg1(1,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi4_Arg8) {
  EXPECT_EXPR_EQ(
    comultiply(QLi4(1,2,3,4,5,6,7,8), {2,2}),
    (
      + coproduct(QLi2   (1,2,3,4), QLi2(1,4,5,6,7,8))
      + coproduct(QLiNeg2(2,3,4,5), QLi2(1,2,5,6,7,8))
      + coproduct(QLi2   (3,4,5,6), QLi2(1,2,3,6,7,8))
      + coproduct(QLiNeg2(4,5,6,7), QLi2(1,2,3,4,7,8))
      + coproduct(QLi2   (5,6,7,8), QLi2(1,2,3,4,5,8))
      + coproduct(QLi2   (1,2,3,4,5,6), QLi2(1,6,7,8))
      + coproduct(QLiNeg2(2,3,4,5,6,7), QLi2(1,2,7,8))
      + coproduct(QLi2   (3,4,5,6,7,8), QLi2(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi5_Arg6) {
  EXPECT_EXPR_EQ(
    comultiply(QLi5(1,2,3,4,5,6), {2,3}),
    (
      + coproduct(QLi2   (1,2,3,4), QLi3(1,4,5,6))
      + coproduct(QLiNeg2(2,3,4,5), QLi3(1,2,5,6))
      + coproduct(QLi2   (3,4,5,6), QLi3(1,2,3,6))
      + coproduct(QLi3   (1,2,3,4), QLi2(1,4,5,6))
      + coproduct(QLiNeg3(2,3,4,5), QLi2(1,2,5,6))
      + coproduct(QLi3   (3,4,5,6), QLi2(1,2,3,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi5_Arg8) {
  EXPECT_EXPR_EQ(
    comultiply(QLi5(1,2,3,4,5,6,7,8), {2,3}),
    (
      + coproduct(QLi3   (1,2,3,4), QLi2(1,4,5,6,7,8))
      + coproduct(QLiNeg3(2,3,4,5), QLi2(1,2,5,6,7,8))
      + coproduct(QLi3   (3,4,5,6), QLi2(1,2,3,6,7,8))
      + coproduct(QLiNeg3(4,5,6,7), QLi2(1,2,3,4,7,8))
      + coproduct(QLi3   (5,6,7,8), QLi2(1,2,3,4,5,8))
      + coproduct(QLi3   (1,2,3,4,5,6), QLi2(1,6,7,8))
      + coproduct(QLiNeg3(2,3,4,5,6,7), QLi2(1,2,7,8))
      + coproduct(QLi3   (3,4,5,6,7,8), QLi2(1,2,3,8))
      + coproduct(QLi2   (1,2,3,4), QLi3(1,4,5,6,7,8))
      + coproduct(QLiNeg2(2,3,4,5), QLi3(1,2,5,6,7,8))
      + coproduct(QLi2   (3,4,5,6), QLi3(1,2,3,6,7,8))
      + coproduct(QLiNeg2(4,5,6,7), QLi3(1,2,3,4,7,8))
      + coproduct(QLi2   (5,6,7,8), QLi3(1,2,3,4,5,8))
      + coproduct(QLi2   (1,2,3,4,5,6), QLi3(1,6,7,8))
      + coproduct(QLiNeg2(2,3,4,5,6,7), QLi3(1,2,7,8))
      + coproduct(QLi2   (3,4,5,6,7,8), QLi3(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiNeg5_Arg8) {
  EXPECT_EXPR_EQ(
    comultiply(QLiNeg5(1,2,3,4,5,6,7,8), {2,3}),
    (
      + coproduct(QLiNeg3(1,2,3,4), QLiNeg2(1,4,5,6,7,8))
      + coproduct(QLi3   (2,3,4,5), QLiNeg2(1,2,5,6,7,8))
      + coproduct(QLiNeg3(3,4,5,6), QLiNeg2(1,2,3,6,7,8))
      + coproduct(QLi3   (4,5,6,7), QLiNeg2(1,2,3,4,7,8))
      + coproduct(QLiNeg3(5,6,7,8), QLiNeg2(1,2,3,4,5,8))
      + coproduct(QLiNeg3(1,2,3,4,5,6), QLiNeg2(1,6,7,8))
      + coproduct(QLi3   (2,3,4,5,6,7), QLiNeg2(1,2,7,8))
      + coproduct(QLiNeg3(3,4,5,6,7,8), QLiNeg2(1,2,3,8))
      + coproduct(QLiNeg2(1,2,3,4), QLiNeg3(1,4,5,6,7,8))
      + coproduct(QLi2   (2,3,4,5), QLiNeg3(1,2,5,6,7,8))
      + coproduct(QLiNeg2(3,4,5,6), QLiNeg3(1,2,3,6,7,8))
      + coproduct(QLi2   (4,5,6,7), QLiNeg3(1,2,3,4,7,8))
      + coproduct(QLiNeg2(5,6,7,8), QLiNeg3(1,2,3,4,5,8))
      + coproduct(QLiNeg2(1,2,3,4,5,6), QLiNeg3(1,6,7,8))
      + coproduct(QLi2   (2,3,4,5,6,7), QLiNeg3(1,2,7,8))
      + coproduct(QLiNeg2(3,4,5,6,7,8), QLiNeg3(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi6_Arg8) {
  EXPECT_EXPR_EQ(
    comultiply(QLi6(1,2,3,4,5,6,7,8), {3,3}),
    (
      + coproduct(QLi3   (1,2,3,4), QLi3(1,4,5,6,7,8))
      + coproduct(QLiNeg3(2,3,4,5), QLi3(1,2,5,6,7,8))
      + coproduct(QLi3   (3,4,5,6), QLi3(1,2,3,6,7,8))
      + coproduct(QLiNeg3(4,5,6,7), QLi3(1,2,3,4,7,8))
      + coproduct(QLi3   (5,6,7,8), QLi3(1,2,3,4,5,8))
      + coproduct(QLi3   (1,2,3,4,5,6), QLi3(1,6,7,8))
      + coproduct(QLiNeg3(2,3,4,5,6,7), QLi3(1,2,7,8))
      + coproduct(QLi3   (3,4,5,6,7,8), QLi3(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiSymm5) {
  EXPECT_EXPR_EQ(
    comultiply(QLiSymm5(1,2,3,4,5,6), {2,3}),
    sum_looped(
      [](X x1, X x2, X x3, X x4, X x5, X x6) {
        return coproduct(QLi3(x1,x2,x3,x4), QLi2(x1,x4,x5,x6));
      },
      6
    )
  );
}
TEST(CoequationsTest, LARGE_QLiSymm6) {
  EXPECT_EXPR_EQ(
    comultiply(QLiSymm6(1,2,3,4,5,6), {3,3}),
    (
      + coproduct(QLi3(1,2,3,4), QLi3(1,4,5,6))
      - coproduct(QLi3(2,3,4,5), QLi3(2,5,6,1))
      + coproduct(QLi3(3,4,5,6), QLi3(3,6,1,2))
    )
  );
}
