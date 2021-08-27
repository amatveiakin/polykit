#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "Eigen/SparseQR"

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


auto CB1(const XArgs& xargs) {
  const auto& args = xargs.as_x();
  const int n = args.size();
  std::vector<DeltaExpr> ret;
  for (const int i : range(n)) {
    for (const int j : range(i+2, n)) {
      const int ip = i + 1;
      CHECK_LT(ip, n);
      const int jp = (j + 1) % n;
      if (jp != i) {
        ret.push_back(Log(args[i], args[j], args[ip], args[jp]));
      }
    }
  }
  return ret;
}
auto CB2(const XArgs& args) {
  const auto head = slice(args.as_x(), 0, 1);
  const auto tail = slice(args.as_x(), 1);
  return mapped(combinations(tail, 3), [&](const auto& p) { return QLi2(concat(head, p)); });
}
auto CB3(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return QLi3(p); });
}
auto CB4(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return QLi4(p); });
}
auto CB5(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return QLi5(p); });
}

auto CL4(const XArgs& args) {
  return concat(
    CB4(args),
    mapped(combinations(args.as_x(), 5), [](const auto& p) { return A2(p); })
  );
}
auto CL5(const XArgs& args) {
  return concat(
    CB5(args),
    mapped(combinations(args.as_x(), 6), [](const auto& p) { return QLi5(p); })
  );
}


template<typename ExprT>
void describe(Profiler& matrix_profiler, const ExprMatrixBuilder<ExprT>& matrix_builder) {
  matrix_profiler.finish("matrix");
  const auto& matrix = matrix_builder.make_matrix();

  Profiler profiler;
  Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> decomp(matrix);
  const int rank = decomp.rank();
  profiler.finish("rank");
  std::cout << "(" << matrix.rows() << ", " << matrix.cols() << ") => " << rank << "\n";
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
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_annotation_sorting(AnnotationSorting::length)
    .set_compact_x(true)
  );


  ExprMatrixBuilder<DeltaNCoExpr> matrix_builder;
  Profiler profiler;


  // const std::vector points = {x1,x2,x3,x4,x5,x6,x7};
  // const auto prepare = [](const auto& expr) {
  //   return ncomultiply(expr);
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
  // describe(profiler, matrix_builder);  // 1407

  // const std::vector points = {x1,x2,x3,x4,x5,x6,x7};
  // const auto prepare = [](const auto& expr) {
  //   return ncomultiply(expr);
  // };
  // for (const auto& s1 : CL5(points)) {
  //   for (const auto& s2 : CB1(points)) {
  //     matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
  //   }
  // }
  // for (const auto& s1 : CL4(points)) {
  //   for (const auto& s2 : CB2(points)) {
  //     matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
  //   }
  // }
  // for (const auto& s1 : CB3(points)) {
  //   for (const auto& s2 : CB3(points)) {
  //     matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
  //   }
  // }
  // describe(profiler, matrix_builder);


  const std::vector points = {x1,x2,x3,x4,x5,x6};
  const auto prepare = [](const auto& expr) {
    return ncomultiply(expr);
  };
  for (const auto& s1 : CL4(points)) {
    for (const auto& s2 : CB1(points)) {
      matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
    }
  }
  for (const auto& s1 : CB3(points)) {
    for (const auto& s2 : CB2(points)) {
      matrix_builder.add_expr(prepare(ncoproduct(s1, s2)));
    }
  }
  describe(profiler, matrix_builder);
}
