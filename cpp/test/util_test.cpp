#include "lib/util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointee;


TEST(AppendVectorTest, MoveOnly) {
  std::vector<std::unique_ptr<int>> a;
  a.push_back(std::make_unique<int>(1));
  std::vector<std::unique_ptr<int>> b;
  b.push_back(std::make_unique<int>(2));
  append_vector(a, std::move(b));  // should not compile without `std::move`
  EXPECT_THAT(a, testing::ElementsAre(Pointee(Eq(1)), Pointee(Eq(2))));
}

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
