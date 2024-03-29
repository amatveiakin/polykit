#pragma once

#include "gmock/gmock.h"

#include "cpp/lib/space_algebra.h"


template<typename SpaceT, typename PrepareF>
void EXPECT_POLYLOG_SPACE_EQ(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto ranks = space_venn_ranks(a, b, prepare);
  EXPECT_TRUE(ranks.are_equal()) << to_string(ranks);
}

template<typename SpaceT, typename PrepareF>
void EXPECT_POLYLOG_SPACE_CONTAINS(const SpaceT& haystack, const SpaceT& needle, const PrepareF& prepare) {
  const auto ranks = space_venn_ranks(haystack, needle, prepare);
  EXPECT_TRUE(ranks.a() == ranks.united()) << to_string(ranks);
}
