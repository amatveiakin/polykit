#include "lib/expr_matrix_builder.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


inline StringExpr SS(const std::string& s) {
  return StringExpr::single(s);
}

using CoordValue = std::pair<int, int>;

Matrix matrix(const std::vector<std::vector<int>>& values) {
  Matrix ret(values.size(), values.at(0).size());
  for (const int i_row : range(values.size())) {
    const auto& row = values[i_row];
    for (const int i_col : range(row.size())) {
      const int value = row[i_col];
      if (value != 0) {
        ret(i_row, i_col) = value;
      }
    }
  }
  return ret;
}


TEST(ExprMatrixBuilderTest, SingleType) {
  ExprTupleMatrixBuilder<SimpleVectorExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}) + SV({2}));
  matrix_builder.add_expr(SV({2}) + SV({4}));
  matrix_builder.add_expr(SV({3}) - SV({4}));
  EXPECT_MATRIX_EQ(
    matrix_builder.make_matrix(),
    matrix({
      { 0,  0,  1 },
      { 0,  1, -1 },
      { 1,  0,  0 },
      { 1,  1,  0 }
    })
  );
}

TEST(ExprMatrixBuilderTest, TwoOfSameTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, SimpleVectorExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SV({2}));
  matrix_builder.add_expr(SV({2}),  SV({4}));
  matrix_builder.add_expr(SV({3}), -SV({4}));
  EXPECT_MATRIX_EQ(
    matrix_builder.make_matrix(),
    matrix({
      // Initially we get a larger matrix, but `make_matrix` deduplicates rows.
      { 0,  0,  1 },
      { 0,  1, -1 },
      { 0,  1,  0 },
      { 1,  0,  0 },
    })
  );
}
TEST(ExprMatrixBuilderTest, ThreeOfSameTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, SimpleVectorExpr, SimpleVectorExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SV({2}), -SV({1}));
  matrix_builder.add_expr(SV({2}),  SV({4}), -SV({1}));
  matrix_builder.add_expr(SV({3}), -SV({4}),  SV({3})+SV({1}));
  EXPECT_MATRIX_EQ(
    matrix_builder.make_matrix(),
    matrix({
      // Initially we get a larger matrix, but `make_matrix` deduplicates rows.
      { -1, -1,  1 },
      {  0,  0,  1 },
      {  0,  1, -1 },
      {  0,  1,  0 },
      {  1,  0,  0 },
    })
  );
}

// Structurally same as `TwoOfSameTypes`.
TEST(ExprMatrixBuilderTest, DifferentTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, StringExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SS({"2"}));
  matrix_builder.add_expr(SV({2}),  SS({"4"}));
  matrix_builder.add_expr(SV({3}), -SS({"4"}));
  EXPECT_MATRIX_EQ(
    matrix_builder.make_matrix(),
    matrix({
      { 0,  0,  1 },
      { 0,  1, -1 },
      { 0,  1,  0 },
      { 1,  0,  0 },
    })
  );
}

// Structurally same as `ThreeOfSameTypes`.
TEST(ExprMatrixBuilderTest, MixedTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, SimpleVectorExpr, StringExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SV({2}), -SS({"1"}));
  matrix_builder.add_expr(SV({2}),  SV({4}), -SS({"1"}));
  matrix_builder.add_expr(SV({3}), -SV({4}),  SS({"3"})+SS({"1"}));
  EXPECT_MATRIX_EQ(
    matrix_builder.make_matrix(),
    matrix({
      { -1, -1,  1 },
      {  0,  0,  1 },
      {  0,  1, -1 },
      {  0,  1,  0 },
      {  1,  0,  0 },
    })
  );
}
