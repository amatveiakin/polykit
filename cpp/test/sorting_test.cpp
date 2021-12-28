#include "lib/sorting.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "lib/compare.h"


TEST(SortedByProjectionTest, Golden) {
  EXPECT_THAT(
    sorted_by_projection(
      std::vector{1, 3, 4, 6, 2, 5},
      [](int x) { return -x; }
    ),
    testing::ElementsAre(6, 5, 4, 3, 2, 1)
  );
}

TEST(SortedByProjectionTest, Equivalent) {
  std::vector v{1, 10, 2, 3, 1, 5, 10, 2, 6, 7, 8, 2, 5};
  static const auto pr = [](int x) { return x % 5; };
  EXPECT_THAT(
    sorted_by_projection(v, pr),
    testing::ElementsAreArray(sorted(v, cmp::projected(pr)))
  );
}
