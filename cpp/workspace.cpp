#include <fstream>
#include <future>
#include <iostream>
#include <regex>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

// TODO: Choose decomposition and clean up
#include "Eigen/QR"
// #include "Eigen/SVD"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/enumerator.h"
#include "lib/expr_matrix_builder.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/itertools.h"
#include "lib/lexicographical.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_space.h"
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


template<typename MatrixT>
int matrix_rank(const MatrixT& matrix) {
  return matrix.colPivHouseholderQr().rank();
}

template<typename MatrixT>
void describe_matrix(const std::string& name, Profiler& profiler, const MatrixT& matrix) {
  profiler.finish("expr");
  const int rank = matrix.colPivHouseholderQr().rank();
  profiler.finish("rank");
  std::cout << name << ": (" << matrix.rows() << ", " << matrix.cols() << ") => " << rank << std::endl;
}

template<typename ExprT>
void describe(Profiler& profiler, const ExprMatrixBuilder<ExprT>& matrix_builder) {
  profiler.finish("expr");
  const auto& matrix = matrix_builder.template make_matrix<double>();
  profiler.finish("matrix");
  // Choose decomposition
  const auto& decomp = matrix.colPivHouseholderQr();
  // Eigen::JacobiSVD<std::decay_t<decltype(matrix)>> decomp;
  // decomp.compute(matrix, Eigen::ComputeThinV);
  const int rank = decomp.rank();
  profiler.finish("rank");
  std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => " << rank << std::endl;
}


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
    .set_encoder(Encoder::ascii)
    // .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_annotation_sorting(AnnotationSorting::length)
    .set_compact_x(true)
  );


  // ExprMatrixBuilder<DeltaExpr> matrix_builder;
  ExprMatrixBuilder<DeltaNCoExpr> matrix_builder;
  Profiler profiler;


  // const std::vector points = {x1,x2,x3,x4,x5,x6,x7};
  // const auto prepare = [](const auto& expr) {
  //   return expr;
  //   // return ncomultiply(expr);
  // };
  // for (const auto& s1 : CL4(points)) {
  //   for (const auto& s2 : CB1(points)) {
  //     matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
  //   }
  // }
  // for (const auto& s1 : CB3(points)) {
  //   for (const auto& s2 : CB2(points)) {
  //     matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
  //   }
  // }
  // describe(profiler, matrix_builder);


  // static const auto prepare = [](const auto& expr) {
  //   return expr;
  //   // return ncomultiply(expr);
  // };
  // std::vector<std::future<DeltaNCoExpr>> results;
  // // Note: lambdas cannot capture structured bindings  o_O
  // //   https://stackoverflow.com/questions/46114214/lambda-implicit-capture-fails-with-variable-declared-from-structured-binding
  // // TODO: Uncomment this when C++ is fixed.
  // // for (const auto& [s1, s2] : weight5(points)) {
  // //   results.push_back(std::async([s1, s2]() { return prepare(ncoproduct(*s1, *s2)); }));
  // // }
  // for (const auto& s : polylog_space_6({x1,x2,x3,x4,x5,x6,x7})) {
  //   results.push_back(std::async([s]() {
  //     const auto& [s1, s2] = s;
  //     return prepare(ncoproduct(*s1, *s2));
  //   }));
  // }
  // for (auto& result : results) {
  //   matrix_builder.add_expr(result.get());
  // }
  // describe(profiler, matrix_builder);


  // auto expr = theta_expr_to_delta_expr(Lira3(1,1)(CR(1,2,3,4), CR(1,4,3,2)));
  // // std::cout << to_lyndon_basis(expr);
  // // std::cout << comultiply(expr, {4,1});
  // auto lhs = comultiply(expr, {4,1});
  // auto rhs =
  //   + coproduct(QLi4(1,2,3,4), QLi1(1,4,3,2))
  //   + coproduct(QLi4(1,2,3,4), QLi1(1,2,3,4))
  // ;
  // std::cout << lhs;
  // std::cout << rhs;
  // std::cout << lhs + rhs;  // ZERO


  // const int weight = 6;
  // const int num_points = 8;
  // std::vector points = mapped(seq_incl(1, num_points), [](int i) { return X(i); });
  // points.back() = Inf;
  // const auto space_mat = polylog_space_matrix(weight, points, false);
  // profiler.finish("space_mat");
  // const auto cospace_mat = polylog_space_matrix(weight, points, true);
  // profiler.finish("cospace_mat");
  // const int space_rank = matrix_rank(space_mat);
  // profiler.finish("space_rank");
  // const int cospace_rank = matrix_rank(cospace_mat);
  // profiler.finish("cospace_rank");
  // const int diff = space_rank - cospace_rank;
  // std::cout << "w=" << weight << ", p=" << num_points << ": ";
  // std::cout << space_rank << " - " << cospace_rank << " = " << diff << std::endl;


  const int num_points = 7;
  std::vector points = mapped(seq_incl(1, num_points), [](int i) { return X(i); });
  points.back() = Inf;
  std::cout << "w=6_via_l, p=" << num_points << std::endl;
  describe_matrix("space", profiler, polylog_space_matrix_6_via_l(points, false));
  describe_matrix("cospace", profiler, polylog_space_matrix_6_via_l(points, true));
}
