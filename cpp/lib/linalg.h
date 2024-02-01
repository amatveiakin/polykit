#pragma once

#include <vector>

#include "matrix.h"


// Returns some vector x such that matrix*x = rhs.
// If there is no solution, returns zero vector or aborts.
// TODO: Consistent error handling.
// TODO: Return a rational instead.
std::vector<double> linear_solve(const Matrix& matrix, const std::vector<int>& rhs);

// Returns some vector from matrix nullspace.
// If there is no solution, returns zero vector or aborts.
// TODO: Consistent error handling.
std::vector<int> find_kernel_vector(const Matrix& matrix);

// Returns matrix rank.
int matrix_rank(const Matrix& matrix);
