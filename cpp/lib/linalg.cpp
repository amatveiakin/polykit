// TODO: Stop this madness! Link linbox directly and remove dependency on Sage, Eigen and boost.

#include "linalg.h"

#include <linbox/linbox-config.h>
#include <givaro/givrational.h>
#include <linbox/ring/modular.h>
#include <linbox/field/gf2.h>
#include <linbox/matrix/sparse-matrix.h>
#include <linbox/blackbox/zero-one.h>
#include <linbox/solutions/rank.h>
#include <linbox/util/matrix-stream.h>

#include "check.h"
#include "file_util.h"
#include "string.h"


int matrix_rank(const Matrix& matrix) {
  // LinBox::commentator().setMaxDetailLevel(-1);
  // LinBox::commentator().setMaxDepth(-1);
  // LinBox::commentator().setReportStream(std::cerr);
  Givaro::QField<Givaro::Rational> QQ;
  LinBox::SparseMatrix<Givaro::QField<Givaro::Rational>, LinBox::SparseMatrixFormat::SparseSeq> linbox_matrix(
    QQ, matrix.rows(), matrix.cols()
  );
  for (const auto& t : matrix.as_triplets()) {
    linbox_matrix.setEntry(t.row, t.col, t.value);
  }

  size_t rank;
  LinBox::rank(rank, linbox_matrix);
  return rank;
}


void save_triplets(const std::string& filename, const Matrix& matrix) {
  std::ofstream fs(filename);
  CHECK(fs.good());
  for (const auto& t : matrix.as_triplets()) {
    fs << t.row << " " << t.col << " " << t.value << "\n";
  }
  CHECK(fs.good());
}

Matrix load_triplets(const std::string& filename) {
  std::ifstream fs(filename);
  Matrix matrix;
  std::string line;
  while (std::getline(fs, line)) {
    std::istringstream ss(line);
    int row, col, value;
    CHECK(!!(ss >> row >> col >> value));
    matrix.insert(row, col) = value;
  }
  return matrix;
}
