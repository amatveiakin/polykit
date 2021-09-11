#include <fstream>
#include <future>
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
#include "lib/iterated_integral.h"
#include "lib/itertools.h"
#include "lib/lexicographical.h"
#include "lib/linalg.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_space.h"
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
#include "lib/triangulation.h"
#include "lib/zip.h"


PolylogSpace CorrNondecreasing(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  CHECK(!args.empty() && args.back() == Inf) << dump_to_string(args);
  PolylogSpace ret;
  for (const auto& word : combinations_with_replacement(args, weight+1)) {
    ret.push_back(wrap_shared(Corr(word)));
  }
  return ret;
}


template<typename T>
T ptr_to_lyndon_basis(const std::shared_ptr<T>& ptr) {
  return to_lyndon_basis(*ptr);
}

template<typename SpaceT, typename PrepareF>
std::tuple<TripletVec, TripletVec, TripletVec> compute_polylog_space_triplets(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;

  ExprMatrixBuilder<ExprT> matrix_builder_a;
  add_polylog_space_to_matrix_builder(a, prepare, matrix_builder_a);
  TripletVec mat_a = matrix_builder_a.make_triplets();
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_a);
  TripletVec mat_united = matrix_builder_a.make_triplets();

  ExprMatrixBuilder<ExprT> matrix_builder_b;
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_b);
  TripletVec mat_b = matrix_builder_b.make_triplets();

  return {std::move(mat_a), std::move(mat_b), std::move(mat_united)};
}

auto polylog_space_triplets_l_vs_m(int weight, const XArgs& points) {
  return compute_polylog_space_triplets(
    L(weight, points),
    M(weight, points),
    DISAMBIGUATE(ptr_to_lyndon_basis)
  );
}

void save(const std::string& prefix, const std::string& suffix, const TripletVec& triplets) {
  save_triplets(absl::StrCat(prefix, suffix, ".triplets"), triplets);
}

void save_set(const std::string& prefix, const std::tuple<TripletVec, TripletVec, TripletVec>& triplets_set) {
  const auto [a, b, united] = triplets_set;
  save(prefix, "a", a);
  save(prefix, "b", b);
  save(prefix, "united", united);
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


  // const int weight = 2;
  // const int num_points = 5;
  // auto points = mapped(range_incl(1, num_points), [](int i) { return X(i); });
  // points.back() = Inf;
  // const auto [a, b, united] = polylog_space_triplets_l_vs_m(weight, points);
  // const std::string prefix = absl::StrCat("/mnt/c/Danya/results/l_vs_m/w", weight, "_p", num_points, "_");
  // save(prefix, "a", a);
  // save(prefix, "b", b);
  // save(prefix, "united", united);


  // const int weight = 2;
  // const int num_points = 5;
  // auto points = mapped(range_incl(1, num_points), [](int i) { return X(i); });
  // points.back() = Inf;
  // const std::string prefix = absl::StrCat("/mnt/c/Danya/results/l_vs_m/w", weight, "_p", num_points, "_");
  // // std::cout << "w=" << weight << ", p=" << num_points << ": " << polylog_spaces_describe(
  // //   CorrNondecreasing(weight, points),
  // //   N(weight, points),
  // //   DISAMBIGUATE(ptr_to_lyndon_basis)
  // // ) << "\n";
  // save_set(prefix, compute_polylog_space_triplets(
  //   CorrNondecreasing(weight, points),
  //   N(weight, points),
  //   DISAMBIGUATE(ptr_to_lyndon_basis)
  // ));


  const int num_points = 9;
  auto points = mapped(range_incl(1, num_points), [](int i) { return X(i); });
  points.back() = Inf;
  for (const int weight : range_incl(2, 6)) {
    std::cout << "w=" << weight << ", p=" << num_points << ": " << polylog_spaces_describe(
      L(weight, points),
      M(weight, points),
      DISAMBIGUATE(ptr_to_lyndon_basis)
    ) << "\n";
  }
}
