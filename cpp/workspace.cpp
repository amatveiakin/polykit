#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/enumerator.h"
#include "lib/expr_matrix_builder.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/gamma.h"
#include "lib/iterated_integral.h"
#include "lib/itertools.h"
#include "lib/lexicographical.h"
#include "lib/linalg.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/polylog_space.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/pvector.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/table_printer.h"
#include "lib/theta.h"
#include "lib/triangulation.h"
#include "lib/zip.h"


std::string GrL3_kernel(const XArgs& xargs) {
  const auto l1 = GrL1(xargs);
  const auto l2 = GrL2(xargs);
  const auto product_space = mapped(
    cartesian_product(l1, l2),
    [](const auto& elements) {
      const auto& [a, b] = elements;
      return ncoproduct(a, b);
    }
  );
  return polylog_space_kernel_describe(
    product_space,
    DISAMBIGUATE(identity_function),
    DISAMBIGUATE(ncomultiply)
  );
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
    .set_expression_line_limit(100)
    .set_annotation_sorting(AnnotationSorting::length)
    .set_compact_x(true)
  );


  // // const auto expr = GrQLiVec(2, {5}, {1,2,3,4});
  // const auto expr = GrQLi2(5)(1,2,3,4);
  // std::cout << expr;
  // std::cout << to_lyndon_basis(expr);

  // std::cout << dump_to_string(GrL2({1,2,3,4,5})) << "\n";

  for (const int num_points : range_incl(5, 8)) {
    const auto points = seq_incl(1, num_points);
    std::cout << "p=" << num_points << ":  ";
    std::cout << GrL3_kernel(points);
    std::cout << "  vs  ";
    std::cout << compute_polylog_space_dim(GrLBasic(3, points), DISAMBIGUATE(to_lyndon_basis));
    std::cout << "\n";
  }
  // p=5:  12 - 7 = 5  vs  5
  // p=6:  190 - 160 = 30  vs  30
  // p=7:  1210 - 1105 = 105  vs  105
  // p=8:  5000 - 4720 = 280  vs  280
}
