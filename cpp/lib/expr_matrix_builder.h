#include "absl/container/flat_hash_set.h"

#include "Eigen/Core"

#include "enumerator.h"
#include "util.h"


template<typename ExprT>
class ExprMatrixBuilder {
public:
  void add_expr(const ExprT& expr) {
    std::vector<SparseElement> row;
    for (const auto& [term, coeff] : expr) {
      row.push_back({monoms_.index(term), coeff});
    }
    sparse_rows_.insert(row);
  }

  template<typename T>
  auto make_matrix() const {
    using MatrixT = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
    const auto sparse_columns = unique_sparse_columns();
    const int num_rows = sparse_rows_.size();
    const int num_cols = sparse_columns.size();
    MatrixT mat = MatrixT::Zero(num_rows, num_cols);
    int i_col = 0;
    for (const auto& column : sparse_columns) {
      for (const auto& [i_row, coeff] : column) {
        mat(i_row, i_col) = coeff;
      }
      ++i_col;
    }
    return mat;
  }

private:
  using SparseElement = std::pair<int, int>;  // (row/col, value)

  absl::flat_hash_set<std::vector<SparseElement>> unique_sparse_columns() const {
    const int num_cols = monoms_.size();
    std::vector<std::vector<SparseElement>> sparse_columns(num_cols);
    int i_row = 0;
    for (const auto& row : sparse_rows_) {
      for (const auto& [i_col, coeff] : row) {
        sparse_columns[i_col].push_back({i_row, coeff});
      }
      ++i_row;
    }
    return to_set(sparse_columns);
  }

  Enumerator<typename ExprT::ObjectT> monoms_;
  // For each row: for each non-zero value: (column, value)
  absl::flat_hash_set<std::vector<SparseElement>> sparse_rows_;
};
