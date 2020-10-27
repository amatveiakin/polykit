#include "lib/polylog_param.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LiParamTest, Serialization) {
  const LiParam orig{{1,3,5}, {{1},{3,4,5},{2}}};
  Word encoded = li_param_to_key(orig);
  const LiParam decoded = key_to_li_param(encoded);
  EXPECT_EQ(orig.weights(), decoded.weights());
  EXPECT_EQ(orig.points(), decoded.points());
  EXPECT_EQ(to_string(orig), "Li_1_3_5(x1,x3x4x5,x2)");
}
