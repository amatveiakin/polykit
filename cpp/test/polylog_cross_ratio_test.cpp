#include "lib/polylog_cross_ratio.h"

#include "gtest/gtest.h"

#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


inline ProjectionExpr Pr(std::initializer_list<int> data) {
  return ProjectionExpr::single(data);
}


TEST(LidoTest, Lido4_Arg6) {
  auto expr = Lido4(1,2,3,4,5,6);
  EXPECT_EQ(expr.size(), 2560);
  auto lyndon = to_lyndon_basis(expr);
  EXPECT_EQ(lyndon.size(), 1272);
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

TEST(LidoTest, LidoSymmEquation_Arg6) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + LidoSymm2(1,2,3,4,5,6)
    + LidoSymm2(2,3,4,5,6,1)
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + LidoSymm3(1,2,3,4,5,6)
    - LidoSymm3(2,3,4,5,6,1)
  );
}

TEST(LidoTest, LidoSymmEquation_Arg8) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + LidoSymm3(1,2,3,4,5,6,7,8)
    - LidoSymm3(2,3,4,5,6,7,8,1)
  );
}

#if RUN_LARGE_TESTS
TEST(LidoTest, LidoSymmEquation_Arg10) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(project_on_x1(
    + LidoSymm4(1,2,3,4,5,6,7,8,9,10)
    + LidoSymm4(2,3,4,5,6,7,8,9,10,1)
  ));
}
#endif

TEST(LidoTest, LidoSymm_Arg8_AlternativeFormula) {
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + LidoSymm3(1,2,3,4,5,6,7,8)
    ,
    + Lido3(1,2,3,4,5,6,7,8)
    + (
      - LidoSymm3(1,2,3,4,5,6)
      - LidoSymm3(3,4,5,6,7,8)
      - LidoSymm3(5,6,7,8,1,2)
      - LidoSymm3(7,8,1,2,3,4)
      - Lido3(1,2,3,4,5,6)
      - Lido3(3,4,5,6,7,8)
      - Lido3(5,6,7,8,1,2)
      - Lido3(7,8,1,2,3,4)
    ).dived_int(2)
  );
}

TEST(LidoTest, Lido_Arg6_ShiftedDiffFormula) {
  for (int w : range_incl(2, 3)) {
    const int s = neg_one_pow(w);
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      +  LidoVec(w, {1,2,3,4,5,6})
      +s*LidoVec(w, {2,3,4,5,6,1})
      ,
      + LidoVec(w, {1,2,3,4})
      - LidoVec(w, {3,4,5,2})
      + LidoVec(w, {3,4,5,6})
      - LidoVec(w, {5,6,1,4})
      + LidoVec(w, {5,6,1,2})
      - LidoVec(w, {1,2,3,6})
    );
  }
}

#if RUN_LARGE_TESTS
TEST(LidoTest, Lido_Arg8_ShiftedDiffFormula) {
  for (int w : range_incl(3, 4)) {
    const int s = neg_one_pow(w);
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      +  LidoVec(w, {1,2,3,4,5,6,7,8})
      +s*LidoVec(w, {2,3,4,5,6,7,8,1})
      ,
      - LidoVec(w, {1,2,3,4,5,8})
      + LidoVec(w, {1,2,3,4,5,6})
      - LidoVec(w, {3,4,5,6,7,2})
      + LidoVec(w, {3,4,5,6,7,8})
      - LidoVec(w, {5,6,7,8,1,4})
      + LidoVec(w, {5,6,7,8,1,2})
      - LidoVec(w, {7,8,1,2,3,6})
      + LidoVec(w, {7,8,1,2,3,4})
      + LidoVec(w, {1,2,3,6})
      - LidoVec(w, {1,2,3,8})
      - LidoVec(w, {1,2,5,6})
      + LidoVec(w, {1,2,5,8})
      + LidoVec(w, {1,4,5,6})
      - LidoVec(w, {1,4,5,8})
      + LidoVec(w, {1,4,7,8})
      - LidoVec(w, {1,6,7,8})
      - LidoVec(w, {3,4,5,2})
      + LidoVec(w, {3,4,7,2})
      + LidoVec(w, {3,4,5,8})
      - LidoVec(w, {3,4,7,8})
      - LidoVec(w, {3,6,7,2})
      + LidoVec(w, {3,6,7,8})
      + LidoVec(w, {5,6,7,2})
      - LidoVec(w, {5,6,7,4})
    );
  }
}
#endif

class SubsetSumFormulaTest : public ::testing::TestWithParam<std::pair<int, int>> {
public:
  int weight() const { return GetParam().first; }
  int total_points() const { return GetParam().second; }
};

TEST_P(SubsetSumFormulaTest, LidoSymm_SubsetSumFormula) {
  DeltaExpr expr;
  for (int num_args = 4; num_args <= total_points(); num_args += 2) {
    for (const auto& seq : increasing_sequences(total_points(), num_args)) {
      const auto args = mapped(seq, [](int x) { return x + 1; });
      const int sign_proto = absl::c_accumulate(args, 0) + num_args / 2;
      const int sign = neg_one_pow(sign_proto);
      expr += sign * LidoSymmVec(weight(), args);
    }
  }
  EXPECT_EXPR_ZERO_AFTER_LYNDON(expr);
}

INSTANTIATE_TEST_SUITE_P(FastCases, SubsetSumFormulaTest, ::testing::Values(
  std::pair{2, 5},
  std::pair{2, 6},
  std::pair{3, 6}
));
#if RUN_LARGE_TESTS
INSTANTIATE_TEST_SUITE_P(SlowCases, SubsetSumFormulaTest, ::testing::Values(
  std::pair{3, 7},
  std::pair{4, 7},
  std::pair{3, 8},
  std::pair{4, 8},
  std::pair{5, 8}
));
#endif

TEST(LidoTest, LidoBuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x1(Lido3(1,2,3,4,5,6)),
    LidoVecPr(3, {1,2,3,4,5,6}, project_on_x1)
  );
  EXPECT_EXPR_EQ(
    project_on_x7(Lido3(1,7,3,4,5,6,7,8)),
    LidoVecPr(3, {1,7,3,4,5,6,7,8}, project_on_x7)
  );
}

TEST(LidoTest, LidoSymmBuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x4(LidoSymm3(1,2,3,4,5,6)),
    LidoSymmVecPr(3, {1,2,3,4,5,6}, project_on_x4)
  );
}
