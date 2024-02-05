#include "expr_matrix_builder.h"


using internal::SparseElement;

namespace internal {

Matrix make_matrix(
  const std::vector<std::vector<SparseElement>>& sparse_cols, int num_rows
) {
  Matrix matrix(num_rows, sparse_cols.size());
  int col = 0;
  for (const auto& rowvalues : sparse_cols) {
    for (const auto& [row, value] : rowvalues) {
      matrix(row, col) = value;
    }
    ++col;
  }
  // We use these matrices for two purposes finding space ranks and finding null spaces.
  // Therefore, it ok to deduplicate the rowumns, because it doesn't affect either operation.
  // It is not ok to deduplicate cols here, because that would break correspondence between
  // cols and the original expressions.
  // Note that after the rowumns are sorted, the matrix for each space is deterministic,
  // assuming the expressions are fed to `ExprTupleMatrixBuilder` in a fixed order.
  return sort_unique_rows(matrix);
}

}  // namespace internal
