#include "lib/integer_math.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "lib/util.h"


TEST(GeneratePartitionsTest, Count) {
  EXPECT_THAT(
    mapped(range_incl(1, 20), [](const int n) {
      return get_partitions(n).size();
    }),
    // https://oeis.org/A000041
    testing::ElementsAre(
      1, 2, 3, 5, 7, 11, 15, 22, 30, 42,
      56, 77, 101, 135, 176, 231, 297, 385, 490, 627
    )
  );
}

TEST(GeneratePartitionsTest, Golden) {
  EXPECT_THAT(
    get_partitions(5),
    testing::ElementsAre(
      std::vector{5},
      std::vector{4, 1},
      std::vector{3, 2},
      std::vector{3, 1, 1},
      std::vector{2, 2, 1},
      std::vector{2, 1, 1, 1},
      std::vector{1, 1, 1, 1, 1}
    )
  );
}

TEST(GeneratePartitionsTest, FixedNumSummand) {
  EXPECT_THAT(
    get_partitions(5, 3),
    testing::ElementsAre(
      std::vector{3, 1, 1},
      std::vector{2, 2, 1}
    )
  );
}
