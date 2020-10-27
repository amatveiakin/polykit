#include "lib/polylog_multiarg.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LilyTest, Li_1_n_plus_Li_n_1) {
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + Lily(1,2)({1},{2})
    + Lily(2,1)({2},{1})
    ,
    + Lily(3)  ({1,2})
  );
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + Lily(1,2)({1},{2})
    + Lily(2,1)({2},{1})
    ,
    + Lily(3)  ({1,2})
  );
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    + Lily(1,3)({1},{2})
    + Lily(3,1)({2},{1})
    ,
    + Lily(4)  ({1,2})
  );
}

TEST(LilyTest, LiShuffleProductNoLyndon) {
  EXPECT_EXPR_EQ(
    + Lily(1,1,1)({1},{2},{3})
    + Lily(1,1,1)({1},{3},{2})
    + Lily(1,1,1)({3},{1},{2})
    - Lily(1,2)  ({1},{2,3})
    - Lily(2,1)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Lily(1,1)({1},{2}),
      Lily(1)  ({3})
    )
  );

  EXPECT_EXPR_EQ(
    + Lily(1,1,2)({1},{2},{3})
    + Lily(1,2,1)({1},{3},{2})
    + Lily(2,1,1)({3},{1},{2})
    - Lily(1,3)  ({1},{2,3})
    - Lily(3,1)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Lily(1,1)({1},{2}),
      Lily(2)  ({3})
    )
  );

#if 0  // More expensive tests
  EXPECT_EXPR_EQ(
    + Lily(2,2,2)({1},{2},{3})
    + Lily(2,2,2)({1},{3},{2})
    + Lily(2,2,2)({3},{1},{2})
    - Lily(2,4)  ({1},{2,3})
    - Lily(4,2)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Lily(2,2)({1},{2}),
      Lily(2)  ({3})
    )
  );

  EXPECT_EXPR_EQ(
    + Lily(2,2,2,2)({1},{2},{3},{4})
    + Lily(2,2,2,2)({1},{2},{4},{3})
    + Lily(2,2,2,2)({1},{4},{2},{3})
    + Lily(2,2,2,2)({4},{1},{2},{3})
    - Lily(2,2,4)  ({1},{2},{3,4})
    - Lily(2,4,2)  ({1},{2,4},{3})
    - Lily(4,2,2)  ({1,4},{2},{3})
    ,
    shuffle_product_expr(
      Lily(2,2,2)({1},{2},{3}),
      Lily(2)    ({4})
    )
  );
#endif
}

TEST(CoLiTest, CoLiShuffleProduct) {
  EXPECT_EXPR_EQ(
    + CoLi({1,1}, {{1},{2}})
    + CoLi({1,1}, {{2},{1}})
    - CoLi({2},   {{1,2}})
    ,
    + coproduct(
      EFormalSymbol(LiParam({1}, {{1}})),
      EFormalSymbol(LiParam({1}, {{2}}))
    )
    + coproduct(
      EFormalSymbol(LiParam({1}, {{2}})),
      EFormalSymbol(LiParam({1}, {{1}}))
    )
  );
}
