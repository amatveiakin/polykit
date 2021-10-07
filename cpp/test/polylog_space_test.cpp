// Some tests use
//   [ref] https://www.brown.edu/academics/physics/sites/physics/files/images/Parker-2015.pdf#page63
// as a reference point for polylog space ranks.

#include "lib/polylog_space.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_qli.h"
#include "lib/summation.h"


template<typename SpaceT, typename PrepareF>
void EXPECT_POLYLOG_SPACE_EQ(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto ranks = space_venn_ranks(a, b, prepare);
  EXPECT_TRUE(ranks.are_equal()) << to_string(ranks);
}

template<typename SpaceF>
int simple_space_rank(const SpaceF& space, int num_points) {
  return matrix_rank(space_matrix(
    space(to_vector(range_incl(1, num_points))),
    DISAMBIGUATE(to_lyndon_basis)
  ));
}

struct ClusterCoLRanks {
  int space = 0;
  int kernel = 0;
  bool operator==(const ClusterCoLRanks& other) const { return space == other.space && kernel == other.kernel; }
};

ClusterCoLRanks cluster_co_l_ranks(int weight, int num_coparts, int num_points) {
  const auto space = simple_co_L(weight, num_coparts, num_points);
  const int space_rank = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
    return keep_non_weakly_separated(expr);
  }).kernel();
  const int kernel_rank = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
    return std::make_tuple(keep_non_weakly_separated(expr), ncomultiply(expr));
  }).kernel();
  return {space_rank, kernel_rank};
}

GammaNCoExpr R_4_3(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 4);
  return sum_looped_vec([&](const std::vector<int>& args) {
    const auto get_args = [&](const std::vector<int>& indices) {
      // TODO: Change `sum` so that it doesn't requires nested `choose_indices_one_based`.
      return choose_indices_one_based(points, choose_indices_one_based(args, indices));
    };
    return ncoproduct(G(get_args({1,2,3})), G(get_args({1,2,4})), G(get_args({1,3,4})));
  }, 4, {1,2,3,4}, SumSign::alternating);
}

PolylogSpace L3_alternative(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi3(choose_indices_one_based(p, {1,2,3,4}));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return QLi3(choose_indices_one_based(p, {1,3,2,4}));
    })
  );
}

PolylogSpace L4_alternative(const XArgs& args) {
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
TEST(PolylogSpaceTest, LARGE_ClusterCoL_Dim2_Weight3) {
  EXPECT_EQ(cluster_co_l_ranks(3, 2, 5), (ClusterCoLRanks{5, 5}));
  EXPECT_EQ(cluster_co_l_ranks(3, 3, 5), (ClusterCoLRanks{0, 0}));
  EXPECT_EQ(cluster_co_l_ranks(3, 2, 6), (ClusterCoLRanks{30, 15}));
  EXPECT_EQ(cluster_co_l_ranks(3, 3, 6), (ClusterCoLRanks{16, 16}));
  EXPECT_EQ(cluster_co_l_ranks(3, 2, 7), (ClusterCoLRanks{105, 35}));
  EXPECT_EQ(cluster_co_l_ranks(3, 3, 7), (ClusterCoLRanks{84, 84}));
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, LARGE_ClusterCoL_Dim2_Weight4) {
  EXPECT_EQ(cluster_co_l_ranks(4, 2, 5), (ClusterCoLRanks{5, 5}));
  EXPECT_EQ(cluster_co_l_ranks(4, 3, 5), (ClusterCoLRanks{0, 0}));
  EXPECT_EQ(cluster_co_l_ranks(4, 2, 6), (ClusterCoLRanks{39, 16}));
  EXPECT_EQ(cluster_co_l_ranks(4, 3, 6), (ClusterCoLRanks{26, 23}));
  EXPECT_EQ(cluster_co_l_ranks(4, 4, 6), (ClusterCoLRanks{3, 3}));
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, HUGE_ClusterCoL_Dim2_Weight5) {
  EXPECT_EQ(cluster_co_l_ranks(5, 2, 5), (ClusterCoLRanks{5, 5}));
  EXPECT_EQ(cluster_co_l_ranks(5, 3, 5), (ClusterCoLRanks{0, 0}));
  EXPECT_EQ(cluster_co_l_ranks(5, 2, 6), (ClusterCoLRanks{43, 16}));
  EXPECT_EQ(cluster_co_l_ranks(5, 3, 6), (ClusterCoLRanks{33, 27}));
  EXPECT_EQ(cluster_co_l_ranks(5, 4, 6), (ClusterCoLRanks{6, 6}));
  EXPECT_EQ(cluster_co_l_ranks(5, 5, 6), (ClusterCoLRanks{0, 0}));
}

TEST(PolylogSpaceTest, LARGE_L2Fx_contains_GrQLi3) {
  const int num_points = 5;
  const int dimension = 3;
  const auto points = to_vector(range_incl(1, num_points));
  const GrPolylogSpace fx = GrFx(dimension, points);
  const GrPolylogSpace l2 = GrL2(dimension, points);
  const GrPolylogNCoSpace space = mapped(
    cartesian_product(l2, fx),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
  const GrPolylogNCoSpace grqli3_space = mapped(
    range(num_points),
    [&](int bonus_arg_idx) {
      const auto bonus_args = std::vector{points[bonus_arg_idx]};
      const auto main_args = removed_index(points, bonus_arg_idx);
      return ncomultiply(GrQLiVec(3, bonus_args, main_args));
    }
  );
  EXPECT_TRUE(space_contains(space, grqli3_space, DISAMBIGUATE(identity_function)));
}

TEST(PolylogSpaceTest, LARGE_ClusterL2Fx_contains_R43Sum) {
  const int num_points = 5;
  const int dimension = 3;
  const auto points = to_vector(range_incl(1, num_points));
  const GrPolylogSpace fx = GrFx(dimension, points);
  const GrPolylogSpace l2 = GrL2(dimension, points);
  GrPolylogNCoSpace space;
  for (const auto& [a, b] : cartesian_product(l2, fx)) {
    const auto expr = ncoproduct(a, b);
    if (is_totally_weakly_separated(expr)) {
      space.push_back(ncomultiply(expr));
    }
  };
  const auto expr = sum_looped_vec(R_4_3, 5, {1,2,3,4});
  EXPECT_TRUE(space_contains(space, {expr}, DISAMBIGUATE(identity_function)));
}
