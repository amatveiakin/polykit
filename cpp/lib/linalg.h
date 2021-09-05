#pragma once

#include "Eigen/Core"


// TODO: Consider using Eigen::SparseMatrix instead: always or when the size is large.
using Matrix = Eigen::MatrixXd;

int matrix_rank(const Matrix& matrix);
