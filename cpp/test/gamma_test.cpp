#include "lib/gamma.h"

#include "gtest/gtest.h"

#include "lib/bitset_util.h"
#include "lib/itertools.h"


static bool between(int point, std::pair<int, int> segment) {
  const auto [a, b] = segment;
  CHECK_LT(a, b);
  CHECK(all_unique_unsorted(std::array{point, a, b}));
  return a < point && point < b;
}

static bool are_weakly_separated_naive(const Gamma& g1, const Gamma& g2) {
  const auto a = bitset_to_vector(bitset_difference(g1.index_bitset(), g2.index_bitset()));
  const auto b = bitset_to_vector(bitset_difference(g2.index_bitset(), g1.index_bitset()));
  for (const auto& s1 : combinations(a, 2)) {
    for (const auto& s2 : combinations(b, 2)) {
      const auto [x1, y1] = to_array<2>(s1);
      const auto [x2, y2] = to_array<2>(s2);
      const bool itersect = between(x1, {x2, y2}) != between(y1, {x2, y2});
      if (itersect) {
        return false;
      }
    }
  }
  return true;
}


TEST(GammaTest, WeaklySeparatedGolden) {
  using v = std::vector<int>;
  EXPECT_TRUE(are_weakly_separated(Gamma(v{}), Gamma(v{})));
  EXPECT_TRUE(are_weakly_separated(Gamma(v{1}), Gamma(v{2})));
  EXPECT_TRUE(are_weakly_separated(Gamma(v{1,2}), Gamma(v{2,3})));
  EXPECT_TRUE(are_weakly_separated(Gamma(v{1,2,3}), Gamma(v{3,4,5})));
  EXPECT_TRUE(are_weakly_separated(Gamma(v{1,3,5}), Gamma(v{2,3,4})));
  EXPECT_TRUE(are_weakly_separated(Gamma(v{1,2,7,8}), Gamma(v{3,4,5,6})));
  EXPECT_TRUE(are_weakly_separated(Gamma(v{1,2,5,6}), Gamma(v{2,3,4,5})));
  EXPECT_FALSE(are_weakly_separated(Gamma(v{1,3}), Gamma(v{2,4})));
  EXPECT_FALSE(are_weakly_separated(Gamma(v{1,3,4,5}), Gamma(v{2,3,5,6})));
}

TEST(GammaTest, WeaklySeparatedAgainstNaive) {
  const int n = 32;
  for (const int b1 : range(n)) {
    for (const int b2 : range(n)) {
      Gamma g1{Gamma::BitsetT(b1)};
      Gamma g2{Gamma::BitsetT(b2)};
      EXPECT_EQ(are_weakly_separated(g1, g2), are_weakly_separated_naive(g1, g2))
          << to_string(g1) << " vs " << to_string(g2);
    }
  }
}
