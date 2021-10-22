#pragma once

#include "absl/container/flat_hash_set.h"

#include "compact_variant.h"
#include "enumerator.h"
#include "linalg.h"
#include "util.h"


template<typename... ExprTs>
class ExprMatrixBuilder {
public:
  void add_expr(const std::tuple<ExprTs...>& expressions) {
    return std::apply(
      [this](const auto&... args) { return add_expr_impl(args...); },
      expressions
    );
  }
  void add_expr(const ExprTs&... expressions) {
    return add_expr_impl(expressions...);
  }

  Matrix make_matrix() const {
    const auto sparse_columns = unique_sparse_columns();
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

private:
  using SparseElement = std::pair<int, int>;  // (row/col, value)
  using KeyT = CompactVariant<typename ExprTs::ObjectT...>;

  void add_expr_impl(const ExprTs&... expressions) {
    std::vector<SparseElement> row;
    add_columns<0>(row, expressions...);
    // TODO: Is sorting required? (col, value) pairs should be sorted in order for row
    //   deduplication to work. However chances are they are already oredered the same
    //   way: absl::hash_map iteration is likely deterministic within one launch.
    absl::c_sort(row);
    sparse_rows_.insert(row);
  }

  template<std::size_t Idx, typename Head, typename... Tail>
  void add_columns(std::vector<SparseElement>& row, const Head& head, const Tail&... tail) {
    for (const auto& [term, coeff] : head) {
      row.push_back({monoms_.index(KeyT{std::in_place_index<Idx>, term}), coeff});
    }
    add_columns<Idx + 1>(row, tail...);
  }
  template<std::size_t Idx>
  void add_columns(std::vector<SparseElement>&) {}

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
    // Note: Elements are sorted within each column since we iterate rows sequentially.
    //   Hence decuplication works.
    return to_set(sparse_columns);
  }

  Enumerator<KeyT> monoms_;

  // For each row: for each non-zero value: (column, value)
  absl::flat_hash_set<std::vector<SparseElement>> sparse_rows_;
};


template<typename... Ts>
struct GetExprMatrixBuilder { using type = ExprMatrixBuilder<Ts...>; };
template<typename... Ts>
struct GetExprMatrixBuilder<std::tuple<Ts...>> { using type = ExprMatrixBuilder<Ts...>; };
template<typename... Ts>
using GetExprMatrixBuilder_t = typename GetExprMatrixBuilder<Ts...>::type;
