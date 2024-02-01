#include "expr_matrix_builder.h"


using internal::SparseElement;

namespace internal {

Matrix make_matrix(
  const std::vector<std::vector<SparseElement>>& sparse_rows, int num_cols
) {
  Matrix matrix(sparse_rows.size(), num_cols);
  int row = 0;
  for (const auto& colvalues : sparse_rows) {
    for (const auto& [col, value] : colvalues) {
      matrix(row, col) = value;
    }
    ++row;
  }
  // We use these matrices for two purposes finding space ranks and finding null spaces.
  // Therefore, it ok to deduplicate the columns, because it doesn't affect either operation.
  // It is not ok to deduplicate rows here, because that would break correspondence between
  // rows and the original expressions.
  // Note that after the columns are sorted, the matrix for each space is deterministic,
  // assuming the expressions are fed to `ExprTupleMatrixBuilder` in a fixed order.
  return sort_unique_cols(matrix);
}

}  // namespace internal
