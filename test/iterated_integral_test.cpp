#include "lib/iterated_integral.h"

#include "gtest/gtest.h"

#include "lib/projection.h"
#include "test_util/matchers.h"


TEST(CorrTest, ArgsCanBeShifted_3) {
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    CorrVec({1,2,3}),
    CorrVec({2,3,1})
  );
  EXPECT_EXPR_EQ_AFTER_LYNDON(
    CorrVec({1,2,3}),
    CorrVec({3,1,2})
  );
}

TEST(CorrTest, ArgsCanBeShifted_4) {
  const std::vector<X> args = {1,2,3,4};
  for (int shift : range(1, 4)) {
    EXPECT_EXPR_EQ_AFTER_LYNDON(
      // Compare projection to speed up
      project_on_x1(CorrVec(args)),
      project_on_x1(CorrVec(rotated_vector(args, shift)))
    );
  }
}

TEST(CorrTest, BuiltinProjection) {
  EXPECT_EXPR_EQ(
    project_on_x2(CorrVec({1,2,3,4})),
    CorrVecPr({1,2,3,4}, project_on_x2)
  );
}
