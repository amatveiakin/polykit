#pragma once

#include "coalgebra.h"


ThetaCoExpr epsilon_coexpr_to_theta_coexpr(
    const EpsilonCoExpr& expr,
    const std::vector<std::vector<CrossRatio>>& ratios);
