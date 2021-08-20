#include "lib/polylog_qli.h"

#include "gtest/gtest.h"

#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


inline ProjectionExpr Pr(std::initializer_list<X> data) {
  return ProjectionExpr::single(data);
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
