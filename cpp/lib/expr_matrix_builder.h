#include "Eigen/Core"
#include "Eigen/SparseCore"

#include "enumerator.h"


template<typename ExprT>
class ExprMatrixBuilder {
public:
  void add_expr(const ExprT& expr) {
    sparse_columns.push_back({});
    auto& col = sparse_columns.back();
    expr.foreach([&](const auto& term, int coeff) {
      col.push_back({monoms_.index(term), coeff});
    });
  }

  Eigen::SparseMatrix<double> make_matrix() const {
    const int num_rows = monoms_.size();
    const int num_cols = sparse_columns.size();
    Eigen::SparseMatrix<double> mat(num_rows, num_cols);
    for (const int col : range(sparse_columns.size())) {
      for (const auto& [row, coeff] : sparse_columns[col]) {
        mat.insert(row, col) = coeff;
      }
    }
    mat.makeCompressed();
    return mat;
  }

private:
  Enumerator<typename ExprT::ObjectT> monoms_;
  std::vector<std::vector<std::pair<int, int>>> sparse_columns;  // col -> (row, value)
};
