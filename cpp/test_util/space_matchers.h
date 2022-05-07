#pragma once

#include "gmock/gmock.h"


template<typename SpaceT, typename PrepareF>
void EXPECT_POLYLOG_SPACE_EQ(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto ranks = space_venn_ranks(a, b, prepare);
  EXPECT_TRUE(ranks.are_equal()) << to_string(ranks);
}
