#pragma once

#include "corr_expression.h"
#include "delta.h"


DeltaExpr QLiViaCorr(int weight, int num_vars);
DeltaExpr QLiNegViaCorr(int weight, int num_vars);
DeltaExpr QLiNegAltViaCorr(int weight, int num_vars);  // NOT equivalent to QLiNegViaCorr
DeltaExpr QLiSymmViaCorr(int weight, int num_vars);

CorrExpr QLiViaCorrFSymb(int weight, int num_vars);
CorrExpr QLiNegViaCorrFSymb(int weight, int num_vars);
CorrExpr PosCorrFSymb(int weight, const std::vector<int>& points);
CorrExpr NegCorrFSymb(int weight, const std::vector<int>& points);
