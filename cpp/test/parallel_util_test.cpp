#include "lib/parallel_util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


TEST(ParallelUtilTest, MappedParallel) {
  EXPECT_THAT(
    mapped_parallel(std::vector{1, 2, 3, 4, 5, 6, 7, 8}, [](int x) { return x * 2; }),
    testing::ElementsAre(2, 4, 6, 8, 10, 12, 14, 16)
  );
}
