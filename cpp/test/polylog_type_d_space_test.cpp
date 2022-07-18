#include "lib/polylog_type_d_space.h"

#include "gtest/gtest.h"


TEST(TypeDSpaceTest, NLogIsWeaklySeparated) {
  for (const auto& gen : typeD_B2_generators()) {
    for (const int weight : range_incl(2, 4)) {
      const auto expr = NLog(weight, gen);
      EXPECT_EQ(expr.weight(), weight);
      EXPECT_TRUE(is_totally_weakly_separated(expr));
    }
  }
}
