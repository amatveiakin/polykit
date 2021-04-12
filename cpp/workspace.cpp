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


  const int w = 2;
  auto lhs =
    - corr_comultiply(CorrQLi   (4, {1,2,3,4,5,6}), {1,3})
  ;
  auto rhs =
    + corr_coproduct(CorrQLiNeg(1, {2,3}), CorrQLi(3, {1,2,3,4,5,6}))
    + corr_coproduct(CorrQLiNeg(1, {2,3,4,5}), CorrQLi(3, {1,2,5,6}))
    + corr_coproduct(CorrQLiNeg(1, {4,5}), CorrQLi(3, {1,2,3,4,5,6}))
    + corr_coproduct(CorrQLiNeg(3, {2,3,4,5}), CorrQLi(1, {1,2,5,6}))
    + corr_coproduct(CorrQLi   (1, {1,2}), CorrQLi(3, {1,2,3,4,5,6}))
    + corr_coproduct(CorrQLi   (1, {1,2,3,4}), CorrQLi(3, {1,4,5,6}))
    + corr_coproduct(CorrQLi   (1, {3,4}), CorrQLi(3, {1,2,3,4,5,6}))
    + corr_coproduct(CorrQLi   (1, {3,4,5,6}), CorrQLi(3, {1,2,3,6}))
    + corr_coproduct(CorrQLi   (1, {5,6}), CorrQLi(3, {1,2,3,4,5,6}))
    + corr_coproduct(CorrQLi   (3, {1,2,3,4}), CorrQLi(1, {1,4,5,6}))
    + corr_coproduct(CorrQLi   (3, {3,4,5,6}), CorrQLi(1, {1,2,3,6}))
    + corr_coproduct(CorrQLi   (3, {1,2,3,4,5,6}), CorrQLi(1, {1,6}))
  ;
  CHECK(lhs == rhs);
  std::cout << "\n\n";
  std::cout << lhs.annotations();
  std::cout << "=\n";
  std::cout << rhs.annotations();
  std::cout << "=\n";
  std::cout << lhs.main();

  std::cout << "\n\n";
  for (const auto& expr : {
    + corr_coproduct(CorrQLiNeg(1, {2,3}), CorrQLi(3, {1,2,3,4,5,6})),
    + corr_coproduct(CorrQLiNeg(1, {2,3,4,5}), CorrQLi(3, {1,2,5,6})),
    + corr_coproduct(CorrQLiNeg(1, {4,5}), CorrQLi(3, {1,2,3,4,5,6})),
    + corr_coproduct(CorrQLiNeg(3, {2,3,4,5}), CorrQLi(1, {1,2,5,6})),
    + corr_coproduct(CorrQLi   (1, {1,2}), CorrQLi(3, {1,2,3,4,5,6})),
    + corr_coproduct(CorrQLi   (1, {1,2,3,4}), CorrQLi(3, {1,4,5,6})),
    + corr_coproduct(CorrQLi   (1, {3,4}), CorrQLi(3, {1,2,3,4,5,6})),
    + corr_coproduct(CorrQLi   (1, {3,4,5,6}), CorrQLi(3, {1,2,3,6})),
    + corr_coproduct(CorrQLi   (1, {5,6}), CorrQLi(3, {1,2,3,4,5,6})),
    + corr_coproduct(CorrQLi   (3, {1,2,3,4}), CorrQLi(1, {1,4,5,6})),
    + corr_coproduct(CorrQLi   (3, {3,4,5,6}), CorrQLi(1, {1,2,3,6})),
    + corr_coproduct(CorrQLi   (3, {1,2,3,4,5,6}), CorrQLi(1, {1,6})),
  }) {
    std::cout << "(\n";
    std::cout << expr.main();
    std::cout << ")\n";
    std::cout << "+\n";
  }
}
