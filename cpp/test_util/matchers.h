#pragma once

#include "gmock/gmock.h"

#include "lib/lyndon.h"


template<typename LinearT>
void EXPECT_EXPR_EQ(const LinearT& lhs, const LinearT& rhs) {
  const LinearT diff = lhs - rhs;
  EXPECT_TRUE(diff.zero()) << "LHS " << lhs << "\nRHS " << rhs << "\nDiff " << diff;
}

template<typename LinearT>
void EXPECT_EXPR_EQ_AFTER_LYNDON(const LinearT& lhs, const LinearT& rhs) {
  const LinearT diff = lhs - rhs;
  const LinearT diff_lyndon = to_lyndon_basis(diff);
  EXPECT_TRUE(diff_lyndon.zero()) << "LHS " << lhs << "\nRHS " << rhs
                                  << "\nDiff before Lyndon " << diff
                                  << "\nDiff after Lyndon " << diff_lyndon;
}
