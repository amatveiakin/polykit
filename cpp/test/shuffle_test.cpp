#include "lib/shuffle.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(ShuffleProductTest, ThreeExpressions) {
  EXPECT_EXPR_EQ(
    shuffle_product({
      Word{0, 1, 0},
      Word{1, 0},
      Word{1 },
    })
    ,
     +2 * W({0, 1, 0, 1, 0, 1})
     +4 * W({0, 1, 0, 1, 1, 0})
     +4 * W({0, 1, 1, 0, 0, 1})
     +8 * W({0, 1, 1, 0, 1, 0})
    +12 * W({0, 1, 1, 1, 0, 0})
     +2 * W({1, 0, 0, 1, 0, 1})
     +4 * W({1, 0, 0, 1, 1, 0})
     +2 * W({1, 0, 1, 0, 0, 1})
     +6 * W({1, 0, 1, 0, 1, 0})
     +8 * W({1, 0, 1, 1, 0, 0})
     +4 * W({1, 1, 0, 0, 1, 0})
     +4 * W({1, 1, 0, 1, 0, 0})
  );
}
