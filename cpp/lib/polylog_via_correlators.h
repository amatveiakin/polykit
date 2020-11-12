#pragma once

#include "delta.h"


DeltaExpr LidoViaCorr(int weight, int num_vars);
DeltaExpr LidoNegViaCorr(int weight, int num_vars);
DeltaExpr LidoSymmViaCorr(int weight, int num_vars);
