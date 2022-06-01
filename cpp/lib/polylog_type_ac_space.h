// Polylog spaces on type A and type C.

#pragma once

#include "delta.h"
#include "x.h"


using TypeAC_Space = std::vector<DeltaExpr>;
using TypeAC_NCoSpace = std::vector<DeltaNCoExpr>;

TypeAC_Space CB_naive_via_QLi_fours(int weight, const XArgs& xargs);
TypeAC_Space CB(int weight, const XArgs& xargs);
inline TypeAC_Space CB1(const XArgs& xargs) { return CB(1, xargs); }
inline TypeAC_Space CB2(const XArgs& xargs) { return CB(2, xargs); }
inline TypeAC_Space CB3(const XArgs& xargs) { return CB(3, xargs); }
inline TypeAC_Space CB4(const XArgs& xargs) { return CB(4, xargs); }
inline TypeAC_Space CB5(const XArgs& xargs) { return CB(5, xargs); }
inline TypeAC_Space CB6(const XArgs& xargs) { return CB(6, xargs); }
inline TypeAC_Space CB7(const XArgs& xargs) { return CB(7, xargs); }
inline TypeAC_Space CB8(const XArgs& xargs) { return CB(8, xargs); }

TypeAC_Space CL(int weight, const XArgs& xargs);
inline TypeAC_Space CL1(const XArgs& xargs) { return CL(1, xargs); }
inline TypeAC_Space CL2(const XArgs& xargs) { return CL(2, xargs); }
inline TypeAC_Space CL3(const XArgs& xargs) { return CL(3, xargs); }
inline TypeAC_Space CL4(const XArgs& xargs) { return CL(4, xargs); }
inline TypeAC_Space CL5(const XArgs& xargs) { return CL(5, xargs); }
inline TypeAC_Space CL6(const XArgs& xargs) { return CL(6, xargs); }
inline TypeAC_Space CL7(const XArgs& xargs) { return CL(7, xargs); }
inline TypeAC_Space CL8(const XArgs& xargs) { return CL(8, xargs); }
TypeAC_Space old_CL4_via_A2(const XArgs& xargs);

// Cluster polylogarithms of type C.
TypeAC_Space typeC_CL(int weight, const XArgs& xargs);
TypeAC_Space typeC_CB(int weight, const XArgs& xargs);

TypeAC_Space H(int weight, const XArgs& xargs);

TypeAC_Space Fx(const XArgs& xargs);

TypeAC_Space LInf(int weight, const XArgs& xargs);
TypeAC_Space L(int weight, const XArgs& xargs);
inline TypeAC_Space L1(const XArgs& xargs) { return L(1, xargs); }
inline TypeAC_Space L2(const XArgs& xargs) { return L(2, xargs); }
inline TypeAC_Space L3(const XArgs& xargs) { return L(3, xargs); }
inline TypeAC_Space L4(const XArgs& xargs) { return L(4, xargs); }
inline TypeAC_Space L5(const XArgs& xargs) { return L(5, xargs); }
inline TypeAC_Space L6(const XArgs& xargs) { return L(6, xargs); }
inline TypeAC_Space L7(const XArgs& xargs) { return L(7, xargs); }
inline TypeAC_Space L8(const XArgs& xargs) { return L(8, xargs); }

TypeAC_Space XCoords(int weight, const XArgs& args);
TypeAC_Space ACoords(int weight, const XArgs& args);
TypeAC_Space ACoordsHopf(int weight, const XArgs& args);

TypeAC_Space CL1_inv(const XArgs& xargs);
TypeAC_Space CL2_inv(const XArgs& xargs);

// The "simple_" functions apply normalize_remove_consecutive (hence not allowing arbitrary input points).
TypeAC_NCoSpace simple_co_L(int weight, int num_coparts, int num_points);

TypeAC_NCoSpace co_CL(int weight, int num_coparts, const XArgs& xargs);
