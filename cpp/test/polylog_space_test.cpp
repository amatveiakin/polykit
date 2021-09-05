// Some tests use
//   [ref] https://www.brown.edu/academics/physics/sites/physics/files/images/Parker-2015.pdf#page63
// as a reference point for polylog space dimensions.

#include "lib/polylog_space.h"

#include "gtest/gtest.h"

#include "lib/itertools.h"
#include "lib/polylog_qli.h"


template<typename T>
T ptr_to_lyndon_basis(const std::shared_ptr<T>& ptr) {
  return to_lyndon_basis(*ptr);
}

template<typename SpaceF>
int simple_space_rank(const SpaceF& space, int num_points) {
  return matrix_rank(compute_polylog_space_matrix(
    space(seq_incl(1, num_points)),
    DISAMBIGUATE(ptr_to_lyndon_basis)
  ));
}


PolylogSpace L3_alternative(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi3(choose_indices_one_based(p, {1,2,3,4})));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi3(choose_indices_one_based(p, {1,3,2,4})));
    })
  );
}

PolylogSpace L4_alternative(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,2,3,4})));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,3,4,2})));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,4,2,3})));
    }),
    mapped(permutations(args.as_x(), 5), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,2,1,3,4,5})));
    })
  );
}


TEST(PolylogSpaceTest, L3SameAsAlternative) {
  for (int num_points : range_incl(4, 6)) {
    auto args = mapped(range_incl(1, num_points), [](int i) { return X(i); });
    args.back() = Inf;
    EXPECT_TRUE(polylog_space_equals(L3(args), L3_alternative(args), DISAMBIGUATE(ptr_to_lyndon_basis)));
  }
}

TEST(PolylogSpaceTest, LARGE_L4SameAsAlternative) {
  for (int num_points : range_incl(4, 6)) {
    auto args = mapped(range_incl(1, num_points), [](int i) { return X(i); });
    args.back() = Inf;
    EXPECT_TRUE(polylog_space_equals(L4(args), L4_alternative(args), DISAMBIGUATE(ptr_to_lyndon_basis)));
  }
}

TEST(PolylogSpaceTest, DimB1) {
  // (dim B1, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB1, 6), 9);
  EXPECT_EQ(simple_space_rank(CB1, 7), 14);
  EXPECT_EQ(simple_space_rank(CB1, 8), 20);
  EXPECT_EQ(simple_space_rank(CB1, 9), 27);
}

TEST(PolylogSpaceTest, DimB2) {
  // (dim B2, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB2, 6), 10);
  EXPECT_EQ(simple_space_rank(CB2, 7), 20);
  EXPECT_EQ(simple_space_rank(CB2, 8), 35);
  EXPECT_EQ(simple_space_rank(CB2, 9), 56);
}

TEST(PolylogSpaceTest, LARGE_DimB3) {
  // (dim B3, A_{n-3}) in [ref]
  EXPECT_EQ(simple_space_rank(CB3, 6), 15);
  EXPECT_EQ(simple_space_rank(CB3, 7), 35);
  EXPECT_EQ(simple_space_rank(CB3, 8), 70);
  EXPECT_EQ(simple_space_rank(CB3, 9), 126);
}
