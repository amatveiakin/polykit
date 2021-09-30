// Some tests use
//   [ref] https://www.brown.edu/academics/physics/sites/physics/files/images/Parker-2015.pdf#page63
// as a reference point for polylog space dimensions.

#include "lib/polylog_space.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "lib/polylog_qli.h"


template<typename SpaceT, typename PrepareF>
void EXPECT_POLYLOG_SPACE_EQUALS(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto dim = compute_polylog_space_dimensions(a, b, prepare);
  // TODO: Use `fmt` for the union sign.
  EXPECT_TRUE(all_equal(absl::MakeConstSpan({dim.a, dim.b, dim.united})))
      << "Expected spaces to be equal, found: " << dim.a << " ∪ " << dim.b << " = " << dim.united;
}

template<typename SpaceF>
int simple_space_rank(const SpaceF& space, int num_points) {
  return matrix_rank(compute_polylog_space_matrix(
    space(to_vector(range_incl(1, num_points))),
    DISAMBIGUATE(to_lyndon_basis)
  ));
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
    auto points = mapped(range_incl(1, num_points), [](int i) { return X(i); });
    points.back() = Inf;
    EXPECT_POLYLOG_SPACE_EQUALS(L(3, points), L3_alternative(points), DISAMBIGUATE(to_lyndon_basis));
  }
}

TEST(PolylogSpaceTest, LARGE_L4SameAsAlternative) {
  for (int num_points : range_incl(4, 6)) {
    auto points = mapped(range_incl(1, num_points), [](int i) { return X(i); });
    points.back() = Inf;
    EXPECT_POLYLOG_SPACE_EQUALS(L(4, points), L4_alternative(points), DISAMBIGUATE(to_lyndon_basis));
  }
}

TEST_P(PolylogSpaceTest_Weight_NumPoints, LSameAsLInf) {
  auto points = mapped(range_incl(1, num_points()), [](int i) { return X(i); });
  points.back() = Inf;
  EXPECT_POLYLOG_SPACE_EQUALS(L(weight(), points), LInf(weight(), points), DISAMBIGUATE(to_lyndon_basis));
}

TEST(PolylogSpaceTest, DimCB1) {
  // (dim B1, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB1, 6), 9);
  EXPECT_EQ(simple_space_rank(CB1, 7), 14);
  EXPECT_EQ(simple_space_rank(CB1, 8), 20);
  EXPECT_EQ(simple_space_rank(CB1, 9), 27);
}

TEST(PolylogSpaceTest, DimCB2) {
  // (dim B2, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB2, 6), 10);
  EXPECT_EQ(simple_space_rank(CB2, 7), 20);
  EXPECT_EQ(simple_space_rank(CB2, 8), 35);
  EXPECT_EQ(simple_space_rank(CB2, 9), 56);
}

TEST(PolylogSpaceTest, LARGE_DimCB3) {
  // (dim B3, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB3, 6), 15);
  EXPECT_EQ(simple_space_rank(CB3, 7), 35);
  EXPECT_EQ(simple_space_rank(CB3, 8), 70);
  EXPECT_EQ(simple_space_rank(CB3, 9), 126);
}
