#pragma once

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"

#include "check.h"


// Sparse integer matrix.
class Matrix {
public:
  struct Triplet {
    int row = 0;
    int col = 0;
    int value = 0;
  };

  explicit Matrix(int rows, int cols) : rows_(rows), cols_(cols) {}
  explicit Matrix(std::pair<int, int> shape) : rows_(shape.first), cols_(shape.second) {}
  explicit Matrix(std::pair<int, int> shape, absl::flat_hash_map<std::pair<int, int>, int> triplets);

  // Use carefully: it detects dimensions automatically, so it will lose trailing zero rows or cols.
  static Matrix from_triplets(absl::flat_hash_map<std::pair<int, int>, int> triplets);

  int rows() const { return rows_; }
  int cols() const { return cols_; }
  std::pair<int, int> shape() const { return {rows_, cols_}; }

  int num_triplets() const { return triplets_.size(); }
  absl::flat_hash_map<std::pair<int, int>, int> as_map() const { return triplets_; }
  // Optimization potential: return a custom iterator instead.
  std::vector<Triplet> as_triplets() const;

  Matrix transposed() const;

  int& operator()(int row, int col);
  int& operator()(std::pair<int, int> rowcol) { return operator()(rowcol.first, rowcol.second); };

  bool operator==(const Matrix& other) const { return triplets_ == other.triplets_ && shape() == other.shape(); }
  bool operator!=(const Matrix& other) const { return !(*this == other); }

private:
  int rows_ = 0;
  int cols_ = 0;
  absl::flat_hash_map<std::pair<int, int>, int> triplets_;
};

inline int& Matrix::operator()(int row, int col) {
  CHECK(0 <= row && row < rows_) << row << " not in [0," << rows_ << ")";
  CHECK(0 <= col && col < cols_) << col << " not in [0," << cols_ << ")";
  return triplets_[{row, col}];
}


class MatrixView {
public:
  enum ViewType {
    RowMajor,
    ColumnMajor,
  };

  struct Triplet {
    int a = 0;
    int b = 0;
    int value = 0;
  };

  MatrixView(const Matrix* matrix, ViewType view_type);

  const Matrix* matrix() const { return matrix_; }
  ViewType view_type() const { return view_type_; }

  int a_size() const { return view_type_ == RowMajor ? matrix_->rows() : matrix_->cols(); }
  int b_size() const { return view_type_ == RowMajor ? matrix_->cols() : matrix_->rows(); }
  // Optimization potential: return a custom iterator instead.
  std::vector<Triplet> as_triplets() const;

private:
  const Matrix* matrix_ = nullptr;
  ViewType view_type_ = {};
};


// Sortes rows in lexicographic order and removes duplicates.
Matrix sort_unique_rows(const Matrix& matrix);

// Sortes columns in lexicographic order and removes duplicates.
Matrix sort_unique_cols(const Matrix& matrix);

// Prints matrix elements (with omissions if the matrix is large).
std::string to_string(const Matrix& matrix);

// Prints matrix metadata.
std::string dump_to_string_impl(const Matrix& matrix);

// Saves matrix in the following format:
//   num_rows num_cols
//   num_nonzero_elements
//   row_1 col_1 value_1
//   row_2 col_2 value_2
//   ...
void save_triplets(const std::string& filename, const Matrix& matrix);

// Loads matrix stored by `save_triplets`.
Matrix load_triplets(const std::string& filename);

// Reads vector written as `[a11, a12, ...; a21, a22, ...; ...]`.
std::vector<std::vector<int>> load_bracketed_vectors(const std::string& filename);
