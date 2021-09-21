// TODO: Install LinBox through bazel
// TODO: Add tests, in particular: matrix_rank == matrix_rank_no_blocking

#include "linalg.h"

#include <linbox/linbox-config.h>
#include <givaro/givrational.h>
#include <linbox/ring/modular.h>
#include <linbox/matrix/sparse-matrix.h>
#include <linbox/solutions/rank.h>

#include "check.h"
#include "enumerator.h"
#include "file_util.h"
#include "format.h"
#include "string.h"
#include "table_printer.h"
#include "util.h"


void extract_block(
  int i_row,
  int i_col,
  int value,
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>>& rows,
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>>& cols,
  absl::flat_hash_map<std::pair<int, int>, int>& block
) {
  const auto inserted = block.insert({{i_row, i_col}, value}).second;
  if (!inserted) {
    return;
  }
  const auto row = extract_value_or(rows, i_row);
  const auto col = extract_value_or(cols, i_col);
  for (const auto& [c, v] : row) {
    extract_block(i_row, c, v, rows, cols, block);
  }
  for (const auto& [r, v] : col) {
    extract_block(r, i_col, v, rows, cols, block);
  }
}

std::vector<Matrix> get_matrix_diagonal_blocks(const Matrix& matrix) {
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>> rows;  // row -> (col, value)
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>> cols;  // col -> (row, value)
  for (const auto& t : matrix.as_triplets()) {
    rows[t.row].push_back({t.col, t.value});
    cols[t.col].push_back({t.row, t.value});
  }
  std::vector<Matrix> blocks;
  while (!rows.empty()) {
    absl::flat_hash_map<std::pair<int, int>, int> block;
    const auto it = rows.begin();
    const auto it_it = it->second.begin();
    extract_block(it->first, it_it->first, it_it->second, rows, cols, block);
    Matrix block_matrix;
    for (const auto& [rowcol, value] : block) {
      const auto [row, col] = rowcol;
      block_matrix.insert(row, col) = value;
    }
    blocks.push_back(compress_matrix(block_matrix));
  }
  CHECK(cols.empty());
  return blocks;
}

int matrix_rank_no_blocking(const Matrix& matrix) {
  const auto& triplets = matrix.as_triplets();
  if (triplets.empty()) {
    // Extend rank definition to empty matrices.
    return 0;
  } else if (triplets.size() == 1) {
    // Speed up block rank computation.
    return triplets.front().value == 0 ? 0 : 1;
  }
  // Optimization potential: Faster implementation for other small matrices (to speed up block rank).

  // LinBox::commentator().setMaxDetailLevel(-1);
  // LinBox::commentator().setMaxDepth(-1);
  // LinBox::commentator().setReportStream(std::cerr);
  Givaro::QField<Givaro::Rational> QQ;
  LinBox::SparseMatrix<Givaro::QField<Givaro::Rational>, LinBox::SparseMatrixFormat::SparseSeq> linbox_matrix(
    QQ, matrix.rows(), matrix.cols()
  );
  for (const auto& t : triplets) {
    linbox_matrix.setEntry(t.row, t.col, t.value);
  }

  size_t rank;
  // TODO: Try adding IntegerTag
  // TODO: Try other methods
  // TODO: Try LinBox::Method::SparseElimination options
  LinBox::rank(rank, linbox_matrix, LinBox::Method::SparseElimination());
  return rank;
}

// TODO: Test
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
  return table.to_string();
}

std::string dump_to_string_impl(const Matrix& matrix) {
  return absl::StrCat(
    "<", matrix.rows(), "x", matrix.cols(), " matrix, ",
    matrix.as_triplets().size(), " elements>"
  );
}

Matrix compress_matrix(const Matrix& uncompressed) {
  Enumerator<int> row_indices;
  Enumerator<int> col_indices;
  Matrix compressed;
  for (const auto& t : uncompressed.as_triplets()) {
    compressed.insert(row_indices.index(t.row), col_indices.index(t.col)) = t.value;
  }
  return compressed;
}

// Right now this usually makes rank much slower, even with one block.
//   This probably has something to do with row/col ordering.
// TODO: Fix this and use this implementation.
// int matrix_rank(const Matrix& matrix) {
//   return sum(mapped(
//     get_matrix_diagonal_blocks(matrix),
//     [](const auto& b) { return matrix_rank_no_blocking(b); }
//   ));
// }
// int matrix_rank(const Matrix& matrix) {
//   Profiler profiler;
//   const int rank_whole = matrix_rank_no_blocking(matrix);
//   profiler.finish("# whole rank");
//   const auto blocks = get_matrix_diagonal_blocks(matrix);
//   profiler.finish(absl::StrCat("# make blocks (", blocks.size(), ")"));
//   const int rank = sum(mapped(blocks, [](const auto& b) { return matrix_rank_no_blocking(b); }));
//   profiler.finish("# block rank");
//   CHECK_EQ(rank_whole, rank);
//   return rank;
// }
int matrix_rank(const Matrix& matrix) {
  return matrix_rank_no_blocking(matrix);
}


void save_triplets(const std::string& filename, const Matrix& matrix) {
  std::ofstream fs(filename);
  CHECK(fs.good());
  for (const auto& t : matrix.as_triplets()) {
    fs << t.row << " " << t.col << " " << t.value << "\n";
  }
  CHECK(fs.good());
}

Matrix load_triplets(const std::string& filename) {
  std::ifstream fs(filename);
  Matrix matrix;
  std::string line;
  while (std::getline(fs, line)) {
    std::istringstream ss(line);
    int row, col, value;
    CHECK(!!(ss >> row >> col >> value));
    matrix.insert(row, col) = value;
  }
  return matrix;
}
