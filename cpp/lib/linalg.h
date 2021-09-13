#pragma once

#include <string>
#include <vector>

#include "check.h"


class Matrix {
public:
  struct Triplet {
    int row = 0;
    int col = 0;
    int value = 0;
  };

  int rows() const { return rows_; }
  int cols() const { return cols_; }
  const std::vector<Triplet>& as_triplets() const { return triplets_; }

  int& insert(int row, int col);

private:
  int rows_ = 0;
  int cols_ = 0;
  std::vector<Triplet> triplets_;
};

inline int& Matrix::insert(int row, int col) {
  CHECK_LE(0, row);
  CHECK_LE(0, col);
  rows_ = std::max(rows_, row + 1);
  cols_ = std::max(cols_, col + 1);
  triplets_.push_back({row, col, 0});
  return triplets_.back().value;
}

int matrix_rank(const Matrix& matrix);

void save_triplets(const std::string& filename, const Matrix& matrix);
Matrix load_triplets(const std::string& filename);
