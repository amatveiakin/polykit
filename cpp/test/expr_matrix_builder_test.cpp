#include "lib/expr_matrix_builder.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "test_util/helpers.h"
#include "test_util/matchers.h"


inline StringExpr SS(const std::string& s) {
  return StringExpr::single(s);
}

bool operator==(const Matrix::Triplet& lhs, const Matrix::Triplet& rhs) {
  return std::tie(lhs.row, lhs.col, lhs.value) == std::tie(rhs.row, rhs.col, rhs.value);
}

using CoordValue = std::pair<int, int>;

Matrix sort_columns(const Matrix& src) {
  std::vector<std::vector<CoordValue>> cols(src.cols());
  for (const auto& t : src.as_triplets()) {
    cols.at(t.col).push_back({t.row, t.value});
  }
  const auto sorted_cols = sorted(mapped(cols, DISAMBIGUATE(sorted)));
  Matrix dst;
  for (const int i_col : range(sorted_cols.size())) {
    for (const auto& [i_row, value] : sorted_cols[i_col]) {
      dst.insert(i_row, i_col) = value;
    }
  }
  return dst;
}

bool matrix_eq_modulo_rearrangement(const Matrix& a, const Matrix& b) {
  const Matrix b_sorted = sort_columns(b);
  std::vector<std::vector<CoordValue>> rows(a.rows());
  for (const auto& t : a.as_triplets()) {
    rows.at(t.row).push_back({t.col, t.value});
  }
  for (const auto& rows_perm : permutations(rows)) {
    Matrix a_shuffled;
    for (const int i_row : range(rows_perm.size())) {
      for (const auto& [i_col, value] : rows_perm[i_row]) {
        a_shuffled.insert(i_row, i_col) = value;
      }
    }
    const Matrix a_shuffled_sorted = sort_columns(a_shuffled);
    if (a_shuffled_sorted.as_triplets() == b_sorted.as_triplets()) {
      return true;
    }
  }
  return false;
}

void EXPECT_MATRIX_EQ_MODULO_REARRANGEMENT(const Matrix& a, const Matrix& b) {
  EXPECT_TRUE(matrix_eq_modulo_rearrangement(a, b)) << to_string(a) << "vs\n" << to_string(b);
}

Matrix matrix(const std::vector<std::vector<int>>& values) {
  Matrix ret;
  for (const int i_row : range(values.size())) {
    const auto& row = values[i_row];
    for (const int i_col : range(row.size())) {
      const int value = row[i_col];
      if (value != 0) {
        ret.insert(i_row, i_col) = value;
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
  EXPECT_MATRIX_EQ_MODULO_REARRANGEMENT(
    matrix_builder.make_matrix(),
    matrix({
      {1,  1,  0,  0},
      {0,  1,  0,  1},
      {0,  0,  1, -1},
    })
  );
}

TEST(ExprMatrixBuilderTest, TwoOfSameTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, SimpleVectorExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SV({2}));
  matrix_builder.add_expr(SV({2}),  SV({4}));
  matrix_builder.add_expr(SV({3}), -SV({4}));
  EXPECT_MATRIX_EQ_MODULO_REARRANGEMENT(
    matrix_builder.make_matrix(),
    matrix({
      // Initially we get:
      //   {1,  0,  0,  1,  0},
      //   {0,  1,  0,  0,  1},
      //   {0,  0,  1,  0, -1},
      // but the columns are deduplicated in `make_matrix`.
      {1,  0,  0,  0},
      {0,  1,  0,  1},
      {0,  0,  1, -1},
    })
  );
}
TEST(ExprMatrixBuilderTest, ThreeOfSameTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, SimpleVectorExpr, SimpleVectorExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SV({2}), -SV({1}));
  matrix_builder.add_expr(SV({2}),  SV({4}), -SV({1}));
  matrix_builder.add_expr(SV({3}), -SV({4}),  SV({3})+SV({1}));
  EXPECT_MATRIX_EQ_MODULO_REARRANGEMENT(
    matrix_builder.make_matrix(),
    matrix({
      // Initially we get:
      //   {1,  0,  0,  1,  0, -1,  0},
      //   {0,  1,  0,  0,  1, -1,  0},
      //   {0,  0,  1,  0, -1,  1,  1},
      // but the columns are deduplicated in `make_matrix`.
      {1,  0,  0,  0, -1},
      {0,  1,  0,  1, -1},
      {0,  0,  1, -1,  1},
    })
  );
}

// Structurally same as `TwoOfSameTypes`.
TEST(ExprMatrixBuilderTest, DifferentTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, StringExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SS({"2"}));
  matrix_builder.add_expr(SV({2}),  SS({"4"}));
  matrix_builder.add_expr(SV({3}), -SS({"4"}));
  EXPECT_MATRIX_EQ_MODULO_REARRANGEMENT(
    matrix_builder.make_matrix(),
    matrix({
      {1,  0,  0,  0},
      {0,  1,  0,  1},
      {0,  0,  1, -1},
    })
  );
}

// Structurally same as `ThreeOfSameTypes`.
TEST(ExprMatrixBuilderTest, MixedTypes) {
  ExprTupleMatrixBuilder<SimpleVectorExpr, SimpleVectorExpr, StringExpr> matrix_builder;
  matrix_builder.add_expr(SV({1}),  SV({2}), -SS({"1"}));
  matrix_builder.add_expr(SV({2}),  SV({4}), -SS({"1"}));
  matrix_builder.add_expr(SV({3}), -SV({4}),  SS({"3"})+SS({"1"}));
  EXPECT_MATRIX_EQ_MODULO_REARRANGEMENT(
    matrix_builder.make_matrix(),
    matrix({
      {1,  0,  0,  0, -1},
      {0,  1,  0,  1, -1},
      {0,  0,  1, -1,  1},
    })
  );
}
