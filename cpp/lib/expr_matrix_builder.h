#pragma once

#include "absl/container/flat_hash_set.h"

#include "compact_variant.h"
#include "enumerator.h"
#include "linalg.h"
#include "util.h"


template<typename... ExprTs>
class ExprMatrixBuilder {
public:
  // TODO: Don't create an intermediate tuple. Implement tuple version via non-tuple instead.
  void add_expr(const std::tuple<ExprTs...>& expressions) {
    std::vector<SparseElement> row;
    add_columns(row, expressions);
    sparse_rows_.insert(row);
  }
  void add_expr(const ExprTs&... expressions) {
    return add_expr(std::tie(expressions...));
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

  template<typename E>
  struct ExprObjectType {
    using type = typename E::ObjectT;
  };
  template<typename E>
  using ExprObjectType_t = typename ExprObjectType<E>::type;

  // TODO: Make this work and remove `ExprObjectType_t`:
  //   CompactVariant<typename ExprTs::ObjectT...>
  using KeyT = CompactVariant<ExprObjectType_t<ExprTs>...>;

  template<std::size_t Idx = 0>
  typename std::enable_if<(Idx < sizeof...(ExprTs)), void>::type
  add_columns(std::vector<SparseElement>& row, const std::tuple<ExprTs...>& expressions) {
    for (const auto& [term, coeff] : std::get<Idx>(expressions)) {
      row.push_back({monoms_.index(KeyT{std::in_place_index<Idx>, term}), coeff});
    }
    add_columns<Idx + 1>(row, expressions);
  }
  template<std::size_t Idx>
  typename std::enable_if<Idx == sizeof...(ExprTs), void>::type
  add_columns(std::vector<SparseElement>&, const std::tuple<ExprTs...>&) {}

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
