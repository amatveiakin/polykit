#pragma once

#include "polylog_li_param.h"
#include "theta.h"


enum class LiFirstPoint {
  odd,
  even,
};

ThetaExpr LiQuad(
    int foreweight,
    const std::vector<int>& points,
    LiFirstPoint first_point = LiFirstPoint::odd);
