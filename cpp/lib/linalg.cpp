// TODO: Install LinBox through bazel
// TODO: Add tests, in particular: matrix_rank == matrix_rank_raw_linbox
// TODO: Add benchmarks, in particular: matrix_rank vs matrix_rank_raw_linbox

#include "linalg.h"

#include <queue>

#include <linbox/linbox-config.h>
#include <givaro/givrational.h>
#include <linbox/ring/modular.h>
#include <linbox/matrix/sparse-matrix.h>
#include <linbox/solutions/rank.h>

#include "check.h"
#include "compare.h"
#include "enumerator.h"
#include "file_util.h"
#include "format.h"
#include "sorting.h"
#include "string.h"
#include "table_printer.h"
#include "util.h"


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


// Finds one diagonal block in a matrix given by `rows` and `cols`, which must be synchronized,
// i.e. for each element (col, value) in `rows` there must be a corresponding element (row, value)
// in `cols` and vice versa. Removes the elements corresponding to the block from `rows` and `cols`.
// Returns the block as a map: (row, col) -> value.
absl::flat_hash_map<std::pair<int, int>, int> extract_block(
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>>& rows,
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>>& cols
) {
  absl::flat_hash_map<std::pair<int, int>, int> block;
  std::queue<std::pair<int, int>> extraction_queue;  // queue of (row, col)
  const auto it = rows.begin();
  const auto it_it = it->second.begin();
  extraction_queue.push({it->first, it_it->first});
  while (!extraction_queue.empty()) {
    const auto [i_row, i_col] = extraction_queue.front();
    extraction_queue.pop();
    const auto row = extract_value_or(rows, i_row);
    const auto col = extract_value_or(cols, i_col);
    for (const auto& [c, v] : row) {
      if (block.insert({{i_row, c}, v}).second) {
        extraction_queue.push({i_row, c});
      }
    }
    for (const auto& [r, v] : col) {
      if (block.insert({{r, i_col}, v}).second) {
        extraction_queue.push({r, i_col});
      }
    }
  }
  return block;
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
    const auto block = extract_block(rows, cols);
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

int matrix_rank_raw_linbox(const Matrix& matrix) {
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
  // Sort triplets first. The order of calls to `setEntry` plays a huge role: `LinBox::rank`
  // can be five times slower in case of random order compared to sorted.
  const auto sorted_triplets = sorted(
    triplets,
    cmp::projected([](const auto& t) { return std::pair{t.row, t.col}; })
  );
  for (const auto& t : sorted_triplets) {
    linbox_matrix.setEntry(t.row, t.col, t.value);
  }

  size_t rank;
  // TODO: Try adding IntegerTag
  // TODO: Try other methods, esp. LinBox::Method::Wiedemann
  // TODO: Try LinBox::Method::SparseElimination options
  // TODO: Try (optionally) computing rank modulo prime
  LinBox::rank(rank, linbox_matrix, LinBox::Method::SparseElimination());
  return rank;
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

Matrix compress_matrix_keep_rowcol_order(const Matrix& uncompressed) {
  std::vector<int> row_index_vector;
  std::vector<int> col_index_vector;
  for (const auto& t : uncompressed.as_triplets()) {
    row_index_vector.push_back(t.row);
    col_index_vector.push_back(t.col);
  }
  absl::c_sort(row_index_vector);
  absl::c_sort(col_index_vector);

  Enumerator<int> row_indices;
  Enumerator<int> col_indices;
  for (const auto& idx : row_index_vector) {
    row_indices.index(idx);
  }
  for (const auto& idx : col_index_vector) {
    col_indices.index(idx);
  }

  Matrix compressed;
  for (const auto& t : uncompressed.as_triplets()) {
    compressed.insert(row_indices.c_index(t.row), col_indices.c_index(t.col)) = t.value;
  }
  return compressed;
}

Matrix diagonalize_matrix(const Matrix& src) {
  using CoordValue = std::pair<int, int>;
  std::optional<Matrix> mat;
  for (const auto view_type : {MatrixView::RowMajor, MatrixView::ColumnMajor}) {
    const MatrixView view{mat ? &*mat : &src, view_type};
    std::vector<std::vector<CoordValue>> lines(view.a_size());
    for (const auto& t : view.as_triplets()) {
      lines.at(t.a).push_back({t.b, t.value});
    }
    const auto sorted_lines = sorted_by_projection(lines, [](const auto& line) {
      // Note. Also tried sorting by average coord, but performance was worse.
      //   Worse than the original even in many cases.
      const auto [min, max] = minmax_value(line);
      return (max.first - min.first);
    });
    Matrix new_mat;
    for (const int a : range(sorted_lines.size())) {
      for (const auto& [b, value] : sorted_lines[a]) {
        new_mat.insert(view_type == MatrixView::RowMajor ? std::pair{a, b} : std::pair{b, a}) = value;
      }
    }
    mat = new_mat;
  }
  CHECK(mat.has_value());
  return *mat;
}

static int matrix_rank_fancy(const Matrix& matrix) {
  return sum(mapped(
    get_matrix_diagonal_blocks(matrix),
    [](const auto& b) { return matrix_rank_raw_linbox(diagonalize_matrix(b)); }
  ));
}

#if 1
int matrix_rank(const Matrix& matrix) {
  return matrix_rank_fancy(matrix);
}
#else
int matrix_rank(const Matrix& matrix) {
  Profiler profiler;
  const int rank_orig = matrix_rank_raw_linbox(matrix);
  profiler.finish("rank (raw)");
  const int rank_fancy = matrix_rank_fancy(matrix);
  profiler.finish("rank (fancy)");
  CHECK_EQ(rank_orig, rank_fancy);
  return rank_orig;
}
#endif

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
