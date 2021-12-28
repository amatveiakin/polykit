#include "gtest/gtest.h"

#include "lib/coalgebra.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_qli.h"
#include "lib/summation.h"
#include "test_util/matchers.h"


TEST(CoequationsTest, I_6) {
  EXPECT_EXPR_EQ(
    icomultiply(I(1,2,3,4,5,6), {2,2}),
    (
      + icoproduct(I(1,2,3,4), I(1,4,5,6))
      + icoproduct(I(2,3,4,5), I(1,2,5,6))
      + icoproduct(I(3,4,5,6), I(1,2,3,6))
    )
  );
}

// Note: also true for QLi4.
TEST(CoequationsTest, QLiSymm4_OldFormula) {
  EXPECT_EXPR_EQ(
    icomultiply(QLiSymm4(1,2,3,4,5,6), {2,2}),
    (
      + icoproduct(QLi2(1,2,3,4), QLi2(1,4,5,6))
      - icoproduct(QLi2(2,3,4,5), QLi2(2,5,6,1))
      + icoproduct(QLi2(3,4,5,6), QLi2(3,6,1,2))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiSymm_Arg6) {
  for (int wr : range_incl(2, 3)) {
    EXPECT_EXPR_EQ(
      icomultiply(QLiSymmVec(wr+1, {1,2,3,4,5,6}), {1,wr}),
      (
        + sum_looped_vec([&](const auto& args) {
          return icoproduct(
            QLiVec    (1,  choose_indices_one_based(args, {1,2,3,4})),
            QLiSymmVec(wr, choose_indices_one_based(args, {1,4,5,6}))
          );
        }, 6, {1,2,3,4,5,6}, (wr % 2 == 0 ? SumSign::plus : SumSign::alternating))
        + icoproduct(cross_ratio(std::vector{1,2,3,4,5,6}), QLiSymmVec(wr, {1,2,3,4,5,6}))
      )
    );
  }
}

TEST(CoequationsTest, QLi_Arg6) {
  for (int w : range_incl(1, 2)) {
    EXPECT_EXPR_EQ(
      icomultiply(QLiVec(2*w, {1,2,3,4,5,6}), {w,w}),
      (
        + icoproduct(QLiVec   (w, {1,2,3,4}), QLiVec(w, {1,4,5,6}))
        + icoproduct(QLiNegVec(w, {2,3,4,5}), QLiVec(w, {1,2,5,6}))
        + icoproduct(QLiVec   (w, {3,4,5,6}), QLiVec(w, {1,2,3,6}))
      )
    );
  }
}

TEST(CoequationsTest, QLiNeg_Arg6) {
  for (int w : range_incl(1, 2)) {
    EXPECT_EXPR_EQ(
      icomultiply(QLiNegVec(2*w, {1,2,3,4,5,6}), {w,w}),
      (
        + icoproduct(QLiNegVec(w, {1,2,3,4}), QLiNegVec(w, {1,4,5,6}))
        + icoproduct(QLiVec   (w, {2,3,4,5}), QLiNegVec(w, {1,2,5,6}))
        + icoproduct(QLiNegVec(w, {3,4,5,6}), QLiNegVec(w, {1,2,3,6}))
      )
    );
  }
}

TEST(CoequationsTest, QLi3_Arg6) {
  EXPECT_EXPR_EQ(
    icomultiply(QLi3(1,2,3,4,5,6), {1,2}),
    (
      + icoproduct(QLi1   (1,2,3,4), QLi2(1,4,5,6))
      + icoproduct(QLiNeg1(2,3,4,5), QLi2(1,2,5,6))
      + icoproduct(QLi1   (3,4,5,6), QLi2(1,2,3,6))
      + icoproduct(QLi2   (1,2,3,4), QLi1(1,4,5,6))
      + icoproduct(QLiNeg2(2,3,4,5), QLi1(1,2,5,6))
      + icoproduct(QLi2   (3,4,5,6), QLi1(1,2,3,6))
      + icoproduct(QLi1   (1,2), QLi2(1,2,3,4,5,6))
      + icoproduct(QLiNeg1(2,3), QLi2(1,2,3,4,5,6))
      + icoproduct(QLi1   (3,4), QLi2(1,2,3,4,5,6))
      + icoproduct(QLiNeg1(4,5), QLi2(1,2,3,4,5,6))
      + icoproduct(QLi1   (5,6), QLi2(1,2,3,4,5,6))
      + icoproduct(QLi2(1,2,3,4,5,6), QLi1   (1,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi4_Arg6_Form1_3) {
  EXPECT_EXPR_EQ(
    icomultiply(QLi4(1,2,3,4,5,6), {1,3}),
    (
      + icoproduct(QLi1   (1,2,3,4), QLi3(1,4,5,6))
      + icoproduct(QLiNeg1(2,3,4,5), QLi3(1,2,5,6))
      + icoproduct(QLi1   (3,4,5,6), QLi3(1,2,3,6))
      + icoproduct(QLi3   (1,2,3,4), QLi1(1,4,5,6))
      + icoproduct(QLiNeg3(2,3,4,5), QLi1(1,2,5,6))
      + icoproduct(QLi3   (3,4,5,6), QLi1(1,2,3,6))
      + icoproduct(QLi1   (1,2), QLi3(1,2,3,4,5,6))
      + icoproduct(QLiNeg1(2,3), QLi3(1,2,3,4,5,6))
      + icoproduct(QLi1   (3,4), QLi3(1,2,3,4,5,6))
      + icoproduct(QLiNeg1(4,5), QLi3(1,2,3,4,5,6))
      + icoproduct(QLi1   (5,6), QLi3(1,2,3,4,5,6))
      + icoproduct(QLi3(1,2,3,4,5,6), QLi1   (1,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiNeg4_Arg6_Form1_3) {
  EXPECT_EXPR_EQ(
    icomultiply(QLiNeg4(1,2,3,4,5,6), {1,3}),
    (
      + icoproduct(QLiNeg1(1,2,3,4), QLiNeg3(1,4,5,6))
      + icoproduct(QLi1   (2,3,4,5), QLiNeg3(1,2,5,6))
      + icoproduct(QLiNeg1(3,4,5,6), QLiNeg3(1,2,3,6))
      + icoproduct(QLiNeg3(1,2,3,4), QLiNeg1(1,4,5,6))
      + icoproduct(QLi3   (2,3,4,5), QLiNeg1(1,2,5,6))
      + icoproduct(QLiNeg3(3,4,5,6), QLiNeg1(1,2,3,6))
      + icoproduct(QLiNeg1(1,2), QLiNeg3(1,2,3,4,5,6))
      + icoproduct(QLi1   (2,3), QLiNeg3(1,2,3,4,5,6))
      + icoproduct(QLiNeg1(3,4), QLiNeg3(1,2,3,4,5,6))
      + icoproduct(QLi1   (4,5), QLiNeg3(1,2,3,4,5,6))
      + icoproduct(QLiNeg1(5,6), QLiNeg3(1,2,3,4,5,6))
      + icoproduct(QLiNeg3(1,2,3,4,5,6), QLiNeg1(1,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi4_Arg8) {
  EXPECT_EXPR_EQ(
    icomultiply(QLi4(1,2,3,4,5,6,7,8), {2,2}),
    (
      + icoproduct(QLi2   (1,2,3,4), QLi2(1,4,5,6,7,8))
      + icoproduct(QLiNeg2(2,3,4,5), QLi2(1,2,5,6,7,8))
      + icoproduct(QLi2   (3,4,5,6), QLi2(1,2,3,6,7,8))
      + icoproduct(QLiNeg2(4,5,6,7), QLi2(1,2,3,4,7,8))
      + icoproduct(QLi2   (5,6,7,8), QLi2(1,2,3,4,5,8))
      + icoproduct(QLi2   (1,2,3,4,5,6), QLi2(1,6,7,8))
      + icoproduct(QLiNeg2(2,3,4,5,6,7), QLi2(1,2,7,8))
      + icoproduct(QLi2   (3,4,5,6,7,8), QLi2(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi5_Arg6) {
  EXPECT_EXPR_EQ(
    icomultiply(QLi5(1,2,3,4,5,6), {2,3}),
    (
      + icoproduct(QLi2   (1,2,3,4), QLi3(1,4,5,6))
      + icoproduct(QLiNeg2(2,3,4,5), QLi3(1,2,5,6))
      + icoproduct(QLi2   (3,4,5,6), QLi3(1,2,3,6))
      + icoproduct(QLi3   (1,2,3,4), QLi2(1,4,5,6))
      + icoproduct(QLiNeg3(2,3,4,5), QLi2(1,2,5,6))
      + icoproduct(QLi3   (3,4,5,6), QLi2(1,2,3,6))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi5_Arg8) {
  EXPECT_EXPR_EQ(
    icomultiply(QLi5(1,2,3,4,5,6,7,8), {2,3}),
    (
      + icoproduct(QLi3   (1,2,3,4), QLi2(1,4,5,6,7,8))
      + icoproduct(QLiNeg3(2,3,4,5), QLi2(1,2,5,6,7,8))
      + icoproduct(QLi3   (3,4,5,6), QLi2(1,2,3,6,7,8))
      + icoproduct(QLiNeg3(4,5,6,7), QLi2(1,2,3,4,7,8))
      + icoproduct(QLi3   (5,6,7,8), QLi2(1,2,3,4,5,8))
      + icoproduct(QLi3   (1,2,3,4,5,6), QLi2(1,6,7,8))
      + icoproduct(QLiNeg3(2,3,4,5,6,7), QLi2(1,2,7,8))
      + icoproduct(QLi3   (3,4,5,6,7,8), QLi2(1,2,3,8))
      + icoproduct(QLi2   (1,2,3,4), QLi3(1,4,5,6,7,8))
      + icoproduct(QLiNeg2(2,3,4,5), QLi3(1,2,5,6,7,8))
      + icoproduct(QLi2   (3,4,5,6), QLi3(1,2,3,6,7,8))
      + icoproduct(QLiNeg2(4,5,6,7), QLi3(1,2,3,4,7,8))
      + icoproduct(QLi2   (5,6,7,8), QLi3(1,2,3,4,5,8))
      + icoproduct(QLi2   (1,2,3,4,5,6), QLi3(1,6,7,8))
      + icoproduct(QLiNeg2(2,3,4,5,6,7), QLi3(1,2,7,8))
      + icoproduct(QLi2   (3,4,5,6,7,8), QLi3(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiNeg5_Arg8) {
  EXPECT_EXPR_EQ(
    icomultiply(QLiNeg5(1,2,3,4,5,6,7,8), {2,3}),
    (
      + icoproduct(QLiNeg3(1,2,3,4), QLiNeg2(1,4,5,6,7,8))
      + icoproduct(QLi3   (2,3,4,5), QLiNeg2(1,2,5,6,7,8))
      + icoproduct(QLiNeg3(3,4,5,6), QLiNeg2(1,2,3,6,7,8))
      + icoproduct(QLi3   (4,5,6,7), QLiNeg2(1,2,3,4,7,8))
      + icoproduct(QLiNeg3(5,6,7,8), QLiNeg2(1,2,3,4,5,8))
      + icoproduct(QLiNeg3(1,2,3,4,5,6), QLiNeg2(1,6,7,8))
      + icoproduct(QLi3   (2,3,4,5,6,7), QLiNeg2(1,2,7,8))
      + icoproduct(QLiNeg3(3,4,5,6,7,8), QLiNeg2(1,2,3,8))
      + icoproduct(QLiNeg2(1,2,3,4), QLiNeg3(1,4,5,6,7,8))
      + icoproduct(QLi2   (2,3,4,5), QLiNeg3(1,2,5,6,7,8))
      + icoproduct(QLiNeg2(3,4,5,6), QLiNeg3(1,2,3,6,7,8))
      + icoproduct(QLi2   (4,5,6,7), QLiNeg3(1,2,3,4,7,8))
      + icoproduct(QLiNeg2(5,6,7,8), QLiNeg3(1,2,3,4,5,8))
      + icoproduct(QLiNeg2(1,2,3,4,5,6), QLiNeg3(1,6,7,8))
      + icoproduct(QLi2   (2,3,4,5,6,7), QLiNeg3(1,2,7,8))
      + icoproduct(QLiNeg2(3,4,5,6,7,8), QLiNeg3(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLi6_Arg8) {
  EXPECT_EXPR_EQ(
    icomultiply(QLi6(1,2,3,4,5,6,7,8), {3,3}),
    (
      + icoproduct(QLi3   (1,2,3,4), QLi3(1,4,5,6,7,8))
      + icoproduct(QLiNeg3(2,3,4,5), QLi3(1,2,5,6,7,8))
      + icoproduct(QLi3   (3,4,5,6), QLi3(1,2,3,6,7,8))
      + icoproduct(QLiNeg3(4,5,6,7), QLi3(1,2,3,4,7,8))
      + icoproduct(QLi3   (5,6,7,8), QLi3(1,2,3,4,5,8))
      + icoproduct(QLi3   (1,2,3,4,5,6), QLi3(1,6,7,8))
      + icoproduct(QLiNeg3(2,3,4,5,6,7), QLi3(1,2,7,8))
      + icoproduct(QLi3   (3,4,5,6,7,8), QLi3(1,2,3,8))
    )
  );
}

TEST(CoequationsTest, LARGE_QLiSymm5) {
  EXPECT_EXPR_EQ(
    icomultiply(QLiSymm5(1,2,3,4,5,6), {2,3}),
    sum_looped(
      [](X x1, X x2, X x3, X x4, X x5, X x6) {
        return icoproduct(QLi3(x1,x2,x3,x4), QLi2(x1,x4,x5,x6));
      },
      6
    )
  );
}
TEST(CoequationsTest, LARGE_QLiSymm6) {
  EXPECT_EXPR_EQ(
    icomultiply(QLiSymm6(1,2,3,4,5,6), {3,3}),
    (
      + icoproduct(QLi3(1,2,3,4), QLi3(1,4,5,6))
      - icoproduct(QLi3(2,3,4,5), QLi3(2,5,6,1))
      + icoproduct(QLi3(3,4,5,6), QLi3(3,6,1,2))
    )
  );
}
