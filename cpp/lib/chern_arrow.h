// Auxiliary operations for finding Chern classes.

#pragma once

#include "gamma.h"


GammaExpr chern_arrow_left(const GammaExpr& expr, int num_dst_points);  // a.k.a. "a"
GammaExpr chern_arrow_up(const GammaExpr& expr, int num_dst_points);  // a.k.a. "b"
GammaExpr a_minus(const GammaExpr& expr, int num_dst_points);
GammaExpr a_plus(const GammaExpr& expr, int num_dst_points);
GammaExpr b_minus(const GammaExpr& expr, int num_dst_points);
GammaExpr b_plus(const GammaExpr& expr, int num_dst_points);

GammaNCoExpr chern_arrow_left(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr chern_arrow_up(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr a_minus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr a_plus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_minus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_plus(const GammaNCoExpr& expr, int num_dst_points);
