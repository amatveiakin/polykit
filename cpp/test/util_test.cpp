#include "lib/util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


TEST(GroupByTest, GroupEqual) {
  EXPECT_EQ(
    group_equal(std::vector<int>{1, 2, 2, 1, 3, 3, 3, 1, 1, 3}),
    (std::vector<std::vector<int>>{{1}, {2, 2}, {1}, {3, 3, 3}, {1, 1}, {3}})
  );
}

TEST(MappedTest, MappedExpanding) {
  std::vector input{1, 2, 3, 4};
  const auto func = [](const int x) { return std::vector<int>(x, x); };
  EXPECT_EQ(
    mapped_expanding(input, func),
    flatten(mapped(input, func))
  );
  EXPECT_EQ(
    mapped_expanding(input, func),
    (std::vector{1, 2, 2, 3, 3, 3, 4, 4, 4, 4})
  );
}
