#pragma once

#include "kappa.h"


using TypeD_Space = std::vector<KappaExpr>;
using TypeD_NCoSpace = std::vector<KappaNCoExpr>;
using TypeD_ACoSpace = std::vector<KappaACoExpr>;

TypeD_Space typeD_free_lie_coalgebra(int weight);

TypeD_Space typeD_Fx();

std::vector<std::array<KappaExpr, 3>> typeD_B2_generators();
TypeD_Space typeD_B2();  // lots of duplicates, `space_basis` is recommended
