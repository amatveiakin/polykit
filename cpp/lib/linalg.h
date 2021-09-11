#pragma once

#include <string>
#include <vector>

#include "Eigen/Core"
#include "Eigen/SparseCore"


using Triplet = Eigen::Triplet<int, int>;
using TripletVec = std::vector<Triplet>;

// In reality matrix elements are integers, but we store tham as a matrix of double,
// because Eigen cannot compute integer matrix rank.
// TODO: Consider using Eigen::SparseMatrix instead: always or when the size is large.
using Matrix = Eigen::MatrixXd;

int matrix_rank(const Matrix& matrix);
int matrix_rank(const TripletVec& triplets);

void save_triplets(const std::string& filename, const std::vector<Triplet>& triplets);
std::vector<Triplet> load_triplets(const std::string& filename);
