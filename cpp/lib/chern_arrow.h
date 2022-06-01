// Auxiliary operations for finding Chern classes.

#pragma once

#include "gamma.h"


GammaExpr chern_arrow_left(const GammaExpr& expr, int num_dst_points);
GammaExpr chern_arrow_up(const GammaExpr& expr, int num_dst_points);

GammaNCoExpr chern_arrow_left(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr chern_arrow_up(const GammaNCoExpr& expr, int num_dst_points);
