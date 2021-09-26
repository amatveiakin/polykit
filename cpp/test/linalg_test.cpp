#include "lib/linalg.h"

#include "gtest/gtest.h"

#include "lib/range.h"
#include "lib/string.h"


const char* skip_starting_newline(const char* s) {
  CHECK_EQ(s[0], '\n');
  return s + 1;
}


TEST(MatrixTest, ToStringAllFit) {
  const int n = 10;
  Matrix mat;
  for (const int i : range(n)) {
    mat.insert(i, i) = i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
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
  Matrix mat;
  for (const int i : range(n)) {
    mat.insert(i, i) = i + 1;
  }
  for (const int i : range(n)) {
    mat.insert(n + i, i) = n + i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
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
  Matrix mat;
  for (const int i : range(n)) {
    mat.insert(i, i) = i + 1;
  }
  for (const int i : range(n)) {
    mat.insert(i, n + i) = n + i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
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
  Matrix mat;
  for (const int i : range(n)) {
    mat.insert(i, i) = i + 1;
  }
  EXPECT_EQ(to_string(mat), skip_starting_newline(R"(
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
