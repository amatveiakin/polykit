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


template<typename SpaceT>
void check_space_weight_eq(const SpaceT& space, int weight) {
  for (const auto& expr : space) {
    if (!expr.is_zero()) {
      CHECK_EQ(expr.weight(), weight);
    }
  }
}


// No dependencies: space rank == space.size()
GrPolylogSpace GrQLi3_test_space(const XArgs& xargs) {  // dimension = 3
  auto args = xargs.as_x();
  CHECK_EQ(6, args.size());
  GrPolylogSpace ret;
  for (int discarded_point : range(6)) {
    const auto combines_args = removed_index(args, discarded_point);
    for (const int shift : range(5)) {
      const auto rotated_args = rotated_vector(combines_args, shift);
      ret.push_back(GrQLi3(rotated_args[0])(slice(rotated_args, 1)));
    }
  }
  return ret;
}

// No dependencies: space rank == space.size()
GrPolylogSpace GrQLi4_test_space(const XArgs& xargs) {  // dimension = 3
  const int weight = 4;
  auto args = xargs.as_x();
  CHECK_EQ(7, args.size());
  GrPolylogSpace ret;
  for (const int bonus_point_idx : range(7)) {
    const X bonus_point = args[bonus_point_idx];
    const auto main_args_set = removed_index(args, bonus_point_idx);
    CHECK_EQ(6, main_args_set.size());
    ret.push_back(GrQLiVec(weight, {bonus_point}, main_args_set));
    for (const auto& main_args : combinations(main_args_set, 4)) {
      ret.push_back(GrQLiVec(weight, {bonus_point}, main_args));
    }
  }
  return ret;
}

GammaNCoExpr R_4_3(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 4);
  return sum_looped_vec([&](const std::vector<int>& args) {
    const auto get_args = [&](const std::vector<int>& indices) {
      // TODO: Change `sum` so that it doesn't requires nested `choose_indices_one_based`.
      return choose_indices_one_based(points, choose_indices_one_based(args, indices));
    };
    return ncoproduct(G(get_args({1,2,3})), G(get_args({1,2,4})), G(get_args({1,3,4})));
  }, 4, {1,2,3,4}, SumSign::alternating);
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


  // // const auto expr = GrQLiVec(2, {5}, {1,2,3,4});
  // const auto expr = GrQLi2(5)(1,2,3,4);
  // std::cout << expr;
  // std::cout << to_lyndon_basis(expr);

  // std::cout << are_weakly_separated(Gamma({2,3,5}), Gamma({1,4,5})) << "\n";

  // const auto expr = to_lyndon_basis(GrQLi3(5)(1,2,3,4));
  // std::cout << expr;
  // std::cout << keep_weakly_separated(expr);
  // std::cout << expr - keep_weakly_separated(expr);

  // std::cout << dump_to_string(GrL2({1,2,3,4,5})) << "\n";

  // for (const int num_points : range_incl(5, 8)) {
  //   const auto points = seq_incl(1, num_points);
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << compute_polylog_space_dim(GrL2(points), DISAMBIGUATE(to_lyndon_basis));
  //   std::cout << "\n";
  // }

  // const int num_points = 6;
  // const auto points = seq_incl(1, num_points);
  // std::cout << "p=" << num_points << ": ";
  // std::cout << compute_polylog_space_dim(GrQLi3_test_space(points), DISAMBIGUATE(to_lyndon_basis));
  // std::cout << "\n";

  // const int num_points = 7;
  // const auto points = seq_incl(1, num_points);
  // std::cout << "p=" << num_points << ": ";
  // std::cout << compute_polylog_space_dim(GrQLi4_test_space(points), DISAMBIGUATE(to_lyndon_basis));
  // std::cout << "\n";

  // const int dimension = 3;
  // const int weight = 3;
  // for (const int num_points : range_incl(5, 9)) {
  //   const auto points = seq_incl(1, num_points);
  //   const auto alphabet = combinations(points, dimension);
  //   GrPolylogNCoSpace s1;
  //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //     // const auto term = mapped(w, [](const auto& t) { return Gamma(t); });
  //     const auto term = mapped(w, convert_to<Gamma>);
  //     if (is_weakly_separated(term)) {
  //       s1.push_back(ncomultiply(GammaExpr::single(term)));
  //     }
  //   }
  //   GrPolylogNCoSpace s2 = mapped(
  //     cartesian_product(GrL1(points), GrL2(points)),
  //     [](const auto& elements) {
  //       const auto& [a, b] = elements;
  //       return ncoproduct(a, b);
  //     }
  //   );
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function)) << "\n";
  //   // Equals `compute_polylog_space_dim(GrLBasic(3, points), DISAMBIGUATE(to_lyndon_basis))`
  // }

  // const int dimension = 3;
  // const int weight = 4;
  // for (const int num_points : range_incl(5, 9)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto alphabet = combinations(points, dimension);
  //   GrPolylogNCoSpace s1;
  //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //     const auto term = mapped(w, convert_to<Gamma>);
  //     if (is_weakly_separated(term)) {
  //       s1.push_back(ncomultiply(GammaExpr::single(term)));
  //     }
  //   }
  //   const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, points));
  //   const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, points));
  //   const auto l3 = normalize_space_remove_consecutive(GrL3(dimension, points));
  //   const auto s2 = mapped(
  //     concat(
  //       cartesian_product(l3, l1),
  //       cartesian_product(l2, l2)
  //     ),
  //     APPLY(DISAMBIGUATE(ncoproduct))
  //   );
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function)) << "\n";
  // }

  // DeltaExpr expr = DeltaExpr::single({Delta(x1,Zero), Delta(x2,Zero), Delta(x3,Zero), Delta(x4,Zero)});
  // std::cout << comultiply(expr, {1,1,2});

  // TablePrinter t;
  // t.set_enable_alignment(false);
  // t.set_column_separator("\t");
  // for (const int dimension : range_incl(3, 5)) {
  //   for (const int num_points : range_incl(5, 11)) {
  //     const auto points = seq_incl(1, num_points);
  //     const int rank = compute_polylog_space_dim(GrL3(dimension, points), DISAMBIGUATE(to_lyndon_basis));
  //     std::cout << "d=" << dimension << ", p=" << num_points << ": ";
  //     std::cout << rank << "\n";
  //     t.set_content({num_points, dimension}, to_string(rank));
  //   }
  // }
  // std::cout << t.to_string();

  // for (const int dimension : range_incl(2, 5)) {
  //   for (const int num_points : range_incl(5, 8)) {
  //     const auto points = seq_incl(1, num_points);
  //     const auto a = GrL4_a_kernel(dimension, points);
  //     const auto b = GrL4_b_kernel(dimension, points);
  //     std::cout << "d=" << dimension << ", p=" << num_points << ":  ";
  //     std::cout << a << "  vs  " << b << "\n";
  //   }
  // }

  // const auto prepare = [](const GammaExpr& expr) {
  //   // return normalize_remove_consecutive(keep_weakly_separated(expr));
  //   return normalize_remove_consecutive(expr);
  // };
  // const int dimension = 3;
  // const int weight = 4;
  // for (const int num_points : range_incl(5, 9)) {
  //   Profiler profiler;
  //   const auto points = to_vector(range_incl(1, num_points));
  //   GrPolylogNCoSpace s1;
  //   const auto alphabet = combinations(points, dimension);
  //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //     const auto term = mapped(w, convert_to<Gamma>);
  //     s1.push_back(ncomultiply(prepare(GammaExpr::single(term))));
  //   }
  //   // for (const auto& alphabet_basis : combinations(points, num_points - 1)) {
  //   //   const auto alphabet = combinations(alphabet_basis, dimension);
  //   //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //   //     const auto term = mapped(w, convert_to<Gamma>);
  //   //     s1.push_back(ncomultiply(prepare(GammaExpr::single(term))));
  //   //   }
  //   // }
  //   const auto l1 = mapped(GrL1(dimension, points), prepare);
  //   const auto l2 = mapped(GrL2(dimension, points), prepare);
  //   const auto l3 = mapped(GrL3(dimension, points), prepare);
  //   const auto s2 = mapped(
  //     concat(
  //       cartesian_product(l3, l1),
  //       cartesian_product(l2, l2)
  //     ),
  //     APPLY(DISAMBIGUATE(ncoproduct))
  //   );
  //   profiler.finish("spaces");
  //   const auto description = polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function));
  //   profiler.finish("ranks");
  //   std::cout << "p=" << num_points << ": " << description << "\n";
  // }

  // const int dimension = 3;
  // const int num_points = 7;
  // const auto points = seq_incl(1, num_points);
  // const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, points));
  // const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, points));
  // for (const int num_l1 : range(0, 10)) {
  //   Profiler profiler;
  //   const auto space = mapped(
  //     cartesian_product(l2, combinations(seq_incl(0, l1.size() - 1), num_l1)),
  //     [&](const auto& args) {
  //       const auto& [l2_arg, l1_indices] = args;
  //       std::vector<GammaExpr> v = concat({l2_arg}, choose_indices(l1, l1_indices));
  //       return ncoproduct(v);
  //     }
  //   );
  //   profiler.finish("spaces");
  //   const auto rank = compute_polylog_space_dim(space, DISAMBIGUATE(ncomultiply));
  //   profiler.finish("rank");
  //   std::cout << "l2*l1^" << num_l1 << ": " << rank << "\n";
  // }

  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 9)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto alphabet = combinations(points, dimension);
  //   const auto l1 = GrL1(dimension, points);
  //   const auto l2 = GrL2(dimension, points);
  //   const auto space = mapped(
  //     cartesian_product(l2, l1, l1),
  //     APPLY(DISAMBIGUATE(ncoproduct))
  //   );
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << polylog_space_kernel_describe(space, DISAMBIGUATE(identity_function), DISAMBIGUATE(ncomultiply)) << "\n";
  // }

  // // K_n rank
  // const int weight = 3;
  // for (const int dimension : range_incl(4, 5)) {
  //   for (const int num_points : range_incl(5, 9)) {
  //     const auto points = to_vector(range_incl(1, num_points));
  //     const auto l1 = GrL1(dimension, points);
  //     const auto l2 = GrL2(dimension, points);
  //     const int l1_rank = compute_polylog_space_dim(l1, DISAMBIGUATE(to_lyndon_basis));
  //     const int space_a_rank = binomial(l1_rank, weight);
  //     const auto space_b = mapped(
  //       cartesian_product(l2, l1),
  //       APPLY(DISAMBIGUATE(ncoproduct))
  //     );
  //     check_space_weight_eq(space_b, weight);
  //     const int space_b_image_rank = compute_polylog_space_dim(space_b, DISAMBIGUATE(ncomultiply));
  //     const int diff = space_a_rank - space_b_image_rank;
  //     std::cout << "d=" << dimension << ", p=" << num_points << ": ";
  //     std::cout << space_a_rank << " - " << space_b_image_rank << " = " << diff << "\n";
  //   }
  // }

  // GrPolylogSpace space;
  // const int num_points = 7;
  // const int weight = 4;
  // for (const int shift : range(num_points)) {
  //   const auto args = mapped(range(num_points), [&](const int p) { return (p + shift) % num_points + 1; });
  //   const int num_bonus_args = 1;
  //   const auto bonus_args = slice(args, 0, num_bonus_args);
  //   const auto main_args = slice(args, num_bonus_args);
  //   space.push_back(GrQLiVec(weight, bonus_args, main_args));
  // }
  // const auto args_pool = to_vector(range_incl(1, num_points));
  // for (const int bonus_arg_idx : range(num_points)) {
  //   const auto bonus_args = std::vector{args_pool[bonus_arg_idx]};
  //   const auto main_args_pool = removed_index(args_pool, bonus_arg_idx);
  //   // for (const int main_args_start : range(num_points - 1)) {
  //   //   const auto main_args = slice(rotated_vector(main_args_pool, main_args_start), 0, 4);
  //   //   space.push_back(GrQLiVec(weight, bonus_args, main_args));
  //   // }
  //   for (const auto& main_args : combinations(main_args_pool, 4)) {
  //     space.push_back(GrQLiVec(weight, bonus_args, main_args));
  //   }
  // }
  // std::cout << dump_to_string(space) << "\n";
  // std::cout << compute_polylog_space_dim(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // const int num_points = 5;
  // const auto points = to_vector(range_incl(1, num_points));
  // const auto func = sum_looped_vec(R_4_3, 5, {1,2,3,4});
  // GrPolylogNCoSpace func_space{func};
  // // std::cout << func;
  // GrPolylogSpace fx;
  // for (const auto& p : combinations(points, 3)) {
  //   fx.push_back(G(p));
  // }
  // GrPolylogSpace l2 = GrL2(3, points);
  // GrPolylogNCoSpace space;
  // for (const auto& [a, b] : cartesian_product(l2, fx)) {
  //   const auto expr = ncoproduct(a, b);
  //   if (is_totally_weakly_separated(expr)) {
  //     // space.push_back(expr);
  //     space.push_back(ncomultiply(expr));
  //   }
  // };
  // // std::cout << space.front();
  // std::cout << polylog_spaces_intersection_describe(space, func_space, DISAMBIGUATE(identity_function)) << "\n";
  // // std::cout << polylog_space_contains(space, func_space, DISAMBIGUATE(identity_function)) << "\n";

}
