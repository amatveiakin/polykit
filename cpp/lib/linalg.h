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
  int& insert(std::pair<int, int> rowcol) { return insert(rowcol.first, rowcol.second); };

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


// Prints matrix elements (with omissions if the matrix is large).
std::string to_string(const Matrix& matrix);

// Prints matrix metadata.
std::string dump_to_string_impl(const Matrix& matrix);

// Splits matrix into blocks if it's block diagonal.
std::vector<Matrix> get_matrix_diagonal_blocks(const Matrix& matrix);

// Removes empty rows and columns.
Matrix compress_matrix(const Matrix& matrix);

// Returns x such that Ax = b, random solution if sigular and consistent, x = 0 if inconsistent.
// TODO: Return a rational instead.
std::vector<double> linear_solve(const Matrix& matrix, const std::vector<int>& rhs);

// Computes matrix rank without preconditioning.
int matrix_rank_raw_linbox(const Matrix& matrix);

// Computes matrix rank.
//
// May use preconditioning, e.g.: split the matrix into diagonal blocks, reorder rows and
// cols to make the matrix closer to diagonal. This is not guaranteed to be faster than
// `matrix_rank_raw_linbox`. In particular, it could to be slower when there is only block.
// However it should be faster on average, especially for large matrices.
//
int matrix_rank(const Matrix& matrix);

void save_triplets(const std::string& filename, const Matrix& matrix);
Matrix load_triplets(const std::string& filename);
