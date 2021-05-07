#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
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


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_annotation_sorting(AnnotationSorting::length)
  );


  // Profiler profiler;
  // const int total_points = 8;
  // const int weight = 5;
  // DeltaExpr expr;
  // for (int num_args = 4; num_args <= total_points; num_args += 2) {
  //   for (const auto& seq : increasing_sequences(total_points, num_args)) {
  //     const auto args = mapped(seq, [](int x) { return x + 1; });
  //     const int sign = neg_one_pow(sum(args) + num_args / 2);
  //     expr += sign * QLiSymmVec(weight, args);
  //   }
  // }
  // profiler.finish("expr");
  // expr = to_lyndon_basis(expr);
  // profiler.finish("lyndon");
  // CHECK(expr.is_zero());


  Profiler profiler;
  auto expr = QLi6(1,2,3,4,5,6);
  profiler.finish("expr");
  expr = to_lyndon_basis(expr);
  profiler.finish("lyndon");
  auto coexpr = comultiply(expr, {3,3});
  profiler.finish("comultiply");
  std::cout << "Checksum = " << expr.l1_norm() + coexpr.l1_norm() << "\n";
}
