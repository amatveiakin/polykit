#include "lib/compression.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "lib/util.h"


using testing::ElementsAre;
using testing::ElementsAreArray;

using TestBlob = CompressedBlob<void>;

// Checks correctness and size
class SmallValuesTest : public testing::TestWithParam<std::vector<int>> {
public:
  const std::vector<int>& values() const { return GetParam(); }
};

TEST_P(SmallValuesTest, OneSegment) {
  Compressor compressor;
  compressor.push_segment(values());
  const auto compressed = std::move(compressor).result<TestBlob>();
  const int kSizeOverhead = 1;
  ASSERT_EQ(compressed.data().size(), div_round_up(values().size() + kSizeOverhead, 2));
  Decompressor decompressor(compressed);
  EXPECT_FALSE(decompressor.done());
  auto uncompressed = decompressor.pop_segment();
  EXPECT_TRUE(decompressor.done());
  EXPECT_THAT(uncompressed, ElementsAreArray(values()));
}

TEST_P(SmallValuesTest, IndividualValues) {
  Compressor compressor;
  for (const int v : values()) {
    compressor.push_value(v);
  }
  const auto compressed = std::move(compressor).result<TestBlob>();
  ASSERT_EQ(compressed.data().size(), div_round_up(values().size(), 2));
  Decompressor decompressor(compressed);
  std::vector<int> uncompressed;
  while (!decompressor.done()) {
    uncompressed.push_back(decompressor.pop_value());
  }
  EXPECT_TRUE(decompressor.done());
  EXPECT_THAT(uncompressed, ElementsAreArray(values()));
}

INSTANTIATE_TEST_SUITE_P(AllCases, SmallValuesTest, testing::Values(
  std::vector<int>{},
  std::vector<int>{0},
  std::vector<int>{1},
  std::vector<int>{1, 2},
  std::vector<int>{0, 5, 1, 10, 1, 1, 0, 0}
));

// Checks correctness only: size for larger values is unspecified
class LargeValuesTest : public testing::TestWithParam<std::vector<int>> {
public:
  const std::vector<int>& values() const { return GetParam(); }
};

TEST_P(LargeValuesTest, OneSegment) {
  Compressor compressor;
  compressor.push_segment(values());
  const auto compressed = std::move(compressor).result<TestBlob>();
  Decompressor decompressor(compressed);
  EXPECT_FALSE(decompressor.done());
  auto uncompressed = decompressor.pop_segment();
  EXPECT_TRUE(decompressor.done());
  EXPECT_THAT(uncompressed, ElementsAreArray(values()));
}

TEST_P(LargeValuesTest, IndividualValues) {
  Compressor compressor;
  for (const int v : values()) {
    compressor.push_value(v);
  }
  const auto compressed = std::move(compressor).result<TestBlob>();
  Decompressor decompressor(compressed);
  std::vector<int> uncompressed;
  while (!decompressor.done()) {
    uncompressed.push_back(decompressor.pop_value());
  }
  EXPECT_TRUE(decompressor.done());
  EXPECT_THAT(uncompressed, ElementsAreArray(values()));
}

INSTANTIATE_TEST_SUITE_P(AllCases, LargeValuesTest, testing::Values(
  std::vector<int>{100},
  std::vector<int>{-100},
  std::vector<int>{0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
  std::vector<int>{17, 0, 17, -5, 100}
));

TEST(OneSmallSegmentTest, Mix) {
  Compressor compressor;
  compressor.push_value(10);
  compressor.push_segment({1, 2, 3});
  compressor.push_segment({1, -2});
  compressor.push_value(100);
  compressor.push_segment({});
  compressor.push_segment({0});
  compressor.push_value(0);
  compressor.push_value(0);
  const auto compressed = std::move(compressor).result<TestBlob>();
  Decompressor decompressor(compressed);
  EXPECT_EQ(decompressor.pop_value(), 10);
  EXPECT_THAT(decompressor.pop_segment(), ElementsAre(1, 2, 3));
  EXPECT_THAT(decompressor.pop_segment(), ElementsAre(1, -2));
  EXPECT_EQ(decompressor.pop_value(), 100);
  EXPECT_THAT(decompressor.pop_segment(), ElementsAre());
  EXPECT_THAT(decompressor.pop_segment(), ElementsAre(0));
  EXPECT_EQ(decompressor.pop_value(), 0);
  EXPECT_EQ(decompressor.pop_value(), 0);
  EXPECT_TRUE(decompressor.done());
}
