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
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/packed.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"
#include "lib/zip.h"


LoopExpr normalize_loops_expr(const LoopExpr& expr) {
  return arg9_semi_lyndon(fully_normalize_loops(expr));
}


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(10)
    .set_annotation_sorting(AnnotationSorting::length)
  );

  const std::vector points = {1,2,1,3,1,4,5,4};
  const int num_points = points.size();
  auto source = LiQuad(num_points / 2 - 1, points);

  std::cout << source;
  auto lira_expr = without_unities(theta_expr_to_lira_expr_without_products(source.without_annotations()));
  auto expr = lira_expr_to_loop_expr(lira_expr);
  std::cout << expr;
  expr = normalize_loops_expr(expr);
  std::cout << expr;
  expr = loops_var5_shuffle_internally(expr);
  std::cout << expr;
}
