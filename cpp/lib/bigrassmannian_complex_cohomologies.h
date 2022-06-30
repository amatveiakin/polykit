#pragma once

#include "polylog_gr_space.h"


int bigrassmannian_complex_cohomology(
  int dimension, int num_points, const std::function<Gr_Space(int, const std::vector<int>&)>& space
);
int bigrassmannian_complex_cohomology(
  int dimension, int num_points, const std::function<Gr_NCoSpace(int, const std::vector<int>&)>& space
);
