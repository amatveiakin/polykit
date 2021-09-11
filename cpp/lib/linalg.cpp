#include "linalg.h"

#include <fstream>
#include <future>
#include <sstream>

#include "boost/process.hpp"

// TODO: Choose decomposition and clean up
#include "Eigen/QR"
// #include "Eigen/SVD"

#include "check.h"
#include "string.h"

namespace bp = boost::process;


int matrix_rank(const Matrix& matrix) {
  return matrix.colPivHouseholderQr().rank();
}

// TODO: Link linbox directly instead, or at least don't hard-code path to the sage file !!!
int matrix_rank(const TripletVec& triplets) {
  std::ostringstream input;
  for (const auto& t : triplets) {
    input << t.row() << " " << t.col() << " " << t.value() << "\n";
  }
  std::future<std::string> output;
  bp::system(
    "sage /mnt/c/Danya/sage/space_rank.sage",
    bp::std_in < boost::asio::buffer(input.str()),
    bp::std_out > output
  );
  return std::stoi(output.get());
}


void save_triplets(const std::string& filename, const std::vector<Triplet>& triplets) {
  std::ofstream fs(filename);
  CHECK(fs.good());
  for (const auto& t : triplets) {
    fs << t.row() << " " << t.col() << " " << t.value() << "\n";
  }
  CHECK(fs.good());
}

std::vector<Triplet> load_triplets(const std::string& filename) {
  std::ifstream fs(filename);
  std::vector<Triplet> triplets;
  std::string line;
  while (std::getline(fs, line)) {
    std::istringstream ss(line);
    int row, col, value;
    CHECK(!!(ss >> row >> col >> value));
    triplets.push_back({row, col, value});
  }
  return triplets;
}
