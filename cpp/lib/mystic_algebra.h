#pragma once

#include "epsilon.h"
#include "theta.h"


EpsilonExpr mystic_product(
    const EpsilonExpr& lhs,
    const EpsilonExpr& rhs);

EpsilonExpr mystic_product(
    const absl::Span<const EpsilonExpr>& expressions);

EpsilonICoExpr mystic_product(
    const EpsilonICoExpr& lhs,
    const EpsilonICoExpr& rhs);


ThetaExpr quasi_shuffle_product_expr(
    const absl::Span<const ThetaExpr>& expressions);
