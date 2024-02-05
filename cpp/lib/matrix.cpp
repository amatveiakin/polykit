#include "matrix.h"

#include <fstream>

#include "compare.h"
#include "format.h"
#include "table_printer.h"
#include "util.h"


static std::pair<int, int> detect_shape(absl::flat_hash_map<std::pair<int, int>, int> triplets) {
  int rows = 0, cols = 0;
  for (const auto& [rowcol, value] : triplets) {
    const auto [row, col] = rowcol;
    rows = std::max(rows, row + 1);
    cols = std::max(cols, col + 1);
  }
  return {rows, cols};
}

Matrix Matrix::from_triplets(absl::flat_hash_map<std::pair<int, int>, int> triplets) {
  const auto shape = detect_shape(triplets);
  return Matrix(shape, std::move(triplets));
}

Matrix::Matrix(std::pair<int, int> shape, absl::flat_hash_map<std::pair<int, int>, int> triplets)
  : Matrix(shape)
{
  const auto content_shape = detect_shape(triplets);
  CHECK_LE(content_shape.first, shape.first);
  CHECK_LE(content_shape.second, shape.second);
  triplets_ = std::move(triplets);
}

std::vector<Matrix::Triplet> Matrix::as_triplets() const {
  std::vector<Triplet> result;
  for (const auto& [rowcol, value] : triplets_) {
    result.push_back({rowcol.first, rowcol.second, value});
  }
  return result;
}

Matrix Matrix::transposed() const {
  Matrix ret(cols(), rows());
  for (const auto& t : as_triplets()) {
    ret(t.col, t.row) = t.value;
  }
  return ret;
}


MatrixView::MatrixView(const Matrix* matrix, ViewType view_type)
    : matrix_(matrix), view_type_(view_type) {
  CHECK(matrix_ != nullptr);
}

std::vector<MatrixView::Triplet> MatrixView::as_triplets() const {
  return mapped(
    matrix_->as_triplets(), [&](const auto& t) {
      return view_type_ == RowMajor
          ? MatrixView::Triplet{t.row, t.col, t.value}
          : MatrixView::Triplet{t.col, t.row, t.value};
    }
  );
}


// Sorts vector of (row, val) or (col, val) pairs so that the resulting the order is what
// it would be for a dense vector where missing elements are filled with zeros.
static bool sparse_vector_less(
  std::vector<std::pair<int, int>>& lhs,
  std::vector<std::pair<int, int>>& rhs
) {
  const int inf = std::numeric_limits<int>::max();
  const int n = std::max(lhs.size(), rhs.size());
  for (const int i : range(n)) {
    const auto [lp, lv] = (i < lhs.size()) ? lhs[i] : std::pair{inf, 0};
    const auto [rp, rv] = (i < rhs.size()) ? rhs[i] : std::pair{inf, 0};
    CHECK(lp == inf || lv != 0);
    CHECK(rp == inf || rv != 0);
    if (lp < rp) {
      return lv < 0;
    } else if (lp > rp) {
      return rv > 0;
    } else if (lv != rv) {
      return lv < rv;
    }
  }
  return false;
}

Matrix sort_unique_rows(const Matrix& matrix) {
  std::vector<std::vector<std::pair<int, int>>> rows(matrix.rows());
  for (const auto& t : matrix.as_triplets()) {
    rows[t.row].push_back({t.col, t.value});
  }
  for (auto& colvalues : rows) {
    absl::c_sort(colvalues);
  }
  auto sorted_rows = sorted(rows, sparse_vector_less);
  keep_unique_sorted(sorted_rows);
  absl::flat_hash_map<std::pair<int, int>, int> triplets;
  int row = 0;
  for (const auto& colvalues : sorted_rows) {
    for (const auto& [col, value] : colvalues) {
      triplets[{row, col}] = value;
    }
    ++row;
  }
  return Matrix(std::pair{row, matrix.cols()}, std::move(triplets));
}

Matrix sort_unique_cols(const Matrix& matrix) {
  return sort_unique_rows(matrix.transposed()).transposed();
}

static std::string matrix_header(const Matrix& matrix) {
  return absl::StrCat(
    matrix.rows(), "x", matrix.cols(), " matrix, ",
    matrix.as_triplets().size(), " elements"
  );
}

std::string to_string(const Matrix& matrix) {
  const int kMaxColumns = 16;
  const int kMaxRows = 16;
  const int kEllipsisSize = 3;

  const int kFirstRowsToPrint = (kMaxRows - kEllipsisSize + 1) / 2;
  const int kLastRowsToPrint = (kMaxRows - kEllipsisSize) / 2;
  const int kFirstColumnsToPrint = (kMaxColumns - kEllipsisSize + 1) / 2;
  const int kLastColumnsToPrint = (kMaxColumns - kEllipsisSize) / 2;

  const bool full_rows = matrix.rows() <= kMaxRows;
  const bool full_cols = matrix.cols() <= kMaxColumns;
  const int table_rows = full_rows ? matrix.rows() : kMaxRows;
  const int table_cols = full_cols ? matrix.cols() : kMaxColumns;

  const auto is_ellipsis_row = [&](int table_row) {
    return !full_rows && table_row >= kFirstRowsToPrint && table_row < kMaxRows - kLastRowsToPrint;
  };
  const auto is_ellipsis_col = [&](int table_col) {
    return !full_cols && table_col >= kFirstColumnsToPrint && table_col < kMaxColumns - kLastColumnsToPrint;
  };
  const auto get_table_row = [&](int matrix_row) -> std::optional<int> {
    if (full_rows || matrix_row < kFirstRowsToPrint) {
      return matrix_row;
    }
    if (matrix_row >= matrix.rows() - kLastRowsToPrint) {
      return matrix_row + kMaxRows - matrix.rows();
    }
    return std::nullopt;
  };
  const auto get_table_col = [&](int matrix_col) -> std::optional<int> {
    if (full_cols || matrix_col < kFirstColumnsToPrint) {
      return matrix_col;
    }
    if (matrix_col >= matrix.cols() - kLastColumnsToPrint) {
      return matrix_col + kMaxColumns - matrix.cols();
    }
    return std::nullopt;
  };

  TablePrinter table;
  table.set_default_alignment(TextAlignment::right);
  table.set_min_column_width(2);
  for (const auto& triplet : matrix.as_triplets()) {
    const auto row = get_table_row(triplet.row);
    const auto col = get_table_col(triplet.col);
    if (row.has_value() && col.has_value()) {
      table.set_content({*row, *col}, fmt::num(triplet.value));
    }
  }
  for (const int row : range(table_rows)) {
    for (const int col : range(table_cols)) {
      const bool ellipsis_row = is_ellipsis_row(row);
      const bool ellipsis_col = is_ellipsis_col(col);
      if (ellipsis_row || ellipsis_col) {
        CHECK(table.content({row, col}).empty());
        if (ellipsis_row != ellipsis_col) {
          table.set_content({row, col}, ".");
        }
      } else {
        if (table.content({row, col}).empty()) {
          table.set_content({row, col}, fmt::num(0));
        }
      }
    }
  }
  return absl::StrCat(
    matrix_header(matrix), ":\n",
    table.to_string()
  );
}

std::string dump_to_string_impl(const Matrix& matrix) {
  return absl::StrCat("<", matrix_header(matrix), ">");
}

void save_triplets(const std::string& filename, const Matrix& matrix) {
  std::ofstream fs(filename);
  CHECK(fs.good());
  fs << matrix.rows() << " " << matrix.cols() << "\n";
  fs << matrix.num_triplets() << "\n";
  const auto triplets_sorted = sorted(
    matrix.as_triplets(),
    cmp::projected([](const auto& t) { return std::tie(t.row, t.col); })
  );
  for (const auto& t : triplets_sorted) {
    fs << t.row << " " << t.col << " " << t.value << "\n";
  }
  CHECK(fs.good());
}

Matrix load_triplets(const std::string& filename) {
  std::ifstream fs(filename);
  int num_rows, num_cols, num_triplets;
  CHECK(!!(fs >> num_rows >> num_cols >> num_triplets));
  Matrix matrix(num_rows, num_cols);
  for (EACH : range(num_triplets)) {
    int row, col, value;
    CHECK(!!(fs >> row >> col >> value));
    matrix(row, col) = value;
  }
  return matrix;
}
