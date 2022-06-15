#include "lib/polylog_qli.h"

#include "gtest/gtest.h"

#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/summation.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


inline ProjectionExpr Pr(std::initializer_list<X> data) {
  return ProjectionExpr::single(data);
}


// Reference A2 comultiplication from https://arxiv.org/pdf/1401.6446.pdf, eq. (3.3)
DeltaICoExpr A2_comult_2_2(const XArgs& args) {
  const auto args_i = [&](int i) {
    return slice(rotated_vector(args.as_x(), 2 * (i-1)), 0, 4);
  };
  DeltaICoExpr ret;
  for (int i : range_incl(1, 5)) {
    for (int j : range_incl(1, 5)) {
      ret += j * icoproduct(
        QLi2(args_i(i)),
        QLi2(args_i(i+j))
      );
    }
  }
  return ret;
}
DeltaICoExpr A2_comult_1_3(const XArgs& args) {
  const auto args_i = [&](int i) {
    return slice(rotated_vector(args.as_x(), 2 * (i-1)), 0, 4);
  };
  DeltaICoExpr ret;
  for (int i : range_incl(1, 5)) {
    ret +=
      + icoproduct(cross_ratio(args_i(i)), QLi3(args_i(i+1)))
      - icoproduct(cross_ratio(args_i(i+1)), QLi3(args_i(i)))
    ;
  }
  ret *= 5;
  return ret;
}


TEST(QLiTest, QLi4_Arg6) {
  auto expr = QLi4(1,2,3,4,5,6);
  EXPECT_EQ(expr.num_terms(), 2560);
  auto lyndon = to_lyndon_basis(expr);
  EXPECT_EQ(lyndon.num_terms(), 1272);
  auto lyndon_then_project = project_on_x1(lyndon);
  auto project_then_lyndon = to_lyndon_basis(project_on_x1(expr));
  EXPECT_EXPR_EQ(lyndon_then_project, project_then_lyndon);
  EXPECT_EXPR_EQ(
    terms_with_min_distinct_variables(lyndon_then_project, 4)
    ,
    + Pr({2, 3, 4, 5})
    - Pr({2, 3, 4, 6})
    + Pr({2, 3, 5, 6})
    - Pr({2, 4, 5, 6})
    + Pr({3, 4, 5, 6})
  );
}

TEST(QLiTest, QLiSymmEquation_Arg6) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + QLiSymm2(1,2,3,4,5,6)
    + QLiSymm2(2,3,4,5,6,1)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + QLiSymm3(1,2,3,4,5,6)
    - QLiSymm3(2,3,4,5,6,1)
  );
}

TEST(QLiTest, QLiSymmEquation_Arg8) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + QLiSymm3(1,2,3,4,5,6,7,8)
    - QLiSymm3(2,3,4,5,6,7,8,1)
  );
}

TEST(QLiTest, LARGE_QLiSymmEquation_Arg10) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(project_on_x1(
    + QLiSymm4(1,2,3,4,5,6,7,8,9,10)
    + QLiSymm4(2,3,4,5,6,7,8,9,10,1)
  ));
}

TEST(QLiTest, QLiSymm_Arg8_AlternativeFormula) {
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + QLiSymm3(1,2,3,4,5,6,7,8)
    ,
    + QLi3(1,2,3,4,5,6,7,8)
    + (
      - QLiSymm3(1,2,3,4,5,6)
      - QLiSymm3(3,4,5,6,7,8)
      - QLiSymm3(5,6,7,8,1,2)
      - QLiSymm3(7,8,1,2,3,4)
      - QLi3(1,2,3,4,5,6)
      - QLi3(3,4,5,6,7,8)
      - QLi3(5,6,7,8,1,2)
      - QLi3(7,8,1,2,3,4)
    ).dived_int(2)
  );
}

TEST(QLiTest, QLi_Arg6_ShiftedDiffFormula) {
  for (int w : range_incl(2, 3)) {
    const int s = neg_one_pow(w);
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      +  QLiVec(w, {1,2,3,4,5,6})
      +s*QLiVec(w, {2,3,4,5,6,1})
      ,
      + QLiVec(w, {1,2,3,4})
      - QLiVec(w, {3,4,5,2})
      + QLiVec(w, {3,4,5,6})
      - QLiVec(w, {5,6,1,4})
      + QLiVec(w, {5,6,1,2})
      - QLiVec(w, {1,2,3,6})
    );
  }
}

TEST(QLiTest, LARGE_QLi_Arg8_ShiftedDiffFormula) {
  for (int w : range_incl(3, 4)) {
    const int s = neg_one_pow(w);
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      +  QLiVec(w, {1,2,3,4,5,6,7,8})
      +s*QLiVec(w, {2,3,4,5,6,7,8,1})
      ,
      - QLiVec(w, {1,2,3,4,5,8})
      + QLiVec(w, {1,2,3,4,5,6})
      - QLiVec(w, {3,4,5,6,7,2})
      + QLiVec(w, {3,4,5,6,7,8})
      - QLiVec(w, {5,6,7,8,1,4})
      + QLiVec(w, {5,6,7,8,1,2})
      - QLiVec(w, {7,8,1,2,3,6})
      + QLiVec(w, {7,8,1,2,3,4})
      + QLiVec(w, {1,2,3,6})
      - QLiVec(w, {1,2,3,8})
      - QLiVec(w, {1,2,5,6})
      + QLiVec(w, {1,2,5,8})
      + QLiVec(w, {1,4,5,6})
      - QLiVec(w, {1,4,5,8})
      + QLiVec(w, {1,4,7,8})
      - QLiVec(w, {1,6,7,8})
      - QLiVec(w, {3,4,5,2})
      + QLiVec(w, {3,4,7,2})
      + QLiVec(w, {3,4,5,8})
      - QLiVec(w, {3,4,7,8})
      - QLiVec(w, {3,6,7,2})
      + QLiVec(w, {3,6,7,8})
      + QLiVec(w, {5,6,7,2})
      - QLiVec(w, {5,6,7,4})
    );
  }
}


class SubsetSumFormulaTest : public ::testing::TestWithParam<std::pair<int, int>> {
public:
  int weight() const { return GetParam().first; }
  int total_points() const { return GetParam().second; }
};

TEST_P(SubsetSumFormulaTest, QLiSymm_SubsetSumFormula) {
  DeltaExpr expr;
  for (int num_args = 4; num_args <= total_points(); num_args += 2) {
    for (const auto& seq : increasing_sequences(total_points(), num_args)) {
      const auto args = mapped(seq, [](int x) { return x + 1; });
      const int sign = neg_one_pow(sum(args) + num_args / 2);
      expr += sign * QLiSymmVec(weight(), args);
    }
  }
  EXPECT_EXPR_ZERO_AFTER_LYNDON(expr);
}

INSTANTIATE_TEST_SUITE_P(Cases, SubsetSumFormulaTest, ::testing::Values(
  std::pair{2, 5},
  std::pair{2, 6},
  std::pair{3, 6}
));
INSTANTIATE_TEST_SUITE_P(LARGE_Cases, SubsetSumFormulaTest, ::testing::Values(
  std::pair{3, 7},
  std::pair{4, 7},
  std::pair{3, 8},
  std::pair{4, 8},
  std::pair{5, 8}
));


TEST(QLiTest, QLi2_Arg4_SuffleDecompose) {
  const auto expr =
    + 2*QLi2(1,2,3,4)
    - 2*QLi2(1,2,3,5)
    + 2*QLi2(1,2,4,5)
    - 2*QLi2(1,3,4,5)
    + 2*QLi2(2,3,4,5)
  ;
  EXPECT_FALSE(expr.is_zero());
  EXPECT_EXPR_ZERO_AFTER_LYNDON(expr);
  EXPECT_EXPR_EQ(
    expr
    ,
    - 2*shuffle_product_expr(QLi1(1,2,3,5), QLi1(1,3,4,5))
    +   shuffle_product_expr(QLi1(1,2,4,5), QLi1(1,2,4,5))
  );
  EXPECT_EXPR_EQ(
    expr
    ,
    + shuffle_product_expr(QLi1(1,2,3,4), Log(1,2,3,4))
    - shuffle_product_expr(QLi1(1,2,3,5), Log(1,2,3,5))
    + shuffle_product_expr(QLi1(1,2,4,5), Log(1,2,4,5))
    - shuffle_product_expr(QLi1(1,3,4,5), Log(1,3,4,5))
    + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
  );
}

TEST(QLiTest, QLi2_Arg6_SuffleDecompose) {
  const auto expr = 2 * (
    + QLi2(1,2,3,4,5,6)
    - QLi2(1,2,3,5)
    + QLi2(1,2,3,6)
    + QLi2(1,2,4,5)
    - QLi2(1,2,4,6)
    - QLi2(1,3,4,5)
    + QLi2(1,3,4,6)
    - QLi2(1,3,5,6)
    + QLi2(1,4,5,6)
    + QLi2(2,3,4,5)
    - QLi2(2,3,4,6)
    + QLi2(2,3,5,6)
    - QLi2(2,4,5,6)
  );
  EXPECT_FALSE(expr.is_zero());
  EXPECT_EXPR_ZERO_AFTER_LYNDON(expr);
  EXPECT_EXPR_EQ(
    expr
    ,
    - shuffle_product_expr(QLi1(1,2,3,5), Log(1,2,3,5))
    + shuffle_product_expr(QLi1(1,2,3,6), Log(1,2,3,6))
    + shuffle_product_expr(QLi1(1,2,4,5), Log(1,2,4,5))
    - shuffle_product_expr(QLi1(1,2,4,6), Log(1,2,4,6))
    - shuffle_product_expr(QLi1(1,3,4,5), Log(1,3,4,5))
    + shuffle_product_expr(QLi1(1,3,4,6), Log(1,3,4,6))
    - shuffle_product_expr(QLi1(1,3,5,6), Log(1,3,5,6))
    + shuffle_product_expr(QLi1(1,4,5,6), Log(1,4,5,6))
    + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
    - shuffle_product_expr(QLi1(2,3,4,6), Log(2,3,4,6))
    + shuffle_product_expr(QLi1(2,3,5,6), Log(2,3,5,6))
    - shuffle_product_expr(QLi1(2,4,5,6), Log(2,4,5,6))
    + shuffle_product_expr(QLi1(1,2,3,4), QLi1(1,4,5,6))
    - shuffle_product_expr(QLi1(1,2,5,6), QLi1(3,4,5,2))
    + shuffle_product_expr(QLi1(3,4,5,6), QLi1(1,2,3,6))
  );
}


#if ENABLE_NEGATIVE_DELTA_VARIABLES
TEST(QLiTest, KummersEquation) {
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + QLiSymm3( x1, x2, x3,-x1,-x2,-x3)
    ,
    + QLiSymm3( x1, x2,-x3, x3)
    + QLiSymm3( x2, x3, x1,-x1)
    + QLiSymm3( x3,-x1, x2,-x2)
    + QLiSymm3(-x1,-x2, x3,-x3)
    + QLiSymm3(-x2,-x3,-x1, x1)
    + QLiSymm3(-x3, x1,-x2, x2)
  );
}

TEST(QLiTest, QLiSymm3_Arg4_EquationDerivedFromKummers) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    -2*QLiSymm3( x1, x2,-x1, x3)
    -2*QLiSymm3( x1, x2, x3,-x2)
    -2*QLiSymm3( x3, x1,-x3, x2)
    -2*QLiSymm3( x1, x2,-x1,-x3)
    -2*QLiSymm3( x2, x3,-x2,-x1)
    -2*QLiSymm3( x1, x3,-x2,-x3)
    +  QLiSymm3( x1, x2,-x1,-x2)
    +  QLiSymm3( x1, x3,-x1,-x3)
    +  QLiSymm3( x2, x3,-x2,-x3)
  );
}

TEST(QLiTest, QLi4_InvolutionEquation) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    +2*QLi4( x1,-x3, x1,-x2, x3, x2)
    -2*QLi4(-x3,-x2, x1, x3,-x1,-x2)
    -  QLi4( x1, x2,-x1,-x2)
    -2*QLi4( x1, x3, x2,-x3)
    +  QLi4( x1, x3,-x1,-x3)
    -2*QLi4( x1,-x2,-x1,-x3)
    +2*QLi4( x2, x3,-x2,-x1)
    -  QLi4( x2, x3,-x2,-x3)
    +2*QLi4( x2,-x1, x3,-x3)
    +2*QLi4( x3,-x1,-x2,-x3)
  );
}

TEST(QLiTest, QLi4_Arg4_InvolutionComultiplication) {
  EXPECT_EXPR_EQ(
    filter_coexpr(
      icomultiply(QLi4(x1,x3,-x1,-x3), {1,3}),
      0, {Delta(x1,x3)}
    ),
    2 * icoproduct(
      DeltaExpr::single({Delta(x1,x3)}),
      + QLi3(x1,x3,-x1,-x3)
    )
  );
}

TEST(QLiTest, QLi4_Arg6_InvolutionComultiplication_1_3) {
  EXPECT_EXPR_EQ(
    filter_coexpr(
      icomultiply(QLi4(x1,-x3,x1,-x2,x3,x2), {1,3}),
      0, {Delta(x1,x3)}
    ),
    icoproduct(
      DeltaExpr::single({Delta(x1,x3)})
      ,
      + QLi3(-x3,x1,-x2,x3)
      - QLi3(x1,-x3,x3,x2)
    )
  );
}

TEST(QLiTest, QLi4_Arg6_InvolutionComultiplication_2_2) {
  EXPECT_EXPR_ZERO(
    icomultiply(
      + QLi4(-x1,x3,x1,x2,-x1,Inf)
      - QLi4(x1,x3,-x1,x2,x1,Inf)
      ,
      {2,2}
    )
  );
}
#endif


TEST(QLiTest, A2_DihedralSymmetry) {
  EXPECT_EXPR_EQ(
    + A2(1,2,3,4,5),
    + A2(2,3,4,5,1)
  );
  EXPECT_EXPR_EQ(
    + A2(1,2,3,4,5),
    - A2(5,4,3,2,1)
  );
}

TEST(QLiTest, A2_Comultiplication_2_2) {
  EXPECT_EXPR_EQ(
    icomultiply(A2(1,2,3,4,5), {2,2}),
    A2_comult_2_2({1,2,3,4,5}).dived_int(5) * 4
  );
}

TEST(QLiTest, A2_Comultiplication_1_3) {
  EXPECT_EXPR_EQ(
    icomultiply(A2(1,2,3,4,5), {1,3}),
    A2_comult_1_3({1,2,3,4,5}).dived_int(5) * 4
  );
}


TEST(QLiTest, QLiBuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x1(QLi3(1,2,3,4,5,6)),
    QLiVecPr(3, {1,2,3,4,5,6}, project_on_x1)
  );
  EXPECT_EXPR_EQ(
    project_on_x7(QLi3(1,7,3,4,5,6,7,8)),
    QLiVecPr(3, {1,7,3,4,5,6,7,8}, project_on_x7)
  );
}

TEST(QLiTest, QLiSymmBuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x4(QLiSymm3(1,2,3,4,5,6)),
    QLiSymmVecPr(3, {1,2,3,4,5,6}, project_on_x4)
  );
}


TEST(QLiTest, LARGE_QLiSymm4_Arg6_AsCyclicSums) {
  const auto z = QLiSymm4(1,2,3,4,5,6);
  const auto a = sum_looped(DISAMBIGUATE(A2), 6, std::array{1,2,3,4,5}, SumSign::alternating);
  const auto b = sum_looped(DISAMBIGUATE(QLi4), 6, std::array{1,2,3,4}, SumSign::alternating);
  const auto c = sum_looped(DISAMBIGUATE(QLi4), 6, std::array{1,2,3,5}, SumSign::alternating);
  const auto expr = 8*z - a + 8*b - 4*c;
  EXPECT_EXPR_ZERO_AFTER_LYNDON(project_on_x1(expr));
}

TEST(QLiTest, HUGE_QLiSymm6_Arg8_AsCyclicSums) {
  static const auto qli6sum_a = [](const XArgs& args) {
    return substitute_variables(
      sum_looped(DISAMBIGUATE(QLiSymm6), 8, std::array{1,2,3,4,5,6}, SumSign::alternating),
      args
    );
  };
  static const auto qli6sum_b = [](const XArgs& args) {
    return substitute_variables(
      sum_looped(DISAMBIGUATE(QLiSymm6), 8, std::array{1,2,3,4,5,7}, SumSign::alternating),
      args
    );
  };
  static const auto qli6sum_c = [](const XArgs& args) {
    return substitute_variables(
      sum_looped(DISAMBIGUATE(QLiSymm6), 8, std::array{1,2,3,4,6,7}, SumSign::alternating),
      args
    );
  };
  static const auto qli6sum_d = [](const XArgs& args) {
    return substitute_variables(
      sum_looped(DISAMBIGUATE(QLiSymm6), 8, std::array{1,2,3,5,6,7}, SumSign::alternating),
      args
    );
  };
  EXPECT_EXPR_ZERO_AFTER_LYNDON(project_on_x1(qli6sum_d({1,2,3,4,5,6,7,8})));

  const auto z = sum_looped(DISAMBIGUATE(QLiSymm6), 9, std::array{1,2,3,4,5,6,7,8});
  const auto a = sum_looped_vec(qli6sum_a, 9, {1,2,3,4,5,6,7,8});
  const auto b = sum_looped_vec(qli6sum_b, 9, {1,2,3,4,5,6,7,8});
  const auto c = sum_looped_vec(qli6sum_c, 9, {1,2,3,4,5,6,7,8});
  const auto expr = 3*z + 3*a - 2*b + c;
  EXPECT_EXPR_ZERO(terms_with_min_distinct_variables(to_lyndon_basis(project_on_x1(expr)), 4));
}

// TODO:  `TEST(QLiTest, LARGE_QLiSymm8_Arg10_AsCyclicSums)`

TEST(QLiTest, WeaklySeparated) {
  // Should be true for any weight and number of points
  for (const int weight : range_incl(3, 4)) {
    for (const int num_points : {4, 6, 8}) {
      const auto points = to_vector(range_incl(1, num_points));
      EXPECT_TRUE(is_totally_weakly_separated(QLiVec(weight, points)));
    }
  }
}
