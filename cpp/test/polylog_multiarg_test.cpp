#include "lib/polylog_multiarg.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LiTest, Li_1_n_plus_Li_n_1) {
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + Lily({1,2}, {{1},{2}})
    + Lily({2,1}, {{2},{1}})
    ,
    + Lily({3},   {{1,2}})
  );
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + Lily({1,2}, {{1},{2}})
    + Lily({2,1}, {{2},{1}})
    ,
    + Lily({3},   {{1,2}})
  );
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + Lily({1,3}, {{1},{2}})
    + Lily({3,1}, {{2},{1}})
    ,
    + Lily({4},   {{1,2}})
  );
}

TEST(LiTest, Li_1_1_1_ShuffleProductNoLyndon) {
  EXPECT_EXPR_EQ(
    + Lily({1,1,1}, {{1},{2},{3}})
    + Lily({1,1,1}, {{1},{3},{2}})
    + Lily({1,1,1}, {{3},{1},{2}})
    - Lily({1,2},   {{1},{2,3}})
    - Lily({2,1},   {{1,3},{2}})
    ,
    shuffle_product_expr(
      Lily({1,1}, {{1},{2}}),
      Lily({1},   {{3}})
    )
  );
}
