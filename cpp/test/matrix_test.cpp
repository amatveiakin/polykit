#include "lib/matrix.h"

#include "gtest/gtest.h"

#include "lib/range.h"
#include "lib/string.h"
#include "test_util/matchers.h"


const char* skip_starting_newline(const char* s) {
  CHECK_EQ(s[0], '\n');
  return s + 1;
}


TEST(MatrixTest, ToStringAllFit) {
  const int n = 10;
  Matrix mat(n, n);
  for (const int i : range(n)) {
    mat(i, i) = i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
10x10 matrix, 10 elements:
 1  0  0  0  0  0  0  0  0  0
 0  2  0  0  0  0  0  0  0  0
 0  0  3  0  0  0  0  0  0  0
 0  0  0  4  0  0  0  0  0  0
 0  0  0  0  5  0  0  0  0  0
 0  0  0  0  0  6  0  0  0  0
 0  0  0  0  0  0  7  0  0  0
 0  0  0  0  0  0  0  8  0  0
 0  0  0  0  0  0  0  0  9  0
 0  0  0  0  0  0  0  0  0 10
)"));
}

TEST(MatrixTest, ToStringRowOverflow) {
  const int n = 10;
  Matrix mat(2*n, n);
  for (const int i : range(n)) {
    mat(i, i) = i + 1;
  }
  for (const int i : range(n)) {
    mat(n + i, i) = n + i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
20x10 matrix, 20 elements:
 1  0  0  0  0  0  0  0  0  0
 0  2  0  0  0  0  0  0  0  0
 0  0  3  0  0  0  0  0  0  0
 0  0  0  4  0  0  0  0  0  0
 0  0  0  0  5  0  0  0  0  0
 0  0  0  0  0  6  0  0  0  0
 0  0  0  0  0  0  7  0  0  0
 .  .  .  .  .  .  .  .  .  .
 .  .  .  .  .  .  .  .  .  .
 .  .  .  .  .  .  .  .  .  .
 0  0  0  0 15  0  0  0  0  0
 0  0  0  0  0 16  0  0  0  0
 0  0  0  0  0  0 17  0  0  0
 0  0  0  0  0  0  0 18  0  0
 0  0  0  0  0  0  0  0 19  0
 0  0  0  0  0  0  0  0  0 20
)"));
}

TEST(MatrixTest, ToStringColOverflow) {
  const int n = 10;
  Matrix mat(n, 2*n);
  for (const int i : range(n)) {
    mat(i, i) = i + 1;
  }
  for (const int i : range(n)) {
    mat(i, n + i) = n + i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
10x20 matrix, 20 elements:
 1  0  0  0  0  0  0  .  .  .  0  0  0  0  0  0
 0  2  0  0  0  0  0  .  .  .  0  0  0  0  0  0
 0  0  3  0  0  0  0  .  .  .  0  0  0  0  0  0
 0  0  0  4  0  0  0  .  .  .  0  0  0  0  0  0
 0  0  0  0  5  0  0  .  .  . 15  0  0  0  0  0
 0  0  0  0  0  6  0  .  .  .  0 16  0  0  0  0
 0  0  0  0  0  0  7  .  .  .  0  0 17  0  0  0
 0  0  0  0  0  0  0  .  .  .  0  0  0 18  0  0
 0  0  0  0  0  0  0  .  .  .  0  0  0  0 19  0
 0  0  0  0  0  0  0  .  .  .  0  0  0  0  0 20
)"));
}

TEST(MatrixTest, ToStringAllOverflow) {
  const int n = 20;
  Matrix mat(n, n);
  for (const int i : range(n)) {
    mat(i, i) = i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
20x20 matrix, 20 elements:
 1  0  0  0  0  0  0  .  .  .  0  0  0  0  0  0
 0  2  0  0  0  0  0  .  .  .  0  0  0  0  0  0
 0  0  3  0  0  0  0  .  .  .  0  0  0  0  0  0
 0  0  0  4  0  0  0  .  .  .  0  0  0  0  0  0
 0  0  0  0  5  0  0  .  .  .  0  0  0  0  0  0
 0  0  0  0  0  6  0  .  .  .  0  0  0  0  0  0
 0  0  0  0  0  0  7  .  .  .  0  0  0  0  0  0
 .  .  .  .  .  .  .           .  .  .  .  .  .
 .  .  .  .  .  .  .           .  .  .  .  .  .
 .  .  .  .  .  .  .           .  .  .  .  .  .
 0  0  0  0  0  0  0  .  .  . 15  0  0  0  0  0
 0  0  0  0  0  0  0  .  .  .  0 16  0  0  0  0
 0  0  0  0  0  0  0  .  .  .  0  0 17  0  0  0
 0  0  0  0  0  0  0  .  .  .  0  0  0 18  0  0
 0  0  0  0  0  0  0  .  .  .  0  0  0  0 19  0
 0  0  0  0  0  0  0  .  .  .  0  0  0  0  0 20
)"));
}

TEST(MatrixTest, Sorting) {
  Matrix m(8, 8);
  m(1, 0) = 1;
  m(2, 0) = -1;
  m(3, 1) = 1;
  m(4, 1) = -1;
  m(5, 0) = 1; m(7, 1) = 2; m(7, 2) = 2;
  m(6, 0) = 1; m(6, 1) = 1; m(6, 2) = 2;
  m(7, 0) = 1; m(5, 1) = 1; m(5, 2) = 1;

  Matrix s(8, 8);
  s(0, 0) = -1;
  s(1, 1) = -1;
  s(3, 1) = 1;
  s(4, 0) = 1;
  s(5, 0) = 1; s(5, 1) = 1; s(5, 2) = 1;
  s(6, 0) = 1; s(6, 1) = 1; s(6, 2) = 2;
  s(7, 0) = 1; s(7, 1) = 2; s(7, 2) = 2;

  EXPECT_MATRIX_EQ(sort_unique_rows(m), s);
}
