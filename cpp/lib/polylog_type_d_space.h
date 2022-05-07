#pragma once

#include "kappa.h"


using TypeD_Space = std::vector<KappaExpr>;
using TypeD_NCoSpace = std::vector<KappaNCoExpr>;
using TypeD_ACoSpace = std::vector<KappaACoExpr>;

TypeD_Space typeD_free_lie_coalgebra(int weight);
