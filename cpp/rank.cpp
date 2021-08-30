#include <fstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "lib/check.h"

#include "Eigen/SparseQR"


using Scalar = double;
using Matrix = Eigen::SparseMatrix<Scalar>;
using Triplet = Eigen::Triplet<Scalar>;

int main(int argc, char* argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  CHECK_EQ(argc, 2);
  std::ifstream fstream(argv[1]);
  CHECK(fstream.good());
  std::string line;
  std::vector<Triplet> triplets;
  int num_rows = 0;
  int num_cols = 0;
  while (std::getline(fstream, line)) {
    std::istringstream iss(line);
    int row, col, value;
    if (!(iss >> row >> col >> value)) {
      FATAL(absl::StrCat("Invalid line:\n", line));
    }
    triplets.push_back({row, col, value});
    num_rows = std::max(num_rows, row + 1);
    num_cols = std::max(num_cols, col + 1);
  }
  Matrix matrix(num_rows, num_cols);
  matrix.setFromTriplets(triplets.begin(), triplets.end(), [](Scalar, Scalar) -> Scalar {
    FATAL("Duplicate entries found");
  });
  std::cout << "matrix read\n";

  Eigen::SparseQR<Matrix, Eigen::COLAMDOrdering<int>> decomp(matrix);
  const int rank = decomp.rank();
  std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => " << rank << "\n";
}
