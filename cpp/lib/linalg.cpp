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
#include <linbox/solutions/solve.h>

#include "check.h"
#include "compare.h"
#include "enumerator.h"
#include "sorting.h"
#include "string.h"
#include "util.h"


// Workaround link errors that I got with LinBox from `liblinbox-dev` package:
//   .../linalg.o:linalg.cpp:function LinBox::NotImplementedYet::NotImplementedYet(char const*):
//     error: undefined reference to 'LinBox::NotImplementedYet::_errorStream'
//   .../linalg.o:linalg.cpp:function LinBox::PreconditionFailed::PreconditionFailed(char const*, int, char const*):
//     error: undefined reference to 'LinBox::PreconditionFailed::_errorStream'
namespace LinBox {
  std::ostream __attribute__((weak)) *PreconditionFailed::_errorStream;
  std::ostream __attribute__((weak)) *NotImplementedYet::_errorStream;
}


template<typename Field>
LinBox::SparseMatrix<Field> to_linbox_matrix(const Matrix& matrix, const Field& field) {
  LinBox::SparseMatrix<Field> linbox_matrix(field, matrix.rows(), matrix.cols());
  // Sort triplets first. The order of calls to `setEntry` plays a huge role: `LinBox::rank`
  // can be five times slower in case of random order compared to sorted.
  const auto sorted_triplets = sorted(
    matrix.as_triplets(),
    cmp::projected([](const auto& t) { return std::pair{t.row, t.col}; })
  );
  for (const auto& t : sorted_triplets) {
    linbox_matrix.setEntry(t.row, t.col, t.value);
  }
  return linbox_matrix;
}

// Finds one diagonal block in a matrix given by `rows` and `cols`, which must be synchronized,
// i.e. for each element (col, value) in `rows` there must be a corresponding element (row, value)
// in `cols` and vice versa. Removes the elements corresponding to the block from `rows` and `cols`.
// Returns the block as a map: (row, col) -> value.
static absl::flat_hash_map<std::pair<int, int>, int> extract_block(
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

// Removes empty rows and columns.
static Matrix compress_matrix(const Matrix& uncompressed) {
  Enumerator<int> row_indices;
  Enumerator<int> col_indices;
  absl::flat_hash_map<std::pair<int, int>, int> compressed_triplets;
  for (const auto& t : uncompressed.as_triplets()) {
    compressed_triplets[{row_indices.index(t.row), col_indices.index(t.col)}] = t.value;
  }
  return Matrix::from_triplets(compressed_triplets);
}

#if 0
static Matrix compress_matrix_keep_rowcol_order(const Matrix& uncompressed) {
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

  absl::flat_hash_map<std::pair<int, int>, int> compressed_triplets;
  for (const auto& t : uncompressed.as_triplets()) {
    compressed_triplets[{row_indices.c_index(t.row), col_indices.c_index(t.col)}] = t.value;
  }
  return Matrix(compressed_triplets);
}
#endif

// Splits matrix into blocks if it's block diagonal.
static std::vector<Matrix> get_matrix_diagonal_blocks(const Matrix& matrix) {
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>> rows;  // row -> (col, value)
  absl::flat_hash_map<int, std::vector<std::pair<int, int>>> cols;  // col -> (row, value)
  for (const auto& t : matrix.as_triplets()) {
    rows[t.row].push_back({t.col, t.value});
    cols[t.col].push_back({t.row, t.value});
  }
  std::vector<Matrix> blocks;
  while (!rows.empty()) {
    const auto block = extract_block(rows, cols);
    int rows = 0, cols = 0;
    for (const auto& [rowcol, value] : block) {
      const auto [row, col] = rowcol;
      rows = std::max(rows, row + 1);
      cols = std::max(cols, col + 1);
    }
    Matrix block_matrix(rows, cols);
    for (const auto& [rowcol, value] : block) {
      const auto [row, col] = rowcol;
      block_matrix(row, col) = value;
    }
    blocks.push_back(compress_matrix(block_matrix));
  }
  CHECK(cols.empty());
  return blocks;
}

static Matrix diagonalize_matrix(const Matrix& src) {
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
    Matrix new_mat(src.rows(), src.cols());
    for (const int a : range(sorted_lines.size())) {
      for (const auto& [b, value] : sorted_lines[a]) {
        new_mat(view_type == MatrixView::RowMajor ? std::pair{a, b} : std::pair{b, a}) = value;
      }
    }
    mat = std::move(new_mat);
  }
  CHECK(mat.has_value());
  return *mat;
}

std::vector<double> linear_solve(const Matrix& matrix, const std::vector<int>& rhs) {
  CHECK_EQ(matrix.rows(), rhs.size());

  using Field = Givaro::QField<Givaro::Rational>;
  Field field;

  auto linbox_matrix = to_linbox_matrix(matrix, field);
  LinBox::DenseVector<Field> linbox_result(field, matrix.cols());
  LinBox::DenseVector<Field> linbox_rhs(field, rhs);

  try {
    LinBox::solveInPlace(linbox_result, linbox_matrix, linbox_rhs);
  } catch (const LinBox::LinboxError& e) {
    CHECK(false) << "Cannot solver linear system: " << e.what() << "\n";
  }
  return mapped(linbox_result, [](const auto& v) { return static_cast<double>(v); });
}

std::vector<int> find_kernel_vector(const Matrix& matrix) {
  using Field = Givaro::QField<Givaro::Rational>;
  Field field;

  int min_nonzero_col = std::numeric_limits<int>::max();
  for (const auto& t : matrix.as_triplets()) {
    if (t.value != 0) {
      min_nonzero_col = std::min(min_nonzero_col, t.col);
    }
  }

  LinBox::DenseVector<Field> linbox_rhs(field, matrix.rows());
  Matrix matrix_fixcol(matrix.rows(), matrix.cols());
  for (const auto& t : matrix.as_triplets()) {
    if (t.col == min_nonzero_col) {
      linbox_rhs[t.row] = -t.value;
    } else {
      matrix_fixcol(t.row, t.col) = t.value;
    }
  }

  auto linbox_matrix = to_linbox_matrix(matrix_fixcol, field);
  LinBox::DenseVector<Field> linbox_result(field, matrix_fixcol.cols());

  try {
    LinBox::solveInPlace(linbox_result, linbox_matrix, linbox_rhs);
  } catch (const LinBox::LinboxError& e) {
    CHECK(false) << "Cannot find kernel vector: " << e.what() << "\n";
  }
  // TODO: Deal with this element must be zero.
  linbox_result[min_nonzero_col] = 1;

  const int denom_lcm = absl::c_accumulate(
    linbox_result,
    1,
    [](int lcm, const auto& v) { return lcm * v.deno(); }
  );
  return mapped(linbox_result, [&](const auto& v) {
    return static_cast<int>(v.nume()) * denom_lcm / static_cast<int>(v.deno());
  });
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

  // Integers are not actually a field, but the corresponding `LinBox::SparseMatrix` parameter is
  // called "field", so using the same name for consistency.
  using Field = Givaro::ZRing<Givaro::Integer>;
  Field field;

  // Alternative field option. Performance seems comparable with `Givaro::ZRing<Givaro::Integer>`,
  // but a little bit worse (hard to tell for sure because my benchmark was noisy).
  //   using Field = Givaro::QField<Givaro::Rational>;
  //   Field field;

  // TODO: Figure out why computing rank modulo prime works so weird. In my experience it even returned
  // non-zero ranks for matrix that contains only zero elements: it was as if it treated unset as zero
  // and zero as non-zero.
  //   using Field = Givaro::Modular<int>;
  //   Field field(1000003u);

  auto linbox_matrix = to_linbox_matrix(matrix, field);

  size_t rank;
  // TODO: Try other methods, esp. LinBox::Method::Wiedemann and LinBox::Method::Blackbox
  // TODO: Try LinBox::Method::SparseElimination options
  LinBox::rank(rank, linbox_matrix);
  return rank;
}

// Computes matrix rank after preconditioning. This is not guaranteed to be faster than
// `matrix_rank_raw_linbox`. In particular, it could to be slower when there is only block.
// However it tends to be faster on average, especially for large matrices.
static int matrix_rank_preconditioned(const Matrix& matrix) {
  return sum(
    get_matrix_diagonal_blocks(matrix),
    [](const auto& b) {
      return matrix_rank_raw_linbox(diagonalize_matrix(sort_unique_rows(sort_unique_cols(b))));
    }
  );
}

#if 1
int matrix_rank(const Matrix& matrix) {
  return matrix_rank_preconditioned(matrix);
}
#else
int matrix_rank(const Matrix& matrix) {
  Profiler profiler;
  const int rank_orig = matrix_rank_raw_linbox(matrix);
  profiler.finish("rank (raw)");
  const int rank_preconditioned = matrix_rank_preconditioned(matrix);
  profiler.finish("rank (preconditioned)");
  CHECK_EQ(rank_orig, rank_preconditioned);
  return rank_orig;
}
#endif
