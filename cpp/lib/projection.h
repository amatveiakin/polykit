#pragma once

#include "delta.h"
#include "word.h"


IntWordExpr project_on(int axis, const DeltaExpr& expr);

inline IntWordExpr project_on_x1 (const DeltaExpr& expr) { return project_on(1, expr); }
inline IntWordExpr project_on_x2 (const DeltaExpr& expr) { return project_on(2, expr); }
inline IntWordExpr project_on_x3 (const DeltaExpr& expr) { return project_on(3, expr); }
inline IntWordExpr project_on_x4 (const DeltaExpr& expr) { return project_on(4, expr); }
inline IntWordExpr project_on_x5 (const DeltaExpr& expr) { return project_on(5, expr); }
inline IntWordExpr project_on_x6 (const DeltaExpr& expr) { return project_on(6, expr); }
inline IntWordExpr project_on_x7 (const DeltaExpr& expr) { return project_on(7, expr); }
inline IntWordExpr project_on_x8 (const DeltaExpr& expr) { return project_on(8, expr); }
inline IntWordExpr project_on_x9 (const DeltaExpr& expr) { return project_on(9, expr); }
inline IntWordExpr project_on_x10(const DeltaExpr& expr) { return project_on(10, expr); }
