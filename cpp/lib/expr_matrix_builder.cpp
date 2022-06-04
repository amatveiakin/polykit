#include "expr_matrix_builder.h"


using internal::SparseElement;

static absl::flat_hash_set<std::vector<SparseElement>> unique_sparse_columns(
  const absl::flat_hash_set<std::vector<SparseElement>>& sparse_rows, int num_cols
) {
  std::vector<std::vector<SparseElement>> sparse_columns(num_cols);
  int i_row = 0;
  for (const auto& row : sparse_rows) {
    for (const auto& [i_col, coeff] : row) {
      sparse_columns[i_col].push_back({i_row, coeff});
    }
    ++i_row;
  }
  // Note: Elements are sorted within each column since we iterate rows sequentially.
  //   Hence decuplication works.
  return to_set(sparse_columns);
}


namespace internal {

Matrix make_matrix(
  const absl::flat_hash_set<std::vector<SparseElement>>& sparse_rows, int num_cols
) {
  const auto sparse_columns = unique_sparse_columns(sparse_rows, num_cols);
  Matrix matrix;
  int i_col = 0;
  for (const auto& column : sparse_columns) {
    for (const auto& [i_row, value] : column) {
      matrix.insert(i_row, i_col) = value;
    }
    ++i_col;
  }
  return matrix;
}

}  // namespace internal
