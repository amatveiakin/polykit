#pragma once

#include "gmock/gmock.h"


// TODO: EXPECT_EXPR_EQ_AFTER_LYNDON
template<typename LinearT>
void EXPECT_EXPR_EQ(const LinearT& lhs, const LinearT& rhs) {
  const LinearT& diff = lhs - rhs;
  EXPECT_TRUE(diff.zero()) << "LHS " << lhs << "\nRHS " << rhs << "\nDiff " << diff;
}
