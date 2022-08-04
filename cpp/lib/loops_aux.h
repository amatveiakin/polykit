// Helper functions for dealing with LoopExpr which are too specific for loops.h.

#pragma once

#include "loops.h"


// Tries to eliminate as many `target_type` type term in `victim` as possible by adding
// `killer` expression with different planar transformations.
LoopExpr auto_kill_planar(LoopExpr victim, const LoopExpr& killer, int target_type);

// Tries to eliminate as many `target_type` type term in `victim` as possible by adding
// `killer` expression with different permutations.
// TODO: Add linear algebra support
LoopExpr auto_kill(LoopExpr victim, const LoopExpr& killer, int target_type);

void arg11_list_all_degenerations(const LoopExpr& expr);

StringExpr arg9_expr_type_1_to_column(const LoopExpr& expr);

StringExpr arg11_expr_type_2_to_column(const LoopExpr& expr);

// Sorts (with sign) the first two loops in a triple assuming their have the same size.
// This is in fact a suffle performed under the assumption that terms with 5-loop in the
// middle are zero.
LoopExpr arg9_kill_middle(const LoopExpr& expr);

// A step in "3.5. Proof of the higher Gangl formula in weights four and six"
//   in https://arxiv.org/pdf/2208.01564v1.pdf
// Removes certain kinds of loops that have been proven to vanish by that points.
LoopExpr reduce_arg9_loop_expr(const LoopExpr& expr);
