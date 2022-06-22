#include "lib/linalg_solvers.h"

#include "gtest/gtest.h"

#include "lib/polylog_gr_space.h"
#include "lib/polylog_type_ac_space.h"
#include "lib/polylog_qli.h"


TEST(LinalgSolversTest, SpaceBasis) {
  const auto check_space = [](const auto& full) {
    const int rank = space_rank(full, DISAMBIGUATE(to_lyndon_basis));
    const auto basis = space_basis(full, DISAMBIGUATE(to_lyndon_basis));
    EXPECT_TRUE(space_equal(basis, full, DISAMBIGUATE(to_lyndon_basis)));
    EXPECT_EQ(rank, basis.size());
  };
  check_space(std::vector{QLi1(1,2,3,4), QLi1(1,2,3,4) + QLi1(1,2,3,5), QLi1(1,2,3,5)});
  check_space(CB_naive_via_QLi_fours(2, {1,2,3,4,5}));
  check_space(CB_naive_via_QLi_fours(2, {1,2,3,4,5,6}));
  check_space(GrL1(3, {1,2,3,4,5,6}));
  check_space(GrL2(3, {1,2,3,4,5,6}));
}
