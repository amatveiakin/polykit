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


  // This should be equal to the first column in corank table. TODO: Test.
  const int weight = 4;
  const int dimension = 3;
  for (const int num_points : range_incl(5, 9)) {
    const auto points = to_vector(range_incl(1, num_points));
    const auto coords = combinations(points, 3);

    // const auto space = mapped_expanding(
    //   get_lyndon_words(coords, weight),
    //   [](const auto& word) -> std::vector<GammaExpr> {
    //     const auto term = mapped(word, convert_to<Gamma>);
    //     if (is_weakly_separated(term)) {
    //       return {GammaExpr::single(term)};
    //     }
    //     return {};
    //   }
    // );
    // const auto ranks = space_mapping_ranks(
    //   space,
    //   DISAMBIGUATE(identity_function),
    //   [](const auto& expr) {
    //     return std::tuple{expand_into_glued_pairs(expr), keep_non_weakly_separated(expr)};
    //   }
    // );
    // std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
    // std::cout << to_string(ranks) << "\n";

    // TODO: Why is mapped_expanding SO SLOW? !!!

    Profiler profiler;
    // const auto lyndon_space = mapped_expanding(
    //   get_lyndon_words(coords, weight),
    //   [](const auto& word) -> std::vector<GammaACoExpr> {
    //     const auto term = mapped(word, convert_to<Gamma>);
    //     if (is_weakly_separated(term)) {
    //       return {expand_into_glued_pairs(GammaExpr::single(term))};
    //     }
    //     return {};
    //   }
    // );
    const auto lyndon_space = filtered(mapped_parallel(
      get_lyndon_words(coords, weight),
      [](const auto& word) {
        const auto term = mapped(word, convert_to<Gamma>);
        return is_weakly_separated(term)
          ? expand_into_glued_pairs(GammaExpr::single(term))
          : GammaACoExpr{};
      }
    ), [](const auto& expr) { return !expr.is_zero(); });
    // profiler.finish("space Lyndon");
    const auto l1 = mapped(GrL1(dimension, points), DISAMBIGUATE(to_lyndon_basis));
    // const auto l1 = normalize_space_remove_consecutive(mapped(GrFx(dimension, points), DISAMBIGUATE(to_lyndon_basis)));
    const auto l2 = mapped(GrL2(dimension, points), DISAMBIGUATE(to_lyndon_basis));
    const auto l2_times_l1_power = mapped_parallel(
      cartesian_product(l2, cartesian_power(l1, weight - 2)),
      [&](const auto& args) {
        const auto& [l2_arg, l1_args] = args;
        std::vector<GammaExpr> v = concat({l2_arg}, to_vector(l1_args));
        return abstract_coproduct_vec<GammaACoExpr>(v);
      }
    );
    // profiler.finish("space L2*L1^n");
    profiler.finish("spaces");
    const auto ranks = space_venn_ranks(lyndon_space, l2_times_l1_power, DISAMBIGUATE(identity_function));
    profiler.finish("ranks");
    std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
    std::cout << to_string(ranks) << "\n";
  }
}
