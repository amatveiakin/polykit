#pragma once

#include "absl/container/flat_hash_set.h"

#include "compact_variant.h"
#include "enumerator.h"
#include "linalg.h"
#include "util.h"


namespace internal {
using SparseElement = std::pair<int, int>;  // (row/col, value)

Matrix make_matrix(
  const std::vector<std::vector<SparseElement>>& sparse_rows, int num_cols
);
}  // namespace internal


template<typename... ExprTs>
class ExprTupleMatrixBuilder {
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
    return internal::make_matrix(sparse_rows_, monoms_.size());
  }

private:
  using SparseElement = internal::SparseElement;
  using KeyT = CompactVariant<typename ExprTs::ObjectT...>;

  void add_expr_impl(const ExprTs&... expressions) {
    std::vector<SparseElement> row;
    add_columns<0>(row, expressions...);
    sparse_rows_.push_back(std::move(row));
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

  Enumerator<KeyT> monoms_;

  // For each row: for each non-zero value: (column, value)
  std::vector<std::vector<SparseElement>> sparse_rows_;
};

template<typename ExprT>
class ExprVectorMatrixBuilder {
public:
  void add_expr(const std::vector<ExprT>& expressions) {
    auto row = make_row(expressions);
    sparse_rows_.push_back(std::move(row));
  }

  Matrix make_matrix() const {
    return internal::make_matrix(sparse_rows_, monoms_.size());
  }

private:
  using SparseElement = internal::SparseElement;
  using KeyT = std::pair<int, typename ExprT::ObjectT>;

  std::vector<SparseElement> make_row(const std::vector<ExprT>& expressions) {
    std::vector<SparseElement> row;
    for (const int expr_idx : range(expressions.size())) {
      const auto& expr = expressions.at(expr_idx);
      for (const auto& [term, coeff] : expr) {
        row.push_back({monoms_.index(KeyT{expr_idx, term}), coeff});
      }
    };
    return row;
  }

  Enumerator<KeyT> monoms_;

  // For each row: for each non-zero value: (column, value)
  std::vector<std::vector<SparseElement>> sparse_rows_;
};


template<typename... Ts>
struct GetExprMatrixBuilder { using type = ExprTupleMatrixBuilder<Ts...>; };
template<typename... Ts>
struct GetExprMatrixBuilder<std::tuple<Ts...>> { using type = ExprTupleMatrixBuilder<Ts...>; };
template<typename T>
struct GetExprMatrixBuilder<std::vector<T>> { using type = ExprVectorMatrixBuilder<T>; };
template<typename... Ts>
using GetExprMatrixBuilder_t = typename GetExprMatrixBuilder<Ts...>::type;
