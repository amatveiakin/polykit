#include "lib/ratio.h"

#include "gtest/gtest.h"

#include "lib/delta_ratio.h"
#include "test_util/matchers.h"


TEST(CompoundRatioTest, MultiplicationGolden) {
  using RatioLoops = std::vector<std::vector<int>>;
  EXPECT_EQ(
    (CR(4,5,6,3) * CR(2,3,6,1)).loops(),
    (RatioLoops{{2,3,4,5,6,1}})
  );
  EXPECT_EQ(
    (CR(2,3,4,5) * CR(2,5,6,1)).loops(),
    (RatioLoops{{2,3,4,5,6,1}})
  );
  EXPECT_EQ(
    (CR(2,3,4,1) * CR(4,5,6,1)).loops(),
    (RatioLoops{{2,3,4,5,6,1}})
  );
}

void check_multiplication_correctness(const std::vector<int>& lhs, const std::vector<int>& rhs) {
  CompoundRatio product = CompoundRatio::from_loops({lhs}) * CompoundRatio::from_loops({rhs});
  ASSERT_EQ(product.loops().size(), 1);
  EXPECT_EXPR_EQ(
    cross_ratio(lhs) + cross_ratio(rhs),
    cross_ratio(product.loops().front())
  );
}

TEST(CompoundRatioTest,MultiplicationCorrectness) {
  check_multiplication_correctness({7,8,9,6}, {5,6,9,4});
  check_multiplication_correctness({7,8,9,6}, {5,6,9,4});
  check_multiplication_correctness({5,6,7,8}, {5,8,9,4});
  check_multiplication_correctness({5,6,7,4}, {7,8,9,4});
  check_multiplication_correctness({5,6,7,4}, {3,4,7,2});
  check_multiplication_correctness({3,4,5,6}, {3,6,7,2});
  check_multiplication_correctness({3,4,5,2}, {5,6,7,2});
  check_multiplication_correctness({7,8,9,6}, {5,6,9,4});
  check_multiplication_correctness({5,6,7,8}, {5,8,9,4});
  check_multiplication_correctness({5,6,7,4}, {7,8,9,4});
  check_multiplication_correctness({5,6,7,8,9,4}, {3,4,9,2});
  check_multiplication_correctness({7,8,9,4}, {3,4,9,2});
  check_multiplication_correctness({5,8,9,4}, {3,4,9,2});
  check_multiplication_correctness({5,6,9,4}, {3,4,9,2});
  check_multiplication_correctness({3,4,5,6}, {3,6,9,2});
  check_multiplication_correctness({9,2,3,4,5,6}, {7,8,9,6});
}
