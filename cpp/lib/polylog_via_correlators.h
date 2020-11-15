#pragma once

#include "corr_expression.h"
#include "delta.h"


DeltaExpr LidoViaCorr(int weight, int num_vars);
DeltaExpr LidoNegViaCorr(int weight, int num_vars);
DeltaExpr LidoNegAltViaCorr(int weight, int num_vars);  // NOT equivalent to LidoNegViaCorr
DeltaExpr LidoSymmViaCorr(int weight, int num_vars);

CorrExpr LidoViaCorrFSymb(int weight, int num_vars);
CorrExpr LidoNegViaCorrFSymb(int weight, int num_vars);
CorrExpr PosCorrFSymb(int weight, const std::vector<int>& points);
CorrExpr NegCorrFSymb(int weight, const std::vector<int>& points);
