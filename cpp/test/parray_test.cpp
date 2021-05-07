#include "lib/parray.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(PArrayTest, UInt4ArrayCorrectnessEven) {
  std::array data{2, 3, 5, 7};
  PArray<uint4_t, 4> packed(data);
  static_assert(sizeof(packed) == 2);
  EXPECT_THAT(packed.unpacked(), testing::ElementsAreArray(data));
}

TEST(PArrayTest, UInt4ArrayCorrectnessOdd) {
  std::array data{2, 3, 5, 7, 11};
  PArray<uint4_t, 5> packed(data);
  static_assert(sizeof(packed) == 3);
  EXPECT_THAT(packed.unpacked(), testing::ElementsAreArray(data));
}

TEST(PArrayTest, UInt4ArrayComparisonIsLexicographic) {
  using PA1 = PArray<uint4_t, 1>;
  using PA2 = PArray<uint4_t, 2>;
  using PA3 = PArray<uint4_t, 3>;
  using PA6 = PArray<uint4_t, 6>;
  EXPECT_TRUE(PA1({5}) > PA1({0}));
  EXPECT_TRUE(PA1({2}) < PA1({3}));
  EXPECT_TRUE(PA2({1,4}) < PA2({2,3}));
  EXPECT_TRUE(PA3({1,10,11}) < PA3({2,3,4}));
  EXPECT_TRUE(PA3({1,10,1}) < PA3({1,10,2}));
  EXPECT_TRUE(PA6({1,2,3,4,5,6}) < PA6({1,2,3,5,4,6}));
  EXPECT_TRUE(PA6({1,2,3,4,5,6}) < PA6({1,2,3,4,6,5}));
  EXPECT_TRUE(PA6({1,2,3,4,5,6}) == PA6({1,2,3,4,5,6}));
}
