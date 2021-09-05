#include "linalg.h"

// TODO: Choose decomposition and clean up
#include "Eigen/QR"
// #include "Eigen/SVD"


int matrix_rank(const Matrix& matrix) {
  return matrix.colPivHouseholderQr().rank();
}
