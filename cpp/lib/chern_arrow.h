// Note. These functions assume 1-based variable indexing.

#pragma once

#include "gamma.h"


GammaExpr chern_arrow_left(const GammaExpr& expr, int num_dst_points);  // same as `a_full`
GammaExpr chern_arrow_up(const GammaExpr& expr, int num_dst_points);  // same as `b_full`
GammaExpr a_full(const GammaExpr& expr, int num_dst_points);
GammaExpr a_minus(const GammaExpr& expr, int num_dst_points);
GammaExpr a_plus(const GammaExpr& expr, int num_dst_points);
GammaExpr b_full(const GammaExpr& expr, int num_dst_points);
GammaExpr b_minus(const GammaExpr& expr, int num_dst_points);
GammaExpr b_plus(const GammaExpr& expr, int num_dst_points);
GammaExpr a_minus_minus(const GammaExpr& expr, int num_dst_points);
GammaExpr a_plus_plus(const GammaExpr& expr, int num_dst_points);
GammaExpr b_minus_minus(const GammaExpr& expr, int num_dst_points);
GammaExpr b_plus_plus(const GammaExpr& expr, int num_dst_points);

GammaNCoExpr chern_arrow_left(const GammaNCoExpr& expr, int num_dst_points);  // same as `a_full`
GammaNCoExpr chern_arrow_up(const GammaNCoExpr& expr, int num_dst_points);  // same as `b_full`
GammaNCoExpr a_full(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr a_minus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr a_plus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_full(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_minus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_plus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr a_minus_minus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr a_plus_plus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_minus_minus(const GammaNCoExpr& expr, int num_dst_points);
GammaNCoExpr b_plus_plus(const GammaNCoExpr& expr, int num_dst_points);

using ABFunction = std::function<GammaExpr(const GammaExpr&, int)>;
using NCoABFunction = std::function<GammaNCoExpr(const GammaNCoExpr&, int)>;

enum class ABDoublePlusMinus {
  Include,
  Exclude,
};

std::vector<ABFunction> list_ab_function(ABDoublePlusMinus double_plus_minus);
std::vector<NCoABFunction> list_nco_ab_function(ABDoublePlusMinus double_plus_minus);
