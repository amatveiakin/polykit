#include "lib/util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


TEST(GroupByTest, GroupEqual) {
  EXPECT_EQ(
    group_equal(std::vector<int>{1, 2, 2, 1, 3, 3, 3, 1, 1, 3}),
    (std::vector<std::vector<int>>{{1}, {2, 2}, {1}, {3, 3, 3}, {1, 1}, {3}})
  );
}
