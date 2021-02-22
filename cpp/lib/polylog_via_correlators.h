// Correlator expressions with the same symbol as QLi functions.

#pragma once

#include "corr_expression.h"
#include "delta.h"


CorrExpr CorrQLi(int weight, const std::vector<int>& points);
CorrExpr CorrQLiNeg(int weight, const std::vector<int>& points);
CorrExpr CorrQLiNegAlt(int weight, const std::vector<int>& points); // same symbol; different formal symbol
CorrExpr CorrQLiSymm(int weight, const std::vector<int>& points);
