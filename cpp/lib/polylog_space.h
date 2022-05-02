// Optimization potential: convert to Lyndon basis earlier to avoid doing this multiple times.

#pragma once

#include "delta.h"
#include "gamma.h"
#include "itertools.h"
#include "kappa.h"
#include "vector_space.h"
#include "x.h"


using PolylogSpace = std::vector<DeltaExpr>;
using PolylogNCoSpace = std::vector<DeltaNCoExpr>;
using GrPolylogSpace = std::vector<GammaExpr>;
using GrPolylogNCoSpace = std::vector<GammaNCoExpr>;
using GrPolylogACoSpace = std::vector<GammaACoExpr>;
using TypeDPolylogSpace = std::vector<KappaExpr>;
using TypeDPolylogNCoSpace = std::vector<KappaNCoExpr>;
using TypeDPolylogACoSpace = std::vector<KappaACoExpr>;

PolylogSpace CB_naive_via_QLi_fours(int weight, const XArgs& xargs);
PolylogSpace CB(int weight, const XArgs& xargs);
inline PolylogSpace CB1(const XArgs& xargs) { return CB(1, xargs); }
inline PolylogSpace CB2(const XArgs& xargs) { return CB(2, xargs); }
inline PolylogSpace CB3(const XArgs& xargs) { return CB(3, xargs); }
inline PolylogSpace CB4(const XArgs& xargs) { return CB(4, xargs); }
inline PolylogSpace CB5(const XArgs& xargs) { return CB(5, xargs); }
inline PolylogSpace CB6(const XArgs& xargs) { return CB(6, xargs); }
inline PolylogSpace CB7(const XArgs& xargs) { return CB(7, xargs); }
inline PolylogSpace CB8(const XArgs& xargs) { return CB(8, xargs); }

PolylogSpace CL(int weight, const XArgs& xargs);
inline PolylogSpace CL1(const XArgs& xargs) { return CL(1, xargs); }
inline PolylogSpace CL2(const XArgs& xargs) { return CL(2, xargs); }
inline PolylogSpace CL3(const XArgs& xargs) { return CL(3, xargs); }
inline PolylogSpace CL4(const XArgs& xargs) { return CL(4, xargs); }
inline PolylogSpace CL5(const XArgs& xargs) { return CL(5, xargs); }
inline PolylogSpace CL6(const XArgs& xargs) { return CL(6, xargs); }
inline PolylogSpace CL7(const XArgs& xargs) { return CL(7, xargs); }
inline PolylogSpace CL8(const XArgs& xargs) { return CL(8, xargs); }
PolylogSpace old_CL4_via_A2(const XArgs& xargs);

// Cluster polylogarithms of type C.
PolylogSpace typeC_CL(int weight, const XArgs& xargs);
PolylogSpace typeC_CB(int weight, const XArgs& xargs);

PolylogSpace H(int weight, const XArgs& xargs);

PolylogSpace Fx(const XArgs& xargs);

PolylogSpace LInf(int weight, const XArgs& xargs);
PolylogSpace L(int weight, const XArgs& xargs);
inline PolylogSpace L1(const XArgs& xargs) { return L(1, xargs); }
inline PolylogSpace L2(const XArgs& xargs) { return L(2, xargs); }
inline PolylogSpace L3(const XArgs& xargs) { return L(3, xargs); }
inline PolylogSpace L4(const XArgs& xargs) { return L(4, xargs); }
inline PolylogSpace L5(const XArgs& xargs) { return L(5, xargs); }
inline PolylogSpace L6(const XArgs& xargs) { return L(6, xargs); }
inline PolylogSpace L7(const XArgs& xargs) { return L(7, xargs); }
inline PolylogSpace L8(const XArgs& xargs) { return L(8, xargs); }

PolylogSpace XCoords(int weight, const XArgs& args);
PolylogSpace ACoords(int weight, const XArgs& args);
PolylogSpace ACoordsHopf(int weight, const XArgs& args);

GrPolylogSpace gr_free_lie_coalgebra(int weight, int dimension, const std::vector<int>& args);

GrPolylogSpace GrFx(int dimension, const std::vector<int>& args);

GrPolylogSpace GrL_core(
  int weight, int dimension, const std::vector<int>& args,
  bool include_one_minus_cross_ratio, int num_fixed_points);
GrPolylogSpace GrL1(int dimension, const std::vector<int>& args);
GrPolylogSpace GrL2(int dimension, const std::vector<int>& args);
GrPolylogSpace GrL3(int dimension, const std::vector<int>& args);
GrPolylogSpace GrL4_Dim3(const std::vector<int>& args);
GrPolylogSpace GrL(int weight, int dimension, const std::vector<int>& args);

// Suffix "test_space" stands for the fact that these are candidate spaces likely to be missing some functions.
GrPolylogSpace CGrL_Dim3_naive_test_space(int weight, const std::vector<int>& points);
GrPolylogSpace CGrL3_Dim3_test_space(const std::vector<int>& points);
GrPolylogSpace CGrL_Dim4_naive_test_space(int weight, const std::vector<int>& points);
GrPolylogSpace CGrL_test_space(int weight, int dimension, const std::vector<int>& points);

TypeDPolylogSpace typeD_free_lie_coalgebra(int weight);

// The "simple_" functions apply normalize_remove_consecutive (hence not allowing arbitrary input points).
PolylogNCoSpace simple_co_L(int weight, int num_coparts, int num_points);
GrPolylogNCoSpace simple_co_GrL(int weight, int num_coparts, int dimension, int num_points);
GrPolylogNCoSpace simple_co_CGrL_test_space(int weight, int dimension, int num_points);

PolylogNCoSpace co_CL(int weight, int num_coparts, const XArgs& xargs);
