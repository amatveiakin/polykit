#include <iostream>
#include <regex>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/summation.h"
#include "lib/theta.h"


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;


  // ScopedFormatting sf(FormattingConfig()
  //   .set_formatter(Formatter::unicode)
  //   .set_expression_line_limit(FormattingConfig::kNoLineLimit)
  // );
  // for (int num_points = 4; num_points <= 8; num_points += 2) {
  //   for (int foreweight = 1; foreweight <= 3; ++foreweight) {
  //     std::stringstream ss;
  //     auto expr = LiQuad(foreweight, seq_incl(1, num_points));
  //     {
  //       ScopedFormatting sf(FormattingConfig().set_compact_expression(true));
  //       ss << expr.annotations();
  //     }
  //     ss << "=\n";
  //     ss << expr.main();
  //     std::cout << trimed_right(std::regex_replace(ss.str(), std::regex("\n"), " ")) << "\n";
  //   }
  //   std::cout << "\n";
  // }


  ScopedFormatting sf(FormattingConfig()
    .set_formatter(Formatter::unicode)
    .set_html_mode(true)
    .set_expression_line_limit(FormattingConfig::kNoLineLimit)
  );
  constexpr char kHeader[] = R"(
<style>
* {
  font-family: Cambria, serif;
  font-size: large;
}
table {
  border-spacing: 0 20px;
}
tr {
  vertical-align: top
}
.right {
  text-align: right
}
</style>
)";
  std::cout << kHeader;
  std::cout << "\n";
  std::cout << "<table>\n";
  for (int num_points = 4; num_points <= 8; num_points += 2) {
    std::cout << "<tr>\n";
    auto expr = LiQuad(1, seq_incl(1, num_points));
    {
      ScopedFormatting sf(FormattingConfig().set_compact_expression(true));
      std::cout << "<td class=\"right\">" << expr.annotations() << "</td>\n";
    }
    std::cout << "<td>&nbsp=&nbsp</td>\n";
    std::cout << "<td>\n";
    std::cout << expr.main();
    std::cout << "</td>\n";
    std::cout << "</tr>\n";
    std::cout << "\n";
  }
  std::cout << "</table>\n";


  // auto diff = -lhs + rhs;
  // profiler.finish("all");
  // std::cout << "\n";

  // std::cout << lhs << "\n";
  // std::cout << rhs << "\n";
  // std::cout << diff << "\n";


  // profiler.finish("expr");

  // auto lyndon = to_lyndon_basis(diff);
  // profiler.finish("lyndon");
  // std::cout << "\n";
  // std::cout << lyndon << "\n";
  // print_sorted_by_num_distinct_variables(std::cout, lyndon);

  // auto lyndon = to_lyndon_basis(project_on_x1(expr));
  // profiler.finish("lyndon");
  // std::cout << "\n";
  // print_sorted_by_num_distinct_elements(std::cout, lyndon);
}
