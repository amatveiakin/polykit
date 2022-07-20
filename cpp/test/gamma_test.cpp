#include "lib/gamma.h"

#include "gtest/gtest.h"

#include "lib/bitset_util.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"
#include "lib/itertools.h"
#include "lib/set_util.h"
#include "test_util/matchers.h"


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

static GammaExpr pullback_naive(const GammaExpr& expr, const std::vector<int>& bonus_points) {
  return expr.mapped([&](const auto& term) {
    return mapped(term, [&](const Gamma& g_src) {
      return Gamma(concat(bonus_points, g_src.index_vector()));
    });
  });
}

static GammaExpr plucker_dual_naive(const GammaExpr& expr, const std::vector<int>& point_universe) {
  return expr.mapped([&](const auto& term) {
    return mapped(term, [&](const Gamma& g_src) {
      return Gamma(set_difference(point_universe, g_src.index_vector()));
    });
  });
}


TEST(GammaTest, SubstituteVariables) {
  EXPECT_EXPR_EQ(
    substitute_variables_1_based(
      + G({1,2,3})
      + G({2,3,4})
      - G({1,3,5}),
      {2,1,4,4,5}
    ),
    + G({1,2,4})
    - G({2,4,5})
  );
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

TEST(GammaTest, PullbackAgainstNaive) {
  const std::vector bonus_points = {2};
  const auto expr = tensor_product(G({1,3,4}), G({3,4,5}));
  ASSERT_EQ(pullback(expr, bonus_points), pullback_naive(expr, bonus_points));
}

TEST(GammaTest, PluckerDualAgainstNaive) {
  const std::vector points = {1,2,3,4,5,6};
  const auto expr = tensor_product(G({1,2,3}), G({1,3,5}));
  ASSERT_EQ(plucker_dual(expr, points), plucker_dual_naive(expr, points));
}

TEST(GammaTest, NormalizeRemoveConsecutive_Linear) {
  // When GCD(dimension, number_of_points) > 1, the normalization cannot wrap.
  EXPECT_TRUE(passes_normalize_remove_consecutive(std::vector{Gamma({1,3}), Gamma({2,4})}, 2, 4));
  EXPECT_FALSE(passes_normalize_remove_consecutive(std::vector{Gamma({1,2}), Gamma({2,4})}, 2, 4));
  EXPECT_TRUE(passes_normalize_remove_consecutive(std::vector{Gamma({1,3}), Gamma({4,1})}, 2, 4));
}

TEST(GammaTest, NormalizeRemoveConsecutive_Circular) {
  // When GCD(dimension, number_of_points) == 1, the normalization can wrap.
  EXPECT_TRUE(passes_normalize_remove_consecutive(std::vector{Gamma({1,3}), Gamma({2,5})}, 2, 5));
  EXPECT_FALSE(passes_normalize_remove_consecutive(std::vector{Gamma({1,2}), Gamma({2,5})}, 2, 5));
  EXPECT_FALSE(passes_normalize_remove_consecutive(std::vector{Gamma({1,3}), Gamma({5,1})}, 2, 5));
}

TEST(GammaTest, SymmetrizeComposition) {
  // Should be true for any expressions.
  for (const auto& expr: {GrQLi2(1)(2,3,4,5,6,7), GrLi(1)(2,3,4,5), G({1,2,3,4,5})}) {
    const int n = detect_num_variables(expr);
    EXPECT_EXPR_ZERO(symmetrize_double_1_based(symmetrize_loop_1_based(expr, n), n));
  }
}
