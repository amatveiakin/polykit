
#include "lib/polylog_gr_space.h"

#include "gtest/gtest.h"

#include "lib/bigrassmannian_complex_cohomologies.h"
#include "lib/chern_arrow.h"
#include "lib/itertools.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_qli.h"
#include "lib/space_algebra.h"
#include "lib/summation.h"
#include "test_util/space_helpers.h"
#include "test_util/space_matchers.h"


ClusterCoRanks cluster_co_grl_ranks(int weight, int num_coparts, int dimension, int num_points) {
  return cluster_co_ranks(simple_co_GrL(weight, num_coparts, dimension, num_points));
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


TEST(PolylogSpaceTest, GrFxContainsGrL1) {
  for (const int dimension : range_incl(2, 3)) {
    for (const int num_points : range_incl(4, 8)) {
      const auto points = seq_incl(1, num_points);
      EXPECT_POLYLOG_SPACE_CONTAINS(
        GrFx(dimension, points),
        GrL1(dimension, points),
        DISAMBIGUATE(to_lyndon_basis)
      );
    }
  }
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, LARGE_ClusterCoGrL_Dim3_Weight3) {
  EXPECT_EQ(cluster_co_grl_ranks(3, 2, 3, 5), (ClusterCoRanks{5, 5}));
  EXPECT_EQ(cluster_co_grl_ranks(3, 3, 3, 5), (ClusterCoRanks{0, 0}));
  EXPECT_EQ(cluster_co_grl_ranks(3, 2, 3, 6), (ClusterCoRanks{90, 31}));
  EXPECT_EQ(cluster_co_grl_ranks(3, 3, 3, 6), (ClusterCoRanks{85, 85}));
}
TEST(PolylogSpaceTest, HUGE_ClusterCoGrL_Dim3_Weight3) {
  EXPECT_EQ(cluster_co_grl_ranks(3, 2, 3, 7), (ClusterCoRanks{539, 111}));
  EXPECT_EQ(cluster_co_grl_ranks(3, 3, 3, 7), (ClusterCoRanks{756, 756}));
}

// Comparing against results previously computed by the app.
TEST(PolylogSpaceTest, HUGE_ClusterCoGrL_Dim3_Weight4) {
  EXPECT_EQ(cluster_co_grl_ranks(4, 2, 3, 6), (ClusterCoRanks{128, 33}));
  EXPECT_EQ(cluster_co_grl_ranks(4, 3, 3, 6), (ClusterCoRanks{153, 95}));
  EXPECT_EQ(cluster_co_grl_ranks(4, 4, 3, 6), (ClusterCoRanks{61, 61}));
}

TEST(PolylogSpaceTest, LARGE_GrL1IndependentOfFixedPoints) {
  static const auto GrL1_zero_fixed_points = [](int dimension, const std::vector<int>& args) {
    return GrL_core(1, dimension, args, true, 0);
  };
  for (const int dimension : range_incl(2, 4)) {
    for (const int num_points : range_incl(5, 7)) {
      const auto points = seq_incl(1, num_points);
      EXPECT_POLYLOG_SPACE_EQ(
        GrL1(dimension, points),
        GrL1_zero_fixed_points(dimension, points),
        DISAMBIGUATE(to_lyndon_basis)
      );
    }
  }
}

TEST(PolylogSpaceTest, LARGE_GrL2IndependentOfFixedPoints) {
  static const auto GrL2_zero_fixed_points = [](int dimension, const std::vector<int>& args) {
    return GrL_core(2, dimension, args, true, 0);
  };
  for (const int dimension : range_incl(2, 4)) {
    for (const int num_points : range_incl(5, 7)) {
      const auto points = seq_incl(1, num_points);
      EXPECT_POLYLOG_SPACE_EQ(
        GrL2(dimension, points),
        GrL2_zero_fixed_points(dimension, points),
        DISAMBIGUATE(to_lyndon_basis)
      );
    }
  }
}

TEST(PolylogSpaceTest, LARGE_L2Fx_contains_GrQLi3) {
  const int num_points = 5;
  const int dimension = 3;
  const auto points = seq_incl(1, num_points);
  const auto space = space_ncoproduct(GrFx(dimension, points), GrL2(dimension, points));
  const auto grqli3_space = mapped(
    range(num_points),
    [&](int bonus_arg_idx) {
      const auto bonus_args = std::vector{points[bonus_arg_idx]};
      const auto main_args = removed_index(points, bonus_arg_idx);
      return ncomultiply(GrQLiVec(3, bonus_args, main_args));
    }
  );
  EXPECT_TRUE(space_contains(space, grqli3_space, std::identity{}));
}

TEST(PolylogSpaceTest, LARGE_ClusterL2Fx_contains_R43Sum) {
  const int num_points = 5;
  const int dimension = 3;
  const auto points = seq_incl(1, num_points);
  Gr_NCoSpace space = mapped(
    filtered(
      space_ncoproduct(GrFx(dimension, points), GrL2(dimension, points)),
      DISAMBIGUATE(is_totally_weakly_separated)
    ),
    DISAMBIGUATE(ncomultiply)
  );
  const auto expr = sum_looped_vec(R_4_3, 5, {1,2,3,4});
  EXPECT_TRUE(space_contains(space, {expr}, std::identity{}));
}


class GLiVsSpacesTest : public ::testing::TestWithParam<std::pair<int, int>> {
public:
  int weight() const { return GetParam().first; }
  int dimension() const { return GetParam().second; }
};
// Note: GLi requires that (weight >= dimension - 1).
INSTANTIATE_TEST_SUITE_P(LARGE_Cases, GLiVsSpacesTest, ::testing::Values(
  std::pair{2, 3},
  std::pair{3, 3}
));
INSTANTIATE_TEST_SUITE_P(HUGE_Cases, GLiVsSpacesTest, ::testing::Values(
  std::pair{4, 3}
));

TEST_P(GLiVsSpacesTest, GrL_contains_GLi) {
  const int num_points = dimension() * 2;
  const auto points = seq_incl(1, num_points);
  const auto expr = GLiVec(weight(), points);
  const auto space = GrL(weight(), dimension(), points);
  EXPECT_TRUE(space_contains(space, {expr}, DISAMBIGUATE(to_lyndon_basis)));
}

TEST_P(GLiVsSpacesTest, CoCGrL_contains_GLiNcomultiplied) {
  const int num_points = dimension() * 2;
  const auto points = seq_incl(1, num_points);
  const auto expr = GLiVec(weight(), points);
  const auto co_space = simple_co_CGrL_test_space(weight(), dimension(), num_points);
  EXPECT_TRUE(space_contains(
    co_space,
    {ncomultiply(normalize_remove_consecutive(expr))},
    DISAMBIGUATE(to_lyndon_basis)
  ));
}

TEST_P(GLiVsSpacesTest, CoGrL_contains_GLiExpandedIntoGluedPairs) {
  if (weight() != dimension() - 1) {
    // TODO: Is it the right condition for when this should hold?
    return;
  }
  const int num_points = dimension() * 2;
  const auto points = seq_incl(1, num_points);
  const auto expr = GLiVec(weight(), points);
  const auto co_space = abstract_co_space(
    weight(),
    weight() - 1,
    [&](const int w) {
      switch (w) {
        case 1: return GrFx(dimension(), points);
        case 2: return GrL2(dimension(), points);
        default: FATAL("Unexpected part weight");
      }
    },
    DISAMBIGUATE(acoproduct_vec)
  );
  EXPECT_TRUE(space_contains(
    co_space,
    {expand_into_glued_pairs(expr)},
    std::identity{}
  ));
}


TEST(PolylogSpaceTest, GrL3Dim4_contains_QLiVec3PluckerDual) {
  const std::vector points = {1,2,3,4,5,6};
  const auto expr = plucker_dual(QLiVec(3, points), points);
  const auto space = GrL3(4, points);
  EXPECT_TRUE(space_contains(space, {expr}, DISAMBIGUATE(to_lyndon_basis)));
}

// TODO: Tests for CGrL ranks (check which ranks are knows for sure)

TEST(PolylogSpaceTest, LARGE_GrL3Dim3ProjectionKernelVsOnePointLess) {
  for (const int num_points : range_incl(4, 8)) {
    const auto points = seq_incl(1, num_points);
    const auto ranks = space_mapping_ranks(
      mapped(GrL3(3, points), DISAMBIGUATE(to_lyndon_basis)),
      std::identity{},
      [](const auto& expr) {
        return project_on(1, expr);
      }
    );
    const auto lower_rank = space_rank(GrL3(3, slice(points, 1)), DISAMBIGUATE(to_lyndon_basis));
    EXPECT_EQ(ranks.kernel(), lower_rank);
  }
}

TEST(PolylogSpaceTest, LARGE_ClusterGrL3AsKernel) {
  const int weight = 3;
  const int dimension = 3;
  // Checked up to 10 points.
  for (const int num_points : range_incl(5, 8)) {
    const auto points = seq_incl(1, num_points);
    const auto co_space = simple_co_CGrL_test_space(weight, dimension, num_points);
    const auto mapping_ranks = space_mapping_ranks(co_space, std::identity{}, [](const auto& expr) {
      return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
    });
    const int cgrl_rank = space_rank(CGrL_test_space(weight, dimension, points), DISAMBIGUATE(to_lyndon_basis));
    EXPECT_EQ(mapping_ranks.kernel(), cgrl_rank);
  }
}

#if 0
  Gr_NCoSpace space_lyndon;
  const auto coords = combinations(points, dimension);
  for (const auto& word : get_lyndon_words(coords, weight)) {
    const auto term = mapped(word, convert_to<Gamma>);
    if (is_weakly_separated(term) && passes_normalize_remove_consecutive(term, dimension, num_points)) {
      space_lyndon.push_back(ncomultiply(GammaExpr::single(term)));
    }
  }
  const auto space_product = simple_co_CGrL_test_space(weight, dimension, num_points);
  const auto ranks = space_venn_ranks(space_lyndon, space_product, std::identity{});
  EXPECT_EQ(ranks.intersected(), ...);  // TODO: What is this?
#endif

TEST(PolylogSpaceTest, HUGE_CGrLCohomologies) {
  const auto compute = [](int weight, int dimension, int num_points) {
    return bigrassmannian_complex_cohomology(dimension, num_points, [&](const int dim, const auto& points) {
      return CGrL_test_space(weight, dim, points);
    });
  };
  EXPECT_EQ(compute(3, 3, 5), 0);
  EXPECT_EQ(compute(3, 3, 6), 1);
  EXPECT_EQ(compute(3, 3, 7), 0);
  EXPECT_EQ(compute(3, 3, 8), 1);
  EXPECT_EQ(compute(4, 3, 5), 0);
  EXPECT_EQ(compute(4, 3, 6), 0);
  EXPECT_EQ(compute(4, 3, 7), 0);
  EXPECT_EQ(compute(4, 3, 8), 2);
  EXPECT_EQ(compute(3, 4, 5), 0);
  EXPECT_EQ(compute(3, 4, 6), 0);
  EXPECT_EQ(compute(3, 4, 7), 0);
  EXPECT_EQ(compute(3, 4, 8), 1);
  EXPECT_EQ(compute(4, 4, 5), 0);
  EXPECT_EQ(compute(4, 4, 6), 0);
  EXPECT_EQ(compute(4, 4, 7), 0);
  EXPECT_EQ(compute(4, 4, 8), 2);
}

TEST(PolylogSpaceTest, HUGE_WedgeChernGrLCohomologies) {
  for (const int dimension : range_incl(3, 4)) {
    for (const int weight : range_incl(3, 4)) {
      for (const int num_points : range_incl(5, 8)) {
        const auto rank = bigrassmannian_complex_cohomology(dimension, num_points, [&](const int dim, const auto& points) {
          return wedge_ChernGrL(weight, dim, points);
        });
        EXPECT_EQ(rank, 0);
      }
    }
  }
}
