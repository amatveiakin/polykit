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



  // const int num_points = 9;
  // Profiler profiler;
  // auto qli_expr = sum_looped_vec(
  //   [&](SpanX args) {
  //     return QLiVec(6, args);
  //   },
  //   num_points,
  //   seq_incl(1, num_points - 1)
  // );
  // profiler.finish("expr");
  // auto qli_comult = comultiply(qli_expr, {2,2,2});
  // profiler.finish("comultiply");
  // std::cout << "Comult " << qli_comult;


  // auto expr =
  //   // QLi6(1,2,3,4,5,6)  // zero
  //   // theta_expr_to_delta_expr(Lira4(2,1)(CR(1,2,3,4), CR(1,4,5,2)))  // zero
  //   Corr(1,2,3,4,5,6,7)  // not zero
  // ;
  // std::cout << comultiply(expr, {2,2,2});



  auto liquad_expr = LiQuad(3, {1,2,3,4,5,6,7,8});
  std::cout << "LiQuad formal " << liquad_expr;
  auto liquad_coexpr = lira_expr_comultiply(theta_expr_to_lira_expr_without_products(liquad_expr));
  std::cout << "LiQuad " << liquad_coexpr;

  auto qli_expr = QLi6(1,2,3,4,5,6,7,8);
  Profiler profiler;
  auto qli_comult = comultiply(qli_expr, {2,2,2});  // not zero
  profiler.finish("comultiply");
  // std::cout << "Comult " << prnt::header_only(qli_comult);
  std::cout << "Comult " << qli_comult;

  // std::cout << "Diff " << qli_comult + 2*liquad_coexpr;
  std::cout << "Diff " << qli_comult + liquad_coexpr;



  // auto expr = QLi6(1,2,3,4,5,6);
  // std::cout << comultiply(expr, {1,2,3});  // zero



  // const std::vector points = {1,2,1,3,1,4,5,4};
  // const int num_points = points.size();
  // auto source = LiQuad(num_points / 2 - 1, points);

  // std::cout << source;
  // auto lira_expr = without_unities(theta_expr_to_lira_expr_without_products(source.without_annotations()));
  // auto expr = lira_expr_to_loop_expr(lira_expr);
  // std::cout << expr;
  // expr = normalize_loops_expr(expr);
  // std::cout << expr;
  // expr = loops_var5_shuffle_internally(expr);
  // std::cout << expr;
}
