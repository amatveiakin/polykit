#pragma once

#include "gmock/gmock.h"

#include "lib/lyndon.h"


template<typename LinearT>
void EXPECT_EXPR_EQ(const LinearT& lhs, const LinearT& rhs) {
  const LinearT diff = lhs - rhs;
  EXPECT_TRUE(diff.is_zero()) << "\nLHS " << lhs << "RHS " << rhs << "Diff " << diff;
}

template<typename LinearT>
void EXPECT_EXPR_ZERO(const LinearT& expr) {
  EXPECT_TRUE(expr.is_zero()) << "\nExpression " << expr;
}

template<typename LinearT>
void EXPECT_EXPR_EQ_AFTER_LYNDON(const LinearT& lhs, const LinearT& rhs) {
  const LinearT diff = lhs - rhs;
  const LinearT diff_lyndon = to_lyndon_basis(diff);
  EXPECT_TRUE(diff_lyndon.is_zero()) << "\n"
    << "LHS " << lhs << "RHS " << rhs
    << "Diff before Lyndon " << diff
    << "Diff after Lyndon " << diff_lyndon;
}

template<typename LinearT>
void EXPECT_EXPR_ZERO_AFTER_LYNDON(const LinearT& expr) {
  const LinearT expr_lyndon = to_lyndon_basis(expr);
  EXPECT_TRUE(expr_lyndon.is_zero()) << "\n"
    << "Before Lyndon " << expr
    << "After Lyndon " << expr_lyndon;
}
