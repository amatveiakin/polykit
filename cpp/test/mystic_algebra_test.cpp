#include "lib/mystic_algebra.h"

#include "gtest/gtest.h"

#include "lib/polylog_li.h"
#include "test_util/matchers.h"


TEST(MysticProductTest, CoLiShuffle_Arg2_Weight2) {
  EXPECT_EXPR_EQ(
    + CoLi(1,1)({1},{2})
    + CoLi(1,1)({2},{1})
    + CoLi(2)  ({1,2})
    ,
    mystic_product(
      CoLi(1)({1}),
      CoLi(1)({2})
    )
  );
}
TEST(MysticProductTest, CoLiShuffle_Arg2_Weight3) {
  EXPECT_EXPR_EQ(
    + CoLi(1,2)({1},{2})
    + CoLi(2,1)({2},{1})
    + CoLi(3)  ({1,2})
    ,
    mystic_product(
      CoLi(1)({1}),
      CoLi(2)({2})
    )
  );
}
TEST(MysticProductTest, CoLiShuffle_Arg2_Weight4) {
  EXPECT_EXPR_EQ(
    + CoLi(1,3)({1},{2})
    + CoLi(3,1)({2},{1})
    + CoLi(4)  ({1,2})
    ,
    mystic_product(
      CoLi(1)({1}),
      CoLi(3)({2})
    )
  );
}

TEST(MysticProductTest, CoLiShuffle_Arg3_Weight6) {
  EXPECT_EXPR_EQ(
    + CoLi(2,2,2)({1},{2},{3})
    + CoLi(2,2,2)({1},{3},{2})
    + CoLi(2,2,2)({3},{1},{2})
    + CoLi(2,4)  ({1},{2,3})
    + CoLi(4,2)  ({1,3},{2})
    ,
    mystic_product(
      CoLi(2,2)({1},{2}),
      CoLi(2)  ({3})
    )
  );
}

#if RUN_LARGE_TESTS
TEST(MysticProductTest, CoLiShuffle_Arg4_Weight8) {
  EXPECT_EXPR_EQ(
    + CoLi(2,2,2,2)({1},{2},{3},{4})
    + CoLi(2,2,2,2)({1},{2},{4},{3})
    + CoLi(2,2,2,2)({1},{4},{2},{3})
    + CoLi(2,2,2,2)({4},{1},{2},{3})
    + CoLi(2,2,4)  ({1},{2},{3,4})
    + CoLi(2,4,2)  ({1},{2,4},{3})
    + CoLi(4,2,2)  ({1,4},{2},{3})
    ,
    mystic_product(
      CoLi(2,2,2)({1},{2},{3}),
      CoLi(2)    ({4})
    )
  );
}
#endif
