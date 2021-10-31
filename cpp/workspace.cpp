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


GrPolylogSpace test_space_dim_3_naive(const int weight, const std::vector<int>& points) {
  GrPolylogSpace space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CL(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  for (const auto& args : combinations(points, 6)) {
    for (const int shift : {0, 1, 2}) {
      space.push_back(CGrLi(weight, rotated_vector(args, shift)));
    }
  }
  return space;
}

GrPolylogSpace test_space_d3_w3(const std::vector<int>& points) {
  const int weight = 3;
  GrPolylogSpace space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CL(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  {
    const int fixed_points_idx = 0;
    const auto fixed_args = choose_indices(points, {fixed_points_idx});
    const auto var_args_pool = removed_index(points, fixed_points_idx);
    for (const auto& var_args : combinations(var_args_pool, 5)) {
      space.push_back(CGrLi(weight, concat(fixed_args, var_args)));
    }
  }
  return space;
}

GrPolylogSpace test_space_dim_4_naive(const int weight, const std::vector<int>& points) {
  GrPolylogSpace space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(test_space_dim_3_naive(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  for (const auto& args : combinations(points, 6)) {
    space.push_back(plucker_dual(QLiVec(weight, args), args));
  }
  for (const auto& args : combinations(points, 8)) {
    for (const int shift : range(args.size() / 2)) {
      space.push_back(CGrLi(weight, rotated_vector(args, shift)));
    }
  }
  return space;
}

GrPolylogNCoSpace test_co_space_dim_3(const int weight, int num_points) {
  static constexpr int dimension = 3;
  const auto points = to_vector(range_incl(1, num_points));
  return co_space(weight, 2, [&](const int weight) {
    return normalize_space_remove_consecutive([&]() {
      switch (weight) {
        // case 1: return GrFx(dimension, points);
        case 1: return GrL1(dimension, points);
        case 2: return GrL2(dimension, points);
        case 3: return test_space_d3_w3(points);
        case 4: return test_space_dim_3_naive(4, points);
        default: FATAL("Unsupported weight");
      }
    }(), dimension, num_points);
  });
}

// TODO: Generalize and factor out
GammaExpr gamma_substitute(const GammaExpr& expr, int from, int to) {
  from -= Gamma::kBitsetOffset;
  to -= Gamma::kBitsetOffset;
  return expr.mapped_expanding([&](const auto& term) {
    bool is_zero = false;
    const auto new_term = mapped(term, [&](const Gamma& g) {
      auto bitset = g.index_bitset();
      if (bitset.test(from)) {
        bitset.reset(from);
        if (bitset.test(to)) {
          is_zero = true;
          return Gamma{};
        } else {
          bitset.set(to);
          return Gamma(bitset);
        }
      } else {
        return g;
      }
    });
    return is_zero ? GammaExpr{} : GammaExpr::single(new_term);
  });
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


  // for (const int weight : range_incl(2, 6)) {
  //   for (const int num_points : range_incl(6, 8)) {
  //     const auto points = to_vector(range_incl(1, num_points));
  //     Profiler profiler;
  //     GrPolylogSpace space = test_space_dim_4(weight, points);
  //     profiler.finish("space");
  //     const int rank = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
  //     profiler.finish("rank");
  //     std::cout << "d=" << 4 << ", w=" << weight << ", p=" << num_points << ": ";
  //     std::cout << rank << "\n";
  //   }
  // }



  // const int weight = 3;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 10)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   const auto space = mapped(
  //     cartesian_product(
  //       normalize_space_remove_consecutive(GrL2(dimension, points)),
  //       normalize_space_remove_consecutive(GrFx(dimension, points))
  //     ),
  //     APPLY(DISAMBIGUATE(ncoproduct))
  //   );
  //   profiler.finish("space");
  //   const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   });
  //   profiler.finish("ranks");
  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << "\n";
  // }

  // const int weight = 5;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 10)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   // const auto space = test_space_d3_w3(points);
  //   const auto space = test_space_dim_3_naive(weight, num_points);
  //   profiler.finish("space");
  //   const auto ranks = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
  //   profiler.finish("ranks");
  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << " (" << space.size() << " funcs)\n";
  //   // std::cout << to_string(ranks) << "\n";
  // }

  // const int weight = 4;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 10)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   const auto space = test_co_space_dim_3(weight, num_points);
  //   profiler.finish("space");
  //   const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   });
  //   profiler.finish("ranks");
  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << "\n";
  // }

  // const int weight = 5;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 10)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   GrPolylogNCoSpace space_lyndon;
  //   const auto coords = combinations(points, dimension);
  //   for (const auto& word : get_lyndon_words(coords, weight)) {
  //     const auto term = mapped(word, convert_to<Gamma>);
  //     if (is_weakly_separated(term) && passes_normalize_remove_consecutive(term, dimension, num_points)) {
  //       space_lyndon.push_back(ncomultiply(GammaExpr::single(term)));
  //     }
  //   }
  //   const auto space_product = test_co_space_dim_3(weight, num_points);
  //   profiler.finish("space");
  //   const auto ranks = space_venn_ranks(space_lyndon, space_product, DISAMBIGUATE(identity_function));
  //   profiler.finish("ranks");
  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << "\n";
  // }



  // const int dimension = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // const auto expr = test_func_d4_p8_w3(points);
  // std::cout << expr;
  // std::cout << to_lyndon_basis(expr);
  // std::cout << is_totally_weakly_separated(expr) << "\n";
  // std::cout << to_string(space_venn_ranks(
  //   mapped(cartesian_product(GrL2(dimension, points), GrFx(dimension, points)), APPLY(DISAMBIGUATE(ncoproduct))),
  //   {ncomultiply(expr)},
  //   DISAMBIGUATE(identity_function)
  // )) << "\n";

//   const int num_points = 10;
//   const auto points = to_vector(range_incl(1, num_points));
//   const int dimension = num_points / 2;
//   // const int weight = dimension - 1;
//   const auto expr = test_func(points);
//   // std::cout << expr;
//   // std::cout << to_lyndon_basis(expr);
//   std::cout << "p=" << num_points << ": ";
//   std::cout << is_totally_weakly_separated(expr) << "\n";
//   // CHECK(expr == test_func_d4_p8_w3({1,2,3,4,5,6,7,8}));
//   std::cout << to_string(space_venn_ranks(
//     mapped(cartesian_product(GrL2(dimension, points), GrFx(dimension, points), GrFx(dimension, points)), [](const auto& exprs) {
//       const auto& [a, b, c] = exprs;
//       return abstract_coproduct_vec<GammaACoExpr>(std::vector{a, b, c});
//     }),
//     {expand_into_glued_pairs(expr)},
//     DISAMBIGUATE(identity_function)
//   ));
//   std::cout << "\n";

  // const int dimension = 3;
  // // const std::vector points = {1,2,2,4,5,6};
  // const std::vector points = {1,2,1,3,4,5};
  // std::vector unique_points = points;
  // keep_unique_sorted(unique_points);
  // const auto expr = CGrLiDim3(5, points);
  // CHECK_EQ(expr.dimension(), 3);
  // // std::cout << expr;
  // std::cout << to_lyndon_basis(expr);
  // std::cout << is_totally_weakly_separated(expr) << "\n";
  // const auto l2 = GrL2(dimension, unique_points);
  // const auto fx = GrFx(dimension, unique_points);
  // std::cout << to_string(space_venn_ranks(
  //   mapped(cartesian_product(l2, fx, fx, fx), [](const auto& exprs) {
  //     return std::apply(DISAMBIGUATE(abstract_coproduct<GammaACoExpr>), exprs);
  //   }),
  //   {expand_into_glued_pairs(expr)},
  //   DISAMBIGUATE(identity_function)
  // ));
  // std::cout << "\n";


  // TODO: Test:
  // const int weight = 4;
  // const auto space = test_co_space_dim_3(weight, 6);
  // std::cout << to_string(space_venn_ranks(
  //   space,
  //   {ncomultiply(normalize_remove_consecutive(test_func(weight, {1,2,3,4,5,6})))},
  //   DISAMBIGUATE(identity_function)
  // )) << "\n";


  // const int weight = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // auto space = test_space_dim_4_naive(weight, points);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // for (const int shift : range(points.size() / 2)) {
  //   space.push_back(CGrLi(weight, rotated_vector(points, shift)));
  // }
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // std::cout << is_totally_weakly_separated(gamma_substitute(CGrLi(4, {1,2,3,4,5,6,7,8}), 7,1)) << "\n";
  // std::cout << gamma_substitute(CGrLi(5, {1,2,3,4,5,6,7,8}), 4,3) << "\n";

  // const int weight = 4;
  // auto space = test_space_dim_4_naive(weight, {1,2,3,4,5,6,7,8});
  // std::cout << to_string(space_venn_ranks(
  //   space,
  //   // {CGrLi(weight, {1,1,3,4,5,6,7,8})},
  //   {gamma_substitute(CGrLi(weight, {1,2,3,4,5,6,7,8}), 7,1)},
  //   DISAMBIGUATE(to_lyndon_basis)
  // )) << "\n";

  // const auto expr = CGrLi(4, {1,1,3,4,5,6,7,8});
  // const auto expr_alt = gamma_substitute(CGrLi(4, {1,2,3,4,5,6,7,8}), 2, 1);
  // std::cout << (expr == expr_alt) << "\n";

  // const int weight = 5;
  // const std::vector points = {1,2,3,4,5,6,7,8,9};
  // std::cout << "w=" << weight << "\n";
  // auto space = test_space_dim_4_naive(weight, points);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  const int dimension = 4;
  const std::vector points = {1,2,3,4,5,6,7,8};
  std::cout << to_string(space_venn_ranks(
    mapped(cartesian_product(test_space_dim_4_naive(4, points), GrL1(dimension, points)), APPLY(DISAMBIGUATE(ncoproduct))),
    {ncomultiply(CGrLi(5, points))},
    DISAMBIGUATE(identity_function)
  )) << "\n";
}
