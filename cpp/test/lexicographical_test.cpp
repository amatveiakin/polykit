#include "lib/lexicographical.h"

#include "gtest/gtest.h"


static std::vector<int> lexicographically_minimal_rotation_naive(std::vector<int> v) {
  std::vector<int> min = v;
  for (int i : range(1, v.size())) {
    absl::c_rotate(v, v.begin() + 1);
    if (v < min) {
      min = v;
    }
  }
  return min;
}


TEST(MinimalRotationTest, Golden) {
  using Vec = std::vector<int>;
  using StrVec = std::vector<std::string>;

  EXPECT_EQ(lexicographically_minimal_rotation(Vec{}), (Vec{}));

  EXPECT_EQ(lexicographically_minimal_rotation(Vec{10}), (Vec{10}));

  EXPECT_EQ(lexicographically_minimal_rotation(Vec{1,2,3}), (Vec{1,2,3}));
  EXPECT_EQ(lexicographically_minimal_rotation(Vec{2,3,1}), (Vec{1,2,3}));
  EXPECT_EQ(lexicographically_minimal_rotation(Vec{3,1,2}), (Vec{1,2,3}));
  EXPECT_EQ(lexicographically_minimal_rotation(Vec{3,2,1}), (Vec{1,3,2}));
  EXPECT_EQ(lexicographically_minimal_rotation(Vec{1,3,2}), (Vec{1,3,2}));
  EXPECT_EQ(lexicographically_minimal_rotation(Vec{2,1,3}), (Vec{1,3,2}));

  EXPECT_EQ(lexicographically_minimal_rotation(Vec{1,2,1}), (Vec{1,1,2}));

  EXPECT_EQ(
    lexicographically_minimal_rotation(Vec{1,1,2,2,1,1,1,2,1,1,2}),
    (Vec{1,1,1,2,1,1,2,1,1,2,2})
  );

  EXPECT_EQ(
    lexicographically_minimal_rotation(StrVec{"bb", "a", "ccc", "dddd"}),
    (StrVec{"a", "ccc", "dddd", "bb"})
  );
}

TEST(MinimalRotationTest, CompareWithNaive) {
  // TODO: ...
}
