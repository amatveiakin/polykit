#include "lib/polylog.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LiTest, Li_1_n_plus_Li_n_1) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + Li(1,2)({1},{2})
    + Li(2,1)({2},{1})
    + Li(3)  ({1,2})
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + Li(1,2)({1},{2})
    + Li(2,1)({2},{1})
    + Li(3)  ({1,2})
  );
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + Li(1,3)({1},{2})
    + Li(3,1)({2},{1})
    + Li(4)  ({1,2})
  );
}

TEST(LiTest, LiShuffleProductNoLyndon) {
  EXPECT_EXPR_EQ(
    + Li(1,1,1)({1},{2},{3})
    + Li(1,1,1)({1},{3},{2})
    + Li(1,1,1)({3},{1},{2})
    + Li(1,2)  ({1},{2,3})
    + Li(2,1)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Li(1,1)({1},{2}),
      Li(1)  ({3})
    )
  );

  EXPECT_EXPR_EQ(
    + Li(1,1,2)({1},{2},{3})
    + Li(1,2,1)({1},{3},{2})
    + Li(2,1,1)({3},{1},{2})
    + Li(1,3)  ({1},{2,3})
    + Li(3,1)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Li(1,1)({1},{2}),
      Li(2)  ({3})
    )
  );

#if 0  // More expensive tests
  EXPECT_EXPR_EQ(
    + Li(2,2,2)({1},{2},{3})
    + Li(2,2,2)({1},{3},{2})
    + Li(2,2,2)({3},{1},{2})
    - Li(2,4)  ({1},{2,3})
    - Li(4,2)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Li(2,2)({1},{2}),
      Li(2)  ({3})
    )
  );

  EXPECT_EXPR_EQ(
    + Li(2,2,2,2)({1},{2},{3},{4})
    + Li(2,2,2,2)({1},{2},{4},{3})
    + Li(2,2,2,2)({1},{4},{2},{3})
    + Li(2,2,2,2)({4},{1},{2},{3})
    - Li(2,2,4)  ({1},{2},{3,4})
    - Li(2,4,2)  ({1},{2,4},{3})
    - Li(4,2,2)  ({1,4},{2},{3})
    ,
    shuffle_product_expr(
      Li(2,2,2)({1},{2},{3}),
      Li(2)    ({4})
    )
  );
#endif
}

TEST(CoLiTest, CoLiShuffleProduct) {
  EXPECT_EXPR_EQ(
    + CoLi(1,1)({1},{2})
    + CoLi(1,1)({2},{1})
    + CoLi(2)  ({1,2})
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
