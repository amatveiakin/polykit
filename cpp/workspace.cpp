#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "Eigen/Dense"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/enumerator.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/permutations.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/pvector.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"
#include "lib/zip.h"


template<typename First, typename Second, typename... Tail>
bool constexpr all_equal(const First& first, const Second& second, const Tail&... tail) {
  return first == second && all_equal(second, tail...);
}
template<typename First, typename Second>
bool constexpr all_equal(const First& first, const Second& second) {
  return first == second;
}


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

  // TODO: Try sparse matrices instead.
  Eigen::MatrixXd make_matrix() const {
    const int num_rows = monoms_.size();
    const int num_cols = sparse_columns.size();
    Eigen::MatrixXd mat = Eigen::MatrixXd::Zero(num_rows, num_cols);
    for (const int col : range(sparse_columns.size())) {
      for (const auto& [row, coeff] : sparse_columns[col]) {
        mat(row, col) = coeff;
      }
    }
    return mat;
  }

private:
  Enumerator<typename ExprT::ObjectT> monoms_;
  std::vector<std::vector<std::pair<int, int>>> sparse_columns;  // col -> (row, value)
};


template<typename ExprT>
void describe(Profiler& matrix_profiler, const ExprMatrixBuilder<ExprT>& matrix_builder) {
  matrix_profiler.finish("Matrix");
  const auto& matrix = matrix_builder.make_matrix();

  // Profiler profiler;
  // Eigen::FullPivLU<Eigen::MatrixXd> decomp(matrix);
  // const int rank = decomp.rank();
  // profiler.finish("rank");
  // std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => " << rank << "\n";

  Profiler profiler;
  const int lu_rank = Eigen::FullPivLU<Eigen::MatrixXd>(matrix).rank();
  profiler.finish("LU");
  const int qr_rank = Eigen::ColPivHouseholderQR<Eigen::MatrixXd>(matrix).rank();
  profiler.finish("QR");
  const int cod_rank = Eigen::CompleteOrthogonalDecomposition<Eigen::MatrixXd>(matrix).rank();
  profiler.finish("COD");
  const int bdcsvd_rank = Eigen::BDCSVD<Eigen::MatrixXd>(matrix).rank();
  profiler.finish("BDCSVD");
  if (lu_rank == qr_rank && qr_rank == cod_rank && cod_rank == bdcsvd_rank) {
    std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => " << lu_rank << "\n";
  } else {
    std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => ";
    std::cout << "[LU=" << lu_rank << ", QR=" << qr_rank;
    std::cout << ", COD=" << cod_rank << ", BDCSVD=" << bdcsvd_rank << "]\n";
  }
}


// def include_permutation(permutation, max_point):
//     return is_sorted(permutation[max_point:])

// def add_expr(matrix_builder, prepare, func, permutation, indices):
//     if include_permutation(permutation, max(indices)):
//         expr = func(substitute(indices, permutation))
//         matrix_builder.add_expr(prepare(expr))

template<typename T>
bool include_permutation(const std::vector<T>& permutation, int max_point) {
  return std::is_sorted(permutation.begin() + max_point, permutation.end());
}

template<typename ExprT, typename PrepareF, typename FuncF, typename T>
void add_expr(
    ExprMatrixBuilder<ExprT>& matrix_builder,
    const PrepareF& prepare,
    const FuncF& func,
    const std::vector<T>& permutation,
    const std::initializer_list<int>& indices
) {
  if (include_permutation(permutation, std::max(indices))) {
    const auto expr = func(choose_indices_one_based(permutation, indices));
    matrix_builder.add_expr(prepare(expr));
  }
}




int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_annotation_sorting(AnnotationSorting::length)
    // .set_compact_x(true)
  );



  Eigen::initParallel();
  Eigen::setNbThreads(4);

  Profiler profiler;


  // ExprMatrixBuilder<DeltaExpr> matrix_builder;
  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(expr);
  // };
  // for (const auto& args : permutations({1,2,3,4})) {
  //   add_expr(matrix_builder, prepare, DISAMBIGUATE(Corr), args, {1,1,2,3,4});
  // }
  // describe(matrix_builder);
  // for (const auto& args : permutations({1,2,3,4})) {
  //   add_expr(matrix_builder, prepare, DISAMBIGUATE(Corr), args, {1,1,1,3,4});
  // }
  // describe(matrix_builder);


  // ExprMatrixBuilder<DeltaExpr> matrix_builder;
  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(expr);
  // };
  // for (const auto& args : permutations({x1,x2,x3,x4,x5,x6,x7,x8})) {
  //   add_expr(matrix_builder, prepare, DISAMBIGUATE(QLi4), args, {1,2,3,4});
  //   add_expr(matrix_builder, prepare, DISAMBIGUATE(QLi4), args, {1,2,1,3,4,5});
  // }
  // describe(profiler, matrix_builder);



  // === Python ===
  // Computing... |################################| 40320/40320   ETA 0:00:00
  // Profiler: expr took 41.940 s (user: 41.460 s, system: 2.580 s)
  // Profiler: rank took 15.680 s (user: 91.210 s, system: 21.000 s)
  // (2868, 6720) [4.14% nonzero] => 143

  ExprMatrixBuilder<DeltaCoExpr> matrix_builder;
  const auto prepare = [](const auto& expr) {
    return comultiply(expr, {2,2});
  };
  for (const auto& args : permutations({x1,x2,x3,x4,-x1,-x2,-x3,-x4})) {
    add_expr(matrix_builder, prepare, DISAMBIGUATE(QLi4), args, {1,2,1,3,4,5});
  }
  describe(profiler, matrix_builder);
}
