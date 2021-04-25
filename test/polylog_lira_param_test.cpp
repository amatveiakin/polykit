#include "lib/polylog_lira_param.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LiraParamTest, Serialization) {
  const LiraParam orig{
    2,
    {1,3,5},
    {
      CR(1,2,4,5),
      CR(4,5,6,3) * CR(2,3,6,1),
      CR(5,6,2,3),
    }
  };
  auto encoded = lira_param_to_key(orig);
  const LiraParam decoded = key_to_lira_param(encoded);
  EXPECT_TRUE(orig == decoded);
}
