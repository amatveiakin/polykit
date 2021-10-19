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
#include "lib/integer_math.h"
#include "lib/iterated_integral.h"
#include "lib/itertools.h"
#include "lib/lexicographical.h"
#include "lib/linalg.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_cgrli.h"
#include "lib/polylog_grli.h"
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


GrPolylogSpace test_space_dim_3(const int weight, const std::vector<int>& points) {
  GrPolylogSpace space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_arg_pool = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CL(weight, main_arg_pool), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  for (const auto& args : combinations(points, 6)) {
    for (const int shift : {0, 1, 2}) {
      space.push_back(CGrLiDim3(weight, rotated_vector(args, shift)));
    }
  }
  return space;
}

GrPolylogSpace test_space_dim_4(const int weight, const std::vector<int>& points) {
  GrPolylogSpace space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(test_space_dim_3(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  for (const auto& args : combinations(points, 6)) {
    space.push_back(plucker_dual(QLiVec(weight, args), args));
  }
  return space;
}


int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_unicode_version(UnicodeVersion::simple)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_expression_line_limit(100)
    .set_annotation_sorting(AnnotationSorting::length)
    .set_compact_x(true)
  );


  // const int weight = 5;
  // const int num_points = 8;
  // const auto points = to_vector(range_incl(1, num_points));
  // Profiler profiler;
  // const auto space = mapped(CL(weight, points), DISAMBIGUATE(to_lyndon_basis));
  // profiler.finish("space");
  // for (const auto separator : {Delta(1,5), Delta(1,6), Delta(1,7)}) {
  //   const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [&](const auto& expr) {
  //     return expr.filtered([&](auto term) {
  //       CHECK(is_weakly_separated(term));
  //       term.push_back(separator);
  //       return !is_weakly_separated(term);
  //     });
  //   });
  //   profiler.finish("ranks");
  //   std::cout << "d=" << 2 << ", w=" << weight << ", p=" << num_points << ", ";
  //   std::cout << "sep=" << to_string(separator) << ": ";
  //   std::cout << to_string(ranks) << "\n";
  // }


  for (const int weight : range_incl(2, 6)) {
    for (const int num_points : range_incl(6, 8)) {
      const auto points = to_vector(range_incl(1, num_points));
      Profiler profiler;
      GrPolylogSpace space = test_space_dim_4(weight, points);
      profiler.finish("space");
      const int rank = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
      profiler.finish("rank");
      std::cout << "d=" << 4 << ", w=" << weight << ", p=" << num_points << ": ";
      std::cout << rank << "\n";
    }
  }
}
