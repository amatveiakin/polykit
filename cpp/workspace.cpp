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


auto annotated_co_space_summands(int weight, int num_coparts, int dimension, int num_points) {
  CHECK_LE(num_coparts, weight);
  const auto points = to_vector(range_incl(1, num_points));
  const auto weights_per_summand = get_partitions(weight, num_coparts);
  const int max_atom_weight = max_value(flatten(weights_per_summand));
  const auto atom_spaces = mapped(range_incl(1, max_atom_weight), [&](const int w) {
    return mapped(GrL(w, dimension, points), [&](const auto& expr) {
      return to_lyndon_basis(normalize_remove_consecutive(expr));
    });
  });
  using ExprT = typename decltype(atom_spaces)::value_type::value_type;
  std::vector<std::pair<std::string, std::vector<NCoExprForExpr_t<ExprT>>>> ret;
  for (const auto& summand_weights : weights_per_summand) {
    const auto weights_with_counts = mapped(
      group_equal(summand_weights),
      [&](const auto& equal_weight_group) {
        return std::pair{equal_weight_group.front(), static_cast<int>(equal_weight_group.size())};
      }
    );
    const std::string annotation = str_join(
      weights_with_counts,
      fmt::tensor_prod(),
      [](const auto& weight_and_count) {
        const auto& [weight, count] = weight_and_count;
        return absl::StrCat(
          count > 1 ? fmt::super_num("Λ", {count}) : "",
          fmt::sub_num(fmt::mathcal("L"), {weight})
        );
      }
    );
    const auto summand_components = cartesian_combinations(
      // TODO: Introduce `mapped_apply`, use it here and in other places.
      mapped(weights_with_counts, [&](const auto& weight_and_count) {
        const auto& [weight, count] = weight_and_count;
        return std::pair{atom_spaces.at(weight - 1), count};
      })
    );
    ret.push_back({
      annotation,
      mapped(summand_components, DISAMBIGUATE(ncoproduct_vec))
    });
  }
  return ret;
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


  const int dimension = 3;
  const int weight = 3;
  for (const int num_points : range_incl(5, 10)) {
    for (const int num_coparts : range_incl(2, weight)) {
      Profiler profiler;
      const auto summands = annotated_co_space_summands(weight, num_coparts, dimension, num_points);
      profiler.finish("spaces");
      const bool compute_kernel = num_coparts < weight;
      std::vector<std::string> annotations;
      std::vector<int> space_ranks;
      GrPolylogNCoSpace united_space;
      for (const auto& [annotation, space] : summands) {
        annotations.push_back(annotation);
        space_ranks.push_back(space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
          return keep_non_weakly_separated(expr);
        }).kernel());
        if (compute_kernel) {
          append_vector(united_space, space);
        }
      }
      int kernel_rank = 0;
      profiler.finish("space ranks");
      if (compute_kernel) {
        kernel_rank = space_mapping_ranks(united_space, DISAMBIGUATE(identity_function), [](const auto& expr) {
          return std::make_tuple(keep_non_weakly_separated(expr), ncomultiply(expr));
        }).kernel();
        profiler.finish("kernel rank");
      }
      std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ", co=" << num_coparts << ": ";
      std::cout << str_join(annotations, " ⊕ ") << "\n";
      std::cout << sum(space_ranks) << "=" << str_join(space_ranks, "+");
      if (compute_kernel) {
        std::cout << " -> " << kernel_rank;
      }
      std::cout << "\n\n";
    }
  }
}
