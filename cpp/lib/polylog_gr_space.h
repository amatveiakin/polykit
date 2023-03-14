#pragma once

#include "gamma.h"


using Gr_Space = std::vector<GammaExpr>;
using Gr_NCoSpace = std::vector<GammaNCoExpr>;
using Gr_ACoSpace = std::vector<GammaACoExpr>;


Gr_Space gr_free_lie_coalgebra(int weight, int dimension, const std::vector<int>& args);

Gr_Space GrFx(int dimension, const std::vector<int>& args);
Gr_Space GrFxPrime(int dimension, const std::vector<int>& args);  // fixed last point

Gr_Space GrL_core(
  int weight, int dimension, const std::vector<int>& args,
  bool include_one_minus_cross_ratio, int num_fixed_points);
Gr_Space GrL1(int dimension, const std::vector<int>& args);
Gr_Space GrL2(int dimension, const std::vector<int>& args);
Gr_Space GrL3(int dimension, const std::vector<int>& args);
Gr_Space GrL4_Dim3(const std::vector<int>& args);
Gr_Space GrL(int weight, int dimension, const std::vector<int>& args);

Gr_NCoSpace GrLArnold2(int dimension, const std::vector<int>& args);

// Suffix "test_space" stands for the fact that these are candidate spaces likely to be missing some functions.
Gr_Space CGrL_Dim3_naive_test_space(int weight, const std::vector<int>& points);
Gr_Space CGrL3_Dim3_test_space(const std::vector<int>& points);
Gr_Space CGrL_Dim4_naive_test_space(int weight, const std::vector<int>& points);
Gr_Space CGrL_test_space(int weight, int dimension, const std::vector<int>& points);

// TODO: Proper infinity (note: don't use `std::numeric_limits<int>::max()` because of `depth + 1`).
Gr_Space ChernGrL(int weight, int dimension, const std::vector<int>& points, int depth = 1000000);
Gr_Space OldChernGrL(int weight, int dimension, const std::vector<int>& points, int depth = 1000000);

Gr_NCoSpace simple_co_GrL(int weight, int num_coparts, int dimension, int num_points);
Gr_NCoSpace simple_co_CGrL_test_space(int weight, int dimension, int num_points);

Gr_NCoSpace wedge_ChernGrL(int weight, int dimension, const std::vector<int>& points);
