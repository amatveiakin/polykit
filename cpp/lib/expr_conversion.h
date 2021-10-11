// Conversion between expression types.
// Note: DeltaExpr <-> ThetaExpr conversion is in theta.h in order to avoid circular dependency.

#pragma once

#include "delta.h"
#include "gamma.h"
#include "theta.h"


// Requires that each term is a simple variable difference, i.e. terms like (x_i + x_j)
// or (x_i + 0) are not allowed.
GammaExpr delta_expr_to_gamma_expr(const DeltaExpr& expr);

// Requires that expression is dimension 2.
DeltaExpr gamma_expr_to_delta_expr(const GammaExpr& expr);
