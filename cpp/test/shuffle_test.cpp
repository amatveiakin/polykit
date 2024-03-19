#include "lib/shuffle.h"

#include "gtest/gtest.h"

#include "test_util/helpers.h"
#include "test_util/matchers.h"


TEST(ShuffleProductTest, ThreeExpressions) {
  EXPECT_EXPR_EQ(
    shuffle_product(std::vector{
      std::vector{0, 1, 0},
      std::vector{1, 0},
      std::vector{1},
    }).cast_to<SimpleVectorExpr::Basic>()
    ,
    (
     +2 * SV({0, 1, 0, 1, 0, 1})
     +4 * SV({0, 1, 0, 1, 1, 0})
     +4 * SV({0, 1, 1, 0, 0, 1})
     +8 * SV({0, 1, 1, 0, 1, 0})
    +12 * SV({0, 1, 1, 1, 0, 0})
     +2 * SV({1, 0, 0, 1, 0, 1})
     +4 * SV({1, 0, 0, 1, 1, 0})
     +2 * SV({1, 0, 1, 0, 0, 1})
     +6 * SV({1, 0, 1, 0, 1, 0})
     +8 * SV({1, 0, 1, 1, 0, 0})
     +4 * SV({1, 1, 0, 0, 1, 0})
     +4 * SV({1, 1, 0, 1, 0, 0})
    ).main()
  );
}
