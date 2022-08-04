#include "lib/permutation.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "lib/util.h"


TEST(PermutationTest, Apply) {
  Permutation perm({{0,2}, {1,3,7}});
  EXPECT_EQ(
    perm.applied({2,3,4,5,6,7,8,9,0,1}),
    (std::vector{0,7,4,5,6,1,8,9,2,3})
  );
}

TEST(PermutationTest, PowCoPrime) {
  const auto perm = Permutation({{1,3,5}}).pow(5);
  EXPECT_EQ(perm.cycles().size(), 1);
  EXPECT_EQ(
    perm.applied({1,2,3,4,5}),
    (std::vector{5,2,1,4,3})
  );
}

TEST(PermutationTest, PowDivides) {
  const auto perm = Permutation({{1,3,5}}).pow(6);
  EXPECT_EQ(perm.cycles().size(), 0);
  EXPECT_TRUE(perm.is_identity());
  EXPECT_EQ(
    perm.applied({1,2,3,4,5}),
    (std::vector{1,2,3,4,5})
  );
}

TEST(PermutationTest, PowNontirvialGcd) {
  const auto perm = Permutation({{1,2,3,4,5,6}}).pow(4);
  EXPECT_EQ(perm.cycles().size(), 2);
  EXPECT_EQ(
    perm.applied({0,1,2,3,4,5,6,7}),
    (std::vector{0,5,6,1,2,3,4,7})
  );
}

TEST(PermutationTest, PowEqIterated) {
  const auto perm = Permutation({{1,3}, {2,4,6}, {7,8,9,10}});
  const auto points = seq(12);
  for (const int k : range(10)) {
    auto expected = points;
    for (EACH : range(k)) {
      expected = perm.applied(expected);
    }
    EXPECT_EQ(perm.pow(k).applied(points), expected);
  }
}
