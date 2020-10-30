#pragma once

#include "polylog_param.h"
#include "theta.h"


enum class LiFirstPoint {
  odd,
  even,
};

ThetaExpr LiQuad(const std::vector<int>& points, LiFirstPoint first_point = LiFirstPoint::odd);
