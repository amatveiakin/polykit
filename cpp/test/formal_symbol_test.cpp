#include "lib/formal_symbol.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(FormalSymbolTest, Inverse) {
  const std::vector<int> weights_orig = {1,3,5};
  const std::vector<std::vector<int>> points_orig = {{1},{3,4,5},{2}};
  Word symbol = formal_symbol(weights_orig, points_orig);
  std::vector<int> weights_decoded;
  std::vector<std::vector<int>> points_decoded;
  decode_formal_symbol(symbol, weights_decoded, points_decoded);
  EXPECT_EQ(weights_decoded, weights_orig);
  EXPECT_EQ(points_decoded, points_orig);
  EXPECT_EQ(formal_symbol_to_string(symbol), "Li_1_3_5(x1,x3x4x5,x2)");
}
