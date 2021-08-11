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


  // auto expr =
  //   + 2*QLi2(1,2,3,4)
  //   - 2*QLi2(1,2,3,5)
  //   + 2*QLi2(1,2,4,5)
  //   - 2*QLi2(1,3,4,5)
  //   + 2*QLi2(2,3,4,5)
  // ;
  // expr +=
  //   + 2*shuffle_product_expr(QLi1(1,2,3,5), QLi1(1,3,4,5))
  //   -   shuffle_product_expr(QLi1(1,2,4,5), QLi1(1,2,4,5))
  // ;
  // std::cout << expr;


  // auto a =
  //   + 2*QLi2(1,2,3,4)
  //   - 2*QLi2(1,2,3,5)
  //   + 2*QLi2(1,2,4,5)
  //   - 2*QLi2(1,3,4,5)
  //   + 2*QLi2(2,3,4,5)
  // ;
  // auto b =
  //   + shuffle_product_expr(QLi1(1,2,3,4), Log(1,2,3,4))
  //   - shuffle_product_expr(QLi1(1,2,3,5), Log(1,2,3,5))
  //   + shuffle_product_expr(QLi1(1,2,4,5), Log(1,2,4,5))
  //   - shuffle_product_expr(QLi1(1,3,4,5), Log(1,3,4,5))
  //   + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
  // ;
  // std::cout << a;
  // std::cout << b;
  // std::cout << a - b;



  // auto qli2symm = [](const XArgs& args) {
  //   auto [x1, x2, x3, x4, x5, x6] = to_array<6>(args.as_int());
  //   return
  //     + QLi2(x1,x2,x3,x4,x5,x6)
  //     - QLi2(x1,x2,x3,x4)
  //     - QLi2(x1,x2,x5,x6)
  //     - QLi2(x3,x4,x5,x6)
  //   ;
  // };
  // auto a = qli2symm({1,2,3,4,5,6}) + qli2symm({2,3,4,5,6,1});
  // auto b =
  //   // + shuffle_product_expr(QLi1(1,2,3,4), QLi1(1,4,5,6))
  //   // - shuffle_product_expr(QLi1(2,3,4,5), QLi1(2,5,6,1))
  //   // + shuffle_product_expr(QLi1(3,4,5,6), QLi1(3,6,1,2))
  //   // - shuffle_product_expr(QLi1(4,5,6,1), QLi1(4,1,2,3))
  //   // + shuffle_product_expr(QLi1(5,6,1,2), QLi1(5,2,3,4))
  //   // - shuffle_product_expr(QLi1(6,1,2,3), QLi1(6,3,4,5))
  //   // - shuffle_product_expr(QLi1(1,3,4,6), Log(1,3,4,6))

  //   + shuffle_product_expr(QLi1(1,2,3,4), Log(1,2,3,4))
  //   + shuffle_product_expr(QLi1(1,2,5,6), Log(1,2,5,6))
  //   + shuffle_product_expr(QLi1(3,4,5,6), Log(3,4,5,6))
  //   + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
  //   + shuffle_product_expr(QLi1(2,3,6,1), Log(2,3,6,1))
  //   + shuffle_product_expr(QLi1(4,5,6,1), Log(4,5,6,1))
  // ;
  // std::cout << a;
  // std::cout << b;
  // std::cout << a + b;


  auto a = QLi2(1,2,3,4,5,6);
  auto b =
    - QLi2(1,2,3,5)
    + QLi2(1,2,3,6)
    + QLi2(1,2,4,5)
    - QLi2(1,2,4,6)
    - QLi2(1,3,4,5)
    + QLi2(1,3,4,6)
    - QLi2(1,3,5,6)
    + QLi2(1,4,5,6)
    + QLi2(2,3,4,5)
    - QLi2(2,3,4,6)
    + QLi2(2,3,5,6)
    - QLi2(2,4,5,6)
  ;
  a *= 2;
  b *= 2;
  // std::cout << to_lyndon_basis(a + b);
  auto c =
    - shuffle_product_expr(QLi1(1,2,3,5), Log(1,2,3,5))
    + shuffle_product_expr(QLi1(1,2,3,6), Log(1,2,3,6))
    + shuffle_product_expr(QLi1(1,2,4,5), Log(1,2,4,5))
    - shuffle_product_expr(QLi1(1,2,4,6), Log(1,2,4,6))
    - shuffle_product_expr(QLi1(1,3,4,5), Log(1,3,4,5))
    + shuffle_product_expr(QLi1(1,3,4,6), Log(1,3,4,6))
    - shuffle_product_expr(QLi1(1,3,5,6), Log(1,3,5,6))
    + shuffle_product_expr(QLi1(1,4,5,6), Log(1,4,5,6))
    + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
    - shuffle_product_expr(QLi1(2,3,4,6), Log(2,3,4,6))
    + shuffle_product_expr(QLi1(2,3,5,6), Log(2,3,5,6))
    - shuffle_product_expr(QLi1(2,4,5,6), Log(2,4,5,6))
  ;
  c +=
    + shuffle_product_expr(QLi1(1,2,3,4), QLi1(1,4,5,6))
    - shuffle_product_expr(QLi1(1,2,5,6), QLi1(3,4,5,2))
    + shuffle_product_expr(QLi1(3,4,5,6), QLi1(1,2,3,6))
  ;
  std::cout << a + b;
  std::cout << c;
  std::cout << a + b - c;
}
