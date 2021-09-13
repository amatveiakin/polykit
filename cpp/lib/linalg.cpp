// TODO: Stop this madness! Link linbox directly and remove dependency on Sage, Eigen and boost.

#include "linalg.h"

#include <filesystem>
#include <fstream>
#include <future>
#include <sstream>

#include "boost/process.hpp"

#include "Eigen/QR"
// #include "Eigen/SVD"

#include "check.h"
#include "file_util.h"
#include "string.h"

namespace bp = boost::process;


static const char integer_matrix_rank_sage[] = R"(
# TODO: Why doesn't this work?
# Parallelism().set(nproc=32)

num_rows = 0
num_cols = 0
triplets = []
for line in sys.stdin:
    row, col, value = [int(x) for x in line.split()]
    num_rows = max(num_rows, row + 1)
    num_cols = max(num_cols, col + 1)
    triplets.append((row, col, value))
# mat = matrix(ZZ, num_rows, num_cols)
mat = matrix(ZZ, num_rows, num_cols, sparse=True)
for row, col, value in triplets:
    mat[row, col] = value

dim = mat.rank(algorithm='linbox')
print(dim)
)";


Eigen::MatrixXd make_eigen_matrix(const Matrix& matrix) {
  Eigen::MatrixXd eigen_matrix = Eigen::MatrixXd::Zero(matrix.rows(), matrix.cols());
  for (const auto& t : matrix.as_triplets()) {
    eigen_matrix(t.row, t.col) = t.value;
  }
  return eigen_matrix;
}


int matrix_rank_via_eigen(const Matrix& matrix) {
  return make_eigen_matrix(matrix).colPivHouseholderQr().rank();
}

int matrix_rank_via_sage(const Matrix& matrix) {
  std::ostringstream input;
  for (const auto& t : matrix.as_triplets()) {
    input << t.row << " " << t.col << " " << t.value << "\n";
  }
  std::future<std::string> output;
  const std::string rank_util_filename = std::filesystem::temp_directory_path() / "integer_matrix_rank.sage";
  set_file_content(rank_util_filename, integer_matrix_rank_sage);
  int exit_code = bp::system(
    absl::StrCat("sage ", rank_util_filename),
    bp::std_in < boost::asio::buffer(input.str()),
    bp::std_out > output
  );
  CHECK_EQ(exit_code, 0);
  return std::stoi(output.get());
}

int matrix_rank(const Matrix& matrix) {
  if (matrix.rows() == 0 || matrix.cols() == 0) {
    return 0;
  }
  // Sage is faster and has guaranteed correctness, but has large start-up time.
  return (matrix.rows() * matrix.cols() > 1'000'000)
    ? matrix_rank_via_sage(matrix)
    : matrix_rank_via_eigen(matrix);
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
