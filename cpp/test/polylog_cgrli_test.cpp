#include "lib/polylog_cgrli.h"

#include "gtest/gtest.h"


TEST(CGrLiTest, IsTotallyWeaklySeparated) {
  const std::vector points = {1,2,3,4,5,6};
  // Should be true for any weight.
  for (const int weight : range_incl(2, 4)) {
    EXPECT_TRUE(is_totally_weakly_separated(CGrLiDim3(weight, points)));
  }
}
