// Some tests use
//   [ref] https://www.brown.edu/academics/physics/sites/physics/files/images/Parker-2015.pdf#page63
// as a reference point for polylog space ranks.

#include "lib/polylog_type_ac_space.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "lib/polylog_qli.h"
#include "lib/summation.h"
#include "test_util/space_helpers.h"
#include "test_util/space_matchers.h"


ClusterCoRanks cluster_co_l_ranks(int weight, int num_coparts, int num_points) {
  return cluster_co_ranks(simple_co_L(weight, num_coparts, num_points));
}

TypeAC_Space L3_alternative(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi3(choose_indices_one_based(p, {1,2,3,4}));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi3(choose_indices_one_based(p, {1,3,2,4}));
    })
  );
}

TypeAC_Space L4_alternative(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi4(choose_indices_one_based(p, {1,2,3,4}));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi4(choose_indices_one_based(p, {1,3,4,2}));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi4(choose_indices_one_based(p, {1,4,2,3}));
    }),
    mapped(permutations(args.as_x(), 5), [](const auto& p) {
      return QLi4(choose_indices_one_based(p, {1,2,1,3,4,5}));
    })
  );
}


class PolylogSpaceTest_Weight_NumPoints : public ::testing::TestWithParam<std::pair<int, int>> {
public:
  int weight() const { return GetParam().first; }
  int num_points() const { return GetParam().second; }
};

INSTANTIATE_TEST_SUITE_P(Cases, PolylogSpaceTest_Weight_NumPoints, ::testing::Values(
  std::pair{1, 4},
  std::pair{1, 6},
  std::pair{1, 8},
  std::pair{2, 4},
  std::pair{2, 6},
  std::pair{3, 4}
));
INSTANTIATE_TEST_SUITE_P(LARGE_Cases, PolylogSpaceTest_Weight_NumPoints, ::testing::Values(
  std::pair{2, 8},
  std::pair{3, 6},
  std::pair{3, 8},
  std::pair{4, 4},
  std::pair{4, 6}
));


TEST(PolylogSpaceTest, L3SameAsAlternative) {
  for (int num_points : range_incl(4, 6)) {
    auto points = mapped(range_incl(1, num_points), convert_to<X>);
    points.back() = Inf;
    EXPECT_POLYLOG_SPACE_EQ(L(3, points), L3_alternative(points), DISAMBIGUATE(to_lyndon_basis));
  }
}

TEST(PolylogSpaceTest, LARGE_L4SameAsAlternative) {
  for (int num_points : range_incl(4, 6)) {
    auto points = mapped(range_incl(1, num_points), convert_to<X>);
    points.back() = Inf;
    EXPECT_POLYLOG_SPACE_EQ(L(4, points), L4_alternative(points), DISAMBIGUATE(to_lyndon_basis));
  }
}

TEST_P(PolylogSpaceTest_Weight_NumPoints, LSameAsLInf) {
  auto points = mapped(range_incl(1, num_points()), [](int i) { return X(i); });
  points.back() = Inf;
  EXPECT_POLYLOG_SPACE_EQ(L(weight(), points), LInf(weight(), points), DISAMBIGUATE(to_lyndon_basis));
}

TEST(PolylogSpaceTest, CB2SameAsNaive) {
  const int weight = 2;
  for (int num_points : range_incl(4, 6)) {
    auto points = to_vector(range_incl(1, num_points));
    EXPECT_POLYLOG_SPACE_EQ(
      CB(weight, points),
      CB_naive_via_QLi_fours(weight, points),
      DISAMBIGUATE(to_lyndon_basis)
    );
  }
}

TEST(PolylogSpaceTest, RankCB1) {
  // (dim B1, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB1, 6), 9);
  EXPECT_EQ(simple_space_rank(CB1, 7), 14);
  EXPECT_EQ(simple_space_rank(CB1, 8), 20);
  EXPECT_EQ(simple_space_rank(CB1, 9), 27);
}

TEST(PolylogSpaceTest, RankCB2) {
  // (dim B2, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB2, 6), 10);
  EXPECT_EQ(simple_space_rank(CB2, 7), 20);
  EXPECT_EQ(simple_space_rank(CB2, 8), 35);
  EXPECT_EQ(simple_space_rank(CB2, 9), 56);
}

TEST(PolylogSpaceTest, LARGE_RankCB3) {
  // (dim B3, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB3, 6), 15);
  EXPECT_EQ(simple_space_rank(CB3, 7), 35);
  EXPECT_EQ(simple_space_rank(CB3, 8), 70);
  EXPECT_EQ(simple_space_rank(CB3, 9), 126);
}

TEST(PolylogSpaceTest, LARGE_CLIsClusterL) {
  for (const int weight : range_incl(1, 5)) {
    for (const int num_points : range_incl(5, 6)) {
      const auto points = to_vector(range_incl(1, num_points));
      const auto cl = mapped(CL(weight, points), DISAMBIGUATE(to_lyndon_basis));
      const auto l = mapped(L(weight, points), DISAMBIGUATE(to_lyndon_basis));
      const int cl_rank = space_rank(cl, DISAMBIGUATE(identity_function));
      const auto l_mapping_ranks = space_mapping_ranks(l, DISAMBIGUATE(identity_function), DISAMBIGUATE(keep_non_weakly_separated));
      EXPECT_EQ(cl_rank, l_mapping_ranks.kernel()) << "w=" << weight << ", p=" << num_points;
    }
  }
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, LARGE_ClusterCoL_Weight3) {
  EXPECT_EQ(cluster_co_l_ranks(3, 2, 5), (ClusterCoRanks{5, 5}));
  EXPECT_EQ(cluster_co_l_ranks(3, 3, 5), (ClusterCoRanks{0, 0}));
  EXPECT_EQ(cluster_co_l_ranks(3, 2, 6), (ClusterCoRanks{30, 15}));
  EXPECT_EQ(cluster_co_l_ranks(3, 3, 6), (ClusterCoRanks{16, 16}));
  EXPECT_EQ(cluster_co_l_ranks(3, 2, 7), (ClusterCoRanks{105, 35}));
  EXPECT_EQ(cluster_co_l_ranks(3, 3, 7), (ClusterCoRanks{84, 84}));
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, LARGE_ClusterCoL_Weight4) {
  EXPECT_EQ(cluster_co_l_ranks(4, 2, 5), (ClusterCoRanks{5, 5}));
  EXPECT_EQ(cluster_co_l_ranks(4, 3, 5), (ClusterCoRanks{0, 0}));
  EXPECT_EQ(cluster_co_l_ranks(4, 2, 6), (ClusterCoRanks{39, 16}));
  EXPECT_EQ(cluster_co_l_ranks(4, 3, 6), (ClusterCoRanks{26, 23}));
  EXPECT_EQ(cluster_co_l_ranks(4, 4, 6), (ClusterCoRanks{3, 3}));
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, HUGE_ClusterCoL_Weight5) {
  EXPECT_EQ(cluster_co_l_ranks(5, 2, 5), (ClusterCoRanks{5, 5}));
  EXPECT_EQ(cluster_co_l_ranks(5, 3, 5), (ClusterCoRanks{0, 0}));
  EXPECT_EQ(cluster_co_l_ranks(5, 2, 6), (ClusterCoRanks{43, 16}));
  EXPECT_EQ(cluster_co_l_ranks(5, 3, 6), (ClusterCoRanks{33, 27}));
  EXPECT_EQ(cluster_co_l_ranks(5, 4, 6), (ClusterCoRanks{6, 6}));
  EXPECT_EQ(cluster_co_l_ranks(5, 5, 6), (ClusterCoRanks{0, 0}));
}

#if ENABLE_NEGATIVE_DELTA_VARIABLES
TEST(PolylogSpaceTest, LARGE_CLInvGluedPairs) {
  const std::vector points = {x1,x2,x3,-x1,-x2,-x3};
  auto cl1 = CL1_inv(points);
  auto cl2 = CL2_inv(points);
  cl1 = space_basis(cl1, DISAMBIGUATE(to_lyndon_basis));
  cl2 = space_basis(cl2, DISAMBIGUATE(to_lyndon_basis));
  const auto space_a = mapped(
    get_lyndon_words(cl1, 4),
    [](const auto& components) {
      return expand_into_glued_pairs(tensor_product(absl::MakeConstSpan(components)));
    }
  );
  const auto space_b = mapped(
    cartesian_product(cl2, cl1, cl1),
    applied(DISAMBIGUATE(acoproduct))
  );
  const auto ranks = space_venn_ranks(space_a, space_b, DISAMBIGUATE(identity_function));
  // Note. Testing against previously computed result; no alternative proof known.
  EXPECT_EQ(ranks.intersected(), 42);
}
#endif
