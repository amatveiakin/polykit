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


GammaExpr SymmCGrLi3(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  constexpr int weight = 3;
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  auto expr =
    + CGrLi(weight, args({1,2,3,4,5,6}))
  ;
  expr +=
    - GrQLiVec(weight, args({2}), args({3,4,5,6}))
    + GrQLiVec(weight, args({5}), args({1,2,3,6}))
    + GrQLiVec(weight, args({2}), args({1,3,4,5}))
    - GrQLiVec(weight, args({5}), args({1,2,4,6}))
    - GrQLiVec(weight, args({2}), args({1,3,4,6}))
    + GrQLiVec(weight, args({5}), args({1,3,4,6}))
    + GrQLiVec(weight, args({3}), args({1,2,4,6}))
    - GrQLiVec(weight, args({6}), args({1,3,4,5}))
    + GrQLiVec(weight, args({1}), args({3,4,5,6}))
    - GrQLiVec(weight, args({4}), args({1,2,3,6}))
  ;
  // TODO: Why is this not equivalent? !!!
  // expr += sum_looped_vec([](const std::vector<int>& points) {
  //   const auto args = [&](const std::vector<int>& indices) {
  //     return choose_indices_one_based(points, indices);
  //   };
  //   return
  //     - GrQLiVec(weight, args({1}), args({2,3,4,6}))
  //     // + GrQLiVec(weight, args({1}), args({2,4,5,6}))
  //   ;
  // }, 6, {1,2,3,4,5,6}, SumSign::alternating);
  // return expr;
  expr -=
    - GrQLiVec(weight, args({1}), args({2,3,4,6}))
    + GrQLiVec(weight, args({2}), args({1,3,4,5}))
    - GrQLiVec(weight, args({3}), args({2,4,5,6}))
    + GrQLiVec(weight, args({4}), args({1,3,5,6}))
    - GrQLiVec(weight, args({5}), args({1,2,4,6}))
    + GrQLiVec(weight, args({6}), args({1,2,3,5}))
  ;
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("SymmCGrLi"), {weight}),
      points
    )
  );
}

// TODO: Finish symmetrizing
GammaExpr SymmCGrLi4(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  constexpr int weight = 4;
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  auto expr =
    + CGrLi(weight, args({1,2,3,4,5,6}))
    - CGrLi(weight, args({2,3,4,5,6,1}))
    + CGrLi(weight, args({3,4,5,6,1,2}))
    - CGrLi(weight, args({4,5,6,1,2,3}))
    + CGrLi(weight, args({5,6,1,2,3,4}))
    - CGrLi(weight, args({6,1,2,3,4,5}))
  ;
  expr -=
    + CGrLi(weight, args({1,6,5,4,3,2}))
    - CGrLi(weight, args({6,5,4,3,2,1}))
    + CGrLi(weight, args({5,4,3,2,1,6}))
    - CGrLi(weight, args({4,3,2,1,6,5}))
    + CGrLi(weight, args({3,2,1,6,5,4}))
    - CGrLi(weight, args({2,1,6,5,4,3}))
  ;
  // return expr;
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("SymmCGrLi"), {weight}),
      points
    )
  );
}

GrPolylogSpace GrL_Dim3_Weight4_test() {
  const int weight = 4;
  const int dimension = 3;
  const std::vector points = {1,2,3,4,5,6,7};
  GrPolylogSpace ret = GrL_core(weight, dimension, points, false, 0);
  // for (const auto& [bonus_p, qli_points] : index_splits(points, dimension - 2)) {
  //   const auto& bonus_points = bonus_p;  // workaround: lambdas cannot capture structured bindings
  //   append_vector(
  //     ret,
  //     mapped(combinations(qli_points, 6), [&](auto p) {
  //       return GrQLiVec(weight, bonus_points, p);
  //     })
  //   );
  // }
  return ret;
}

GammaNCoExpr C33(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 4);
  const auto pl = [&](int p1, int p2, int p3) {
    return G(choose_indices_one_based(points, {p1,p2,p3}));
  };
  return
    + ncoproduct(pl(1,2,3), pl(1,2,4), pl(1,3,4))
    - ncoproduct(pl(2,3,4), pl(2,3,1), pl(2,4,1))
    + ncoproduct(pl(3,4,1), pl(3,4,2), pl(3,1,2))
    - ncoproduct(pl(4,1,2), pl(4,1,3), pl(4,2,3))
  ;
}

GammaNCoExpr C335(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 5);
  const auto c = [&](int p1, int p2, int p3, int p4) {
    return C33(choose_indices_one_based(points, {p1,p2,p3,p4}));
  };
  return
    + c(1,2,3,4)
    + c(2,3,4,5)
    + c(3,4,5,1)
    + c(4,5,1,2)
    + c(5,1,2,3)
  ;
}

// TODO: rename
GammaExpr arrow_left(const GammaExpr& expr, int num_dst_points) {
  GammaExpr ret;
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  for (const int i : range(num_dst_points)) {
    const auto dst_points = removed_index(all_dst_points, i);
    ret += neg_one_pow(i) * substitute_variables(expr, dst_points);
  }
  return ret;
}

// TODO: rename
GammaExpr arrow_up(const GammaExpr& expr, int num_dst_points) {
  GammaExpr ret;
  const auto all_dst_points = to_vector(range_incl(1, num_dst_points));
  for (const int i : range(num_dst_points)) {
    const auto [removed_points, dst_points] = split_indices(all_dst_points, {i});
    ret += neg_one_pow(i) * pullback(substitute_variables(expr, dst_points), removed_points);
  }
  return ret;
}

GammaExpr symmetrize_double(const GammaExpr& expr, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  const int sign = neg_one_pow(num_points);
  return expr + sign * substitute_variables(expr, rotated_vector(points, 1));
}

GammaExpr symmetrize_loop(const GammaExpr& expr, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  GammaExpr ret;
  for (const int i : range(num_points)) {
    const int sign = num_points % 2 == 1 ? 1 : neg_one_pow(i);
    ret += sign * substitute_variables(expr, rotated_vector(points, i));
  }
  return ret;
}

// TODO: Test:  arrow_left(arrow_left(x)) == 0
// TODO: Test:  arrow_up(arrow_up(x)) == 0
// TODO: Test:  arrow_left(arrow_up(x)) + arrow_up(arrow_left(x) == 0
// TODO: Test:  symmetrize_double(symmetrize_loop(x)) == 0


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
    .set_expression_line_limit(300)
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
  //     applied(DISAMBIGUATE(ncoproduct))
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
  //   // const auto space = CGrL3_Dim3_test_space(points);
  //   const auto space = CGrL_Dim3_naive_test_space(weight, num_points);
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
  //   const auto space = simple_co_CGrL_test_space(weight, dimension, num_points);
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
  //   const auto space_product = simple_co_CGrL_test_space(weight, dimension, num_points);
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
  //   mapped(cartesian_product(GrL2(dimension, points), GrFx(dimension, points)), applied(DISAMBIGUATE(ncoproduct))),
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
  // const auto space = simple_co_CGrL_test_space(weight, 3, 6);
  // std::cout << to_string(space_venn_ranks(
  //   space,
  //   {ncomultiply(normalize_remove_consecutive(test_func(weight, {1,2,3,4,5,6})))},
  //   DISAMBIGUATE(identity_function)
  // )) << "\n";


  // const int weight = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // auto space = CGrL_Dim4_naive_test_space(weight, points);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // for (const int shift : range(points.size() / 2)) {
  //   space.push_back(CGrLi(weight, rotated_vector(points, shift)));
  // }
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // std::cout << is_totally_weakly_separated(substitute_variables(CGrLi(4, {1,2,3,4,5,6,7,8}), {1,2,3,4,5,6,1,8})) << "\n";
  // std::cout << substitute_variables(CGrLi(5, {1,2,3,4,5,6,7,8}), {1,2,3,3,5,6,7,8}) << "\n";

  // const int weight = 4;
  // auto space = CGrL_Dim4_naive_test_space(weight, {1,2,3,4,5,6,7,8});
  // std::cout << to_string(space_venn_ranks(
  //   space,
  //   // {CGrLi(weight, {1,1,3,4,5,6,7,8})},
  //   {substitute_variables(CGrLi(weight, {1,2,3,4,5,6,7,8}), {1,2,3,4,5,6,1,8})},
  //   DISAMBIGUATE(to_lyndon_basis)
  // )) << "\n";

  // const auto expr = CGrLi(4, {1,1,3,4,5,6,7,8});
  // const auto expr_alt = substitute_variables(CGrLi(4, {1,2,3,4,5,6,7,8}), {1,1,3,4,5,6,7,8});
  // std::cout << (expr == expr_alt) << "\n";

  // const int weight = 5;
  // const std::vector points = {1,2,3,4,5,6,7,8,9};
  // std::cout << "w=" << weight << "\n";
  // auto space = CGrL_Dim4_naive_test_space(weight, points);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // const int dimension = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // std::cout << to_string(space_venn_ranks(
  //   mapped(cartesian_product(CGrL_Dim4_naive_test_space(4, points), GrL1(dimension, points)), applied(DISAMBIGUATE(ncoproduct))),
  //   {ncomultiply(CGrLi(5, points))},
  //   DISAMBIGUATE(identity_function)
  // )) << "\n";



#if 0
  const auto prepare = [](const GammaExpr& expr) {
    // return expr.filtered([](const auto& term) {
    //   return absl::c_all_of(term, [](const Gamma& g) {
    //     return g.index_bitset().test(1 - Gamma::kBitsetOffset);
    //   });
    // });
    // return to_lyndon_basis(expr.filtered([](const auto& term) {
    //   Gamma::BitsetT common_vars;
    //   Gamma::BitsetT all_vars;
    //   common_vars.set();
    //   for (const Gamma& g : term) {
    //     all_vars |= g.index_bitset();
    //     common_vars &= g.index_bitset();
    //   }
    //   Gamma::BitsetT expected_common_vars = vector_to_bitset_or<Gamma::BitsetT>({1}, Gamma::kBitsetOffset).value();
    //   Gamma::BitsetT expected_vars = vector_to_bitset_or<Gamma::BitsetT>({1,2,4,5,6}, Gamma::kBitsetOffset).value();
    //   return common_vars == expected_common_vars && all_vars == expected_vars;
    // }));
    return to_lyndon_basis(expr.filtered([](const auto& term) {
      Gamma::BitsetT common_vars;
      Gamma::BitsetT all_vars;
      common_vars.set();
      for (const Gamma& g : term) {
        all_vars |= g.index_bitset();
        common_vars &= g.index_bitset();
      }
      return bitset_to_vector(common_vars).size() == 1 && bitset_to_vector(all_vars).size() == 5;
    }));
  };

  const auto to_grqli_func_expr = [](const auto& term) -> StringExpr {
    Gamma::BitsetT common_vars;
    Gamma::BitsetT all_vars;
    common_vars.set();
    for (const Gamma& g : term) {
      all_vars |= g.index_bitset();
      common_vars &= g.index_bitset();
    }
    const auto common_vars_vector = bitset_to_vector(common_vars, Gamma::kBitsetOffset);
    const auto all_vars_vector = bitset_to_vector(all_vars, Gamma::kBitsetOffset);
    if (common_vars_vector.size() == 1 && all_vars_vector.size() == 5) {
      return StringExpr::single(absl::StrCat(
        "GrQLi3(",
        common_vars_vector.front(),
        ")(",
        str_join(set_difference(all_vars_vector, common_vars_vector), ","),
        ")"
      ));
    } else {
      return {};
    }
  };
#endif

  // auto expr = (
  //   + CGrLi(3, {1,2,3,4,5,6})
  //   - CGrLi(3, {1,2,3,4,5,7})
  //   + CGrLi(3, {1,2,3,4,6,7})
  //   - CGrLi(3, {1,2,3,5,6,7})
  //   + CGrLi(3, {1,2,4,5,6,7})
  //   - CGrLi(3, {1,3,4,5,6,7})
  //   + CGrLi(3, {2,3,4,5,6,7})
  // );

  // std::cout << prepare(expr);
  // std::cout << prepare(GrQLi3(1)(2,4,5,6));
  // std::cout << prepare(expr - GrQLi3(1)(2,4,5,6));
  // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);

  // expr += (
  //   - GrQLi3(1)(2,4,5,6)
  //   + GrQLi3(1)(2,4,5,7)
  //   - GrQLi3(1)(2,4,6,7)
  //   + GrQLi3(1)(3,4,5,6)
  //   - GrQLi3(1)(3,4,5,7)
  //   + GrQLi3(1)(3,4,6,7)
  //   + GrQLi3(2)(1,4,5,6)
  //   - GrQLi3(2)(1,4,5,7)
  //   + GrQLi3(2)(1,4,6,7)
  //   - GrQLi3(2)(3,4,5,6)
  //   + GrQLi3(2)(3,4,5,7)
  //   - GrQLi3(2)(3,4,6,7)
  //   - GrQLi3(3)(1,4,5,6)
  //   + GrQLi3(3)(1,4,5,7)
  //   - GrQLi3(3)(1,4,6,7)
  //   + GrQLi3(3)(2,4,5,6)
  //   - GrQLi3(3)(2,4,5,7)
  //   + GrQLi3(3)(2,4,6,7)
  // );
  // // std::cout << prepare(expr);
  // std::cout << to_lyndon_basis(expr);

  // // std::cout << to_string(space_venn_ranks(
  // //   GrL_core(3, 3, {1,2,3,4,5,6,7}, false, 0),
  // //   {expr},
  // //   DISAMBIGUATE(to_lyndon_basis)
  // // )) << "\n";


  // const auto lhs =
  //   +6*CGrLi(3, {1,2,3,4,5,6})
  // ;
  // const auto rhs =
  //   + CGrLi(3, {1,2,3,4,5,6})
  //   - CGrLi(3, {2,3,4,5,6,1})
  //   + CGrLi(3, {3,4,5,6,1,2})
  //   - CGrLi(3, {4,5,6,1,2,3})
  //   + CGrLi(3, {5,6,1,2,3,4})
  //   - CGrLi(3, {6,1,2,3,4,5})
  // ;
  // auto expr = lhs - rhs;
  // // std::cout << prepare(expr);
  // // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);
  // expr +=
  //   -2 * GrQLi3(1)(2,3,4,5)
  //   -4 * GrQLi3(2)(3,4,5,6)
  //   -2 * GrQLi3(3)(1,4,5,6)
  //   +2 * GrQLi3(4)(1,2,5,6)
  //   +4 * GrQLi3(5)(1,2,3,6)
  //   +2 * GrQLi3(6)(1,2,3,4)

  //   +2 * GrQLi3(1)(2,3,4,6)
  //   +4 * GrQLi3(2)(1,3,4,5)
  //   +2 * GrQLi3(3)(2,4,5,6)
  //   -2 * GrQLi3(4)(1,3,5,6)
  //   -4 * GrQLi3(5)(1,2,4,6)
  //   -2 * GrQLi3(6)(1,2,3,5)

  //   -2 * GrQLi3(1)(2,3,5,6)
  //   -4 * GrQLi3(2)(1,3,4,6)
  //   -2 * GrQLi3(3)(1,2,4,5)
  //   +2 * GrQLi3(4)(2,3,5,6)
  //   +4 * GrQLi3(5)(1,3,4,6)
  //   +2 * GrQLi3(6)(1,2,4,5)

  //   -2 * GrQLi3(1)(2,4,5,6)
  //   +2 * GrQLi3(2)(1,3,5,6)
  //   +4 * GrQLi3(3)(1,2,4,6)
  //   +2 * GrQLi3(4)(1,2,3,5)
  //   -2 * GrQLi3(5)(2,3,4,6)
  //   -4 * GrQLi3(6)(1,3,4,5)

  //   +4 * GrQLi3(1)(3,4,5,6)
  //   +2 * GrQLi3(2)(1,4,5,6)
  //   -2 * GrQLi3(3)(1,2,5,6)
  //   -4 * GrQLi3(4)(1,2,3,6)
  //   -2 * GrQLi3(5)(1,2,3,4)
  //   +2 * GrQLi3(6)(2,3,4,5)
  // ;
  // std::cout << prepare(expr);

  // std::cout << to_lyndon_basis(
  //   + SymmCGrLi3({1,2,3,4,5,6})
  //   + SymmCGrLi3({2,3,4,5,6,1})
  // );


  // auto expr =
  //   + CGrLi(3, {1,2,3,4,5,6})
  //   + CGrLi(3, {2,3,4,5,6,1})
  // ;
  // expr +=
  //   - GrQLi3(1)(2,4,5,6)
  //   + GrQLi3(1)(3,4,5,6)
  //   + GrQLi3(2)(1,3,4,5)
  //   - GrQLi3(2)(1,3,4,6)
  //   + GrQLi3(2)(1,4,5,6)
  //   - GrQLi3(2)(3,4,5,6)
  //   - GrQLi3(3)(1,2,4,5)
  //   + GrQLi3(3)(1,2,4,6)
  //   - GrQLi3(3)(1,4,5,6)
  //   + GrQLi3(3)(2,4,5,6)
  //   + GrQLi3(4)(1,2,3,5)
  //   - GrQLi3(4)(1,2,3,6)
  //   - GrQLi3(5)(1,2,3,4)
  //   + GrQLi3(5)(1,2,3,6)
  //   - GrQLi3(5)(1,2,4,6)
  //   + GrQLi3(5)(1,3,4,6)
  //   + GrQLi3(6)(1,2,3,4)
  //   - GrQLi3(6)(1,2,3,5)
  //   + GrQLi3(6)(1,2,4,5)
  //   - GrQLi3(6)(1,3,4,5)
  // ;
  // // std::cout << prepare(expr);
  // std::cout << to_lyndon_basis(expr);


  // auto expr = (
  //   + SymmCGrLi3({1,2,3,4,5,6})
  //   - SymmCGrLi3({1,2,3,4,5,7})
  //   + SymmCGrLi3({1,2,3,4,6,7})
  //   - SymmCGrLi3({1,2,3,5,6,7})
  //   + SymmCGrLi3({1,2,4,5,6,7})
  //   - SymmCGrLi3({1,3,4,5,6,7})
  //   + SymmCGrLi3({2,3,4,5,6,7})
  // );
  // expr +=
  //   - GrQLi3(1)(2,3,4,6)
  //   + GrQLi3(1)(2,3,5,7)
  //   - GrQLi3(1)(2,4,5,6)
  //   - GrQLi3(1)(2,4,6,7)
  //   + GrQLi3(1)(3,4,5,7)
  //   + GrQLi3(1)(3,5,6,7)
  //   + GrQLi3(2)(1,3,4,6)
  //   - GrQLi3(2)(1,3,5,7)
  //   + GrQLi3(2)(1,4,5,6)
  //   + GrQLi3(2)(1,4,6,7)
  //   - GrQLi3(2)(3,4,5,7)
  //   - GrQLi3(2)(3,5,6,7)
  //   - GrQLi3(3)(1,2,4,6)
  //   + GrQLi3(3)(1,2,5,7)
  //   - GrQLi3(3)(1,4,5,6)
  //   - GrQLi3(3)(1,4,6,7)
  //   + GrQLi3(3)(2,4,5,7)
  //   + GrQLi3(3)(2,5,6,7)
  //   + GrQLi3(4)(1,2,3,6)
  //   - GrQLi3(4)(1,2,5,7)
  //   + GrQLi3(4)(1,3,5,6)
  //   + GrQLi3(4)(1,3,6,7)
  //   - GrQLi3(4)(2,3,5,7)
  //   - GrQLi3(4)(2,5,6,7)
  //   - GrQLi3(5)(1,2,3,6)
  //   + GrQLi3(5)(1,2,4,7)
  //   - GrQLi3(5)(1,3,4,6)
  //   - GrQLi3(5)(1,3,6,7)
  //   + GrQLi3(5)(2,3,4,7)
  //   + GrQLi3(5)(2,4,6,7)
  //   + GrQLi3(6)(1,2,3,5)
  //   - GrQLi3(6)(1,2,4,7)
  //   + GrQLi3(6)(1,3,4,5)
  //   + GrQLi3(6)(1,3,5,7)
  //   - GrQLi3(6)(2,3,4,7)
  //   - GrQLi3(6)(2,4,5,7)
  //   - GrQLi3(7)(1,2,3,5)
  //   + GrQLi3(7)(1,2,4,6)
  //   - GrQLi3(7)(1,3,4,5)
  //   - GrQLi3(7)(1,3,5,6)
  //   + GrQLi3(7)(2,3,4,6)
  //   + GrQLi3(7)(2,4,5,6)
  // ;
  // // std::cout << prepare(expr);
  // std::cout << to_lyndon_basis(expr);  // ZERO
  // // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);



  // auto expr =
  //   + SymmCGrLi4({1,2,3,4,5,6})
  //   + SymmCGrLi4({2,3,4,5,6,7})
  //   + SymmCGrLi4({3,4,5,6,7,1})
  //   + SymmCGrLi4({4,5,6,7,1,2})
  //   + SymmCGrLi4({5,6,7,1,2,3})
  //   + SymmCGrLi4({6,7,1,2,3,4})
  //   + SymmCGrLi4({7,1,2,3,4,5})
  // ;
  // expr +=
  //   -4*GrQLi4(1)(2,3,4,5,6,7)
  //   -4*GrQLi4(2)(3,4,5,6,7,1)
  //   -4*GrQLi4(3)(4,5,6,7,1,2)
  //   -4*GrQLi4(4)(5,6,7,1,2,3)
  //   -4*GrQLi4(5)(6,7,1,2,3,4)
  //   -4*GrQLi4(6)(7,1,2,3,4,5)
  //   -4*GrQLi4(7)(1,2,3,4,5,6)
  // ;
  // std::cout << to_string(space_venn_ranks(
  //   GrL_Dim3_Weight4_test(),
  //   {expr},
  //   DISAMBIGUATE(to_lyndon_basis)
  // )) << "\n";
  // // std::cout << prepare(expr);
  // // std::cout << to_lyndon_basis(expr);
  // // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5};
  // std::cout << to_string(space_venn_ranks(
  //   mapped_expanding(
  //     cartesian_product(GrL2(dimension, points), GrFx(dimension, points)),
  //     [](const auto& p) -> std::vector<GammaNCoExpr> {
  //       auto ret = ncomultiply(std::apply(DISAMBIGUATE(ncoproduct), p));
  //       if (is_totally_weakly_separated(ret)) {
  //         const auto& [a, b] = p;
  //         std::cout << annotations_one_liner(a.annotations()) << " * " << annotations_one_liner(b.annotations()) << "\n";
  //         return {ret};
  //       } else {
  //         return {};
  //       }
  //     }
  //   ),
  //   {C335(points)},
  //   DISAMBIGUATE(to_lyndon_basis)
  // )) << "\n";



  // std::cout << to_string(space_mapping_ranks(
  //   CGrL3_Dim3_test_space({1,2,3,4,5,6}),
  //   // CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{
  //       // to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,1})),
  //       // to_lyndon_basis(expr - substitute_variables(expr, {1,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(expr, {1,2,3,4,5,6})
  //         - substitute_variables(expr, {1,2,3,4,5,7})
  //         + substitute_variables(expr, {1,2,3,4,6,7})
  //         - substitute_variables(expr, {1,2,3,5,6,7})
  //         + substitute_variables(expr, {1,2,4,5,6,7})
  //         - substitute_variables(expr, {1,3,4,5,6,7})
  //         + substitute_variables(expr, {2,3,4,5,6,7})
  //       ),
  //       to_lyndon_basis(
  //         + pullback(substitute_variables(expr, {1,2,3,4,5,6}), {7})
  //         - pullback(substitute_variables(expr, {1,2,3,4,5,7}), {6})
  //         + pullback(substitute_variables(expr, {1,2,3,4,6,7}), {5})
  //         - pullback(substitute_variables(expr, {1,2,3,5,6,7}), {4})
  //         + pullback(substitute_variables(expr, {1,2,4,5,6,7}), {3})
  //         - pullback(substitute_variables(expr, {1,3,4,5,6,7}), {2})
  //         + pullback(substitute_variables(expr, {2,3,4,5,6,7}), {1})
  //       ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CGrL3_Dim3_test_space({1,2,3,4,5}),
  //   // CGrL_Dim3_naive_test_space(4, {1,2,3,4,5}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     // const auto subsum =
  //     //   + substitute_variables(expr, {1,2,3,4,5})
  //     //   + substitute_variables(expr, {2,3,4,5,1})
  //     //   + substitute_variables(expr, {3,4,5,1,2})
  //     //   + substitute_variables(expr, {4,5,1,2,3})
  //     //   + substitute_variables(expr, {5,1,2,3,4})
  //     // ;
  //     const auto subsum = expr;
  //     const auto sum =
  //       + substitute_variables(subsum, {1,2,3,4,5})
  //       - substitute_variables(subsum, {2,3,4,5,6})
  //       + substitute_variables(subsum, {3,4,5,6,1})
  //       - substitute_variables(subsum, {4,5,6,1,2})
  //       + substitute_variables(subsum, {5,6,1,2,3})
  //       - substitute_variables(subsum, {6,1,2,3,4})
  //     ;
  //     return to_lyndon_basis(sum);
  //   }
  // )) << "\n";


  // std::cout << to_string(space_mapping_ranks(
  //   CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7,8}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{
  //       to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,7,8,1})),
  //       to_lyndon_basis(expr + substitute_variables(expr, {1,8,7,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(expr, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(expr, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(expr, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(expr, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(expr, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(expr, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(expr, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(expr, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(expr, {2,3,4,5,6,7,8,9})
  //       ),
  //       // to_lyndon_basis(
  //       //   + pullback(substitute_variables(expr, {1,2,3,4,5,6,7,8}), {9})
  //       //   - pullback(substitute_variables(expr, {1,2,3,4,5,6,7,9}), {8})
  //       //   + pullback(substitute_variables(expr, {1,2,3,4,5,6,8,9}), {7})
  //       //   - pullback(substitute_variables(expr, {1,2,3,4,5,7,8,9}), {6})
  //       //   + pullback(substitute_variables(expr, {1,2,3,4,6,7,8,9}), {5})
  //       //   - pullback(substitute_variables(expr, {1,2,3,5,6,7,8,9}), {4})
  //       //   + pullback(substitute_variables(expr, {1,2,4,5,6,7,8,9}), {3})
  //       //   - pullback(substitute_variables(expr, {1,3,4,5,6,7,8,9}), {2})
  //       //   + pullback(substitute_variables(expr, {2,3,4,5,6,7,8,9}), {1})
  //       // ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     const auto subsum =
  //       + substitute_variables(expr, {1,2,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7,1})
  //       + substitute_variables(expr, {3,4,5,6,7,1,2})
  //       + substitute_variables(expr, {4,5,6,7,1,2,3})
  //       + substitute_variables(expr, {5,6,7,1,2,3,4})
  //       + substitute_variables(expr, {6,7,1,2,3,4,5})
  //       + substitute_variables(expr, {7,1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,7,6,5,4,3,2})
  //       - substitute_variables(expr, {2,1,7,6,5,4,3})
  //       - substitute_variables(expr, {3,2,1,7,6,5,4})
  //       - substitute_variables(expr, {4,3,2,1,7,6,5})
  //       - substitute_variables(expr, {5,4,3,2,1,7,6})
  //       - substitute_variables(expr, {6,5,4,3,2,1,7})
  //       - substitute_variables(expr, {7,6,5,4,3,2,1})
  //     ;
  //     const auto sum =
  //       + substitute_variables(subsum, {1,2,3,4,5,6,7})
  //       - substitute_variables(subsum, {1,2,3,4,5,6,8})
  //       + substitute_variables(subsum, {1,2,3,4,5,7,8})
  //       - substitute_variables(subsum, {1,2,3,4,6,7,8})
  //       + substitute_variables(subsum, {1,2,3,5,6,7,8})
  //       - substitute_variables(subsum, {1,2,4,5,6,7,8})
  //       + substitute_variables(subsum, {1,3,4,5,6,7,8})
  //       - substitute_variables(subsum, {2,3,4,5,6,7,8})
  //     ;
  //     // return to_lyndon_basis(sum);
  //     return std::tuple{
  //       to_lyndon_basis(sum + substitute_variables(sum, {2,3,4,5,6,7,8,1})),
  //       to_lyndon_basis(sum + substitute_variables(sum, {1,8,7,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(sum, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(sum, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(sum, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(sum, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(sum, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(sum, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(sum, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(sum, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(sum, {2,3,4,5,6,7,8,9})
  //       ),
  //       to_lyndon_basis(
  //         + pullback(substitute_variables(sum, {1,2,3,4,5,6,7,8}), {9})
  //         - pullback(substitute_variables(sum, {1,2,3,4,5,6,7,9}), {8})
  //         + pullback(substitute_variables(sum, {1,2,3,4,5,6,8,9}), {7})
  //         - pullback(substitute_variables(sum, {1,2,3,4,5,7,8,9}), {6})
  //         + pullback(substitute_variables(sum, {1,2,3,4,6,7,8,9}), {5})
  //         - pullback(substitute_variables(sum, {1,2,3,5,6,7,8,9}), {4})
  //         + pullback(substitute_variables(sum, {1,2,4,5,6,7,8,9}), {3})
  //         - pullback(substitute_variables(sum, {1,3,4,5,6,7,8,9}), {2})
  //         + pullback(substitute_variables(sum, {2,3,4,5,6,7,8,9}), {1})
  //       ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6,7}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     const auto subsum =
  //       + substitute_variables(expr, {1,2,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7,1})
  //       + substitute_variables(expr, {3,4,5,6,7,1,2})
  //       + substitute_variables(expr, {4,5,6,7,1,2,3})
  //       + substitute_variables(expr, {5,6,7,1,2,3,4})
  //       + substitute_variables(expr, {6,7,1,2,3,4,5})
  //       + substitute_variables(expr, {7,1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,7,6,5,4,3,2})
  //       - substitute_variables(expr, {2,1,7,6,5,4,3})
  //       - substitute_variables(expr, {3,2,1,7,6,5,4})
  //       - substitute_variables(expr, {4,3,2,1,7,6,5})
  //       - substitute_variables(expr, {5,4,3,2,1,7,6})
  //       - substitute_variables(expr, {6,5,4,3,2,1,7})
  //       - substitute_variables(expr, {7,6,5,4,3,2,1})
  //     ;
  //     const auto sum =
  //       + pullback(substitute_variables(subsum, {1,2,3,4,5,6,7}), {8})
  //       - pullback(substitute_variables(subsum, {1,2,3,4,5,6,8}), {7})
  //       + pullback(substitute_variables(subsum, {1,2,3,4,5,7,8}), {6})
  //       - pullback(substitute_variables(subsum, {1,2,3,4,6,7,8}), {5})
  //       + pullback(substitute_variables(subsum, {1,2,3,5,6,7,8}), {4})
  //       - pullback(substitute_variables(subsum, {1,2,4,5,6,7,8}), {3})
  //       + pullback(substitute_variables(subsum, {1,3,4,5,6,7,8}), {2})
  //       - pullback(substitute_variables(subsum, {2,3,4,5,6,7,8}), {1})
  //     ;
  //     // return to_lyndon_basis(sum);
  //     return std::tuple{
  //       to_lyndon_basis(sum + substitute_variables(sum, {2,3,4,5,6,7,8,1})),
  //       // to_lyndon_basis(sum + substitute_variables(sum, {1,8,7,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(sum, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(sum, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(sum, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(sum, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(sum, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(sum, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(sum, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(sum, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(sum, {2,3,4,5,6,7,8,9})
  //       ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   // CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7}),
  //   concat(
  //     CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7}),
  //     CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6,7})
  //   ),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     const auto subsum =
  //       + substitute_variables(expr, {1,2,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7,1})
  //       + substitute_variables(expr, {3,4,5,6,7,1,2})
  //       + substitute_variables(expr, {4,5,6,7,1,2,3})
  //       + substitute_variables(expr, {5,6,7,1,2,3,4})
  //       + substitute_variables(expr, {6,7,1,2,3,4,5})
  //       + substitute_variables(expr, {7,1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,7,6,5,4,3,2})
  //       - substitute_variables(expr, {2,1,7,6,5,4,3})
  //       - substitute_variables(expr, {3,2,1,7,6,5,4})
  //       - substitute_variables(expr, {4,3,2,1,7,6,5})
  //       - substitute_variables(expr, {5,4,3,2,1,7,6})
  //       - substitute_variables(expr, {6,5,4,3,2,1,7})
  //       - substitute_variables(expr, {7,6,5,4,3,2,1})
  //     ;
  //     if (expr.dimension() == 4) {
  //       const auto sum =
  //         + substitute_variables(subsum, {1,2,3,4,5,6,7})
  //         - substitute_variables(subsum, {1,2,3,4,5,6,8})
  //         + substitute_variables(subsum, {1,2,3,4,5,7,8})
  //         - substitute_variables(subsum, {1,2,3,4,6,7,8})
  //         + substitute_variables(subsum, {1,2,3,5,6,7,8})
  //         - substitute_variables(subsum, {1,2,4,5,6,7,8})
  //         + substitute_variables(subsum, {1,3,4,5,6,7,8})
  //         - substitute_variables(subsum, {2,3,4,5,6,7,8})
  //       ;
  //       return to_lyndon_basis(sum);
  //     } else {
  //       CHECK_EQ(expr.dimension(), 3);
  //       const auto sum =
  //         + pullback(substitute_variables(subsum, {1,2,3,4,5,6,7}), {8})
  //         - pullback(substitute_variables(subsum, {1,2,3,4,5,6,8}), {7})
  //         + pullback(substitute_variables(subsum, {1,2,3,4,5,7,8}), {6})
  //         - pullback(substitute_variables(subsum, {1,2,3,4,6,7,8}), {5})
  //         + pullback(substitute_variables(subsum, {1,2,3,5,6,7,8}), {4})
  //         - pullback(substitute_variables(subsum, {1,2,4,5,6,7,8}), {3})
  //         + pullback(substitute_variables(subsum, {1,3,4,5,6,7,8}), {2})
  //         - pullback(substitute_variables(subsum, {2,3,4,5,6,7,8}), {1})
  //       ;
  //       return to_lyndon_basis(sum);
  //     }
  //   }
  // )) << "\n";


  // std::cout << to_string(space_mapping_ranks(
  //   CL4({1,2,3,4,5,6}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return to_lyndon_basis(
  //       + substitute_variables(expr, {1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,2,3,4,5,7})
  //       + substitute_variables(expr, {1,2,3,4,6,7})
  //       - substitute_variables(expr, {1,2,3,5,6,7})
  //       + substitute_variables(expr, {1,2,4,5,6,7})
  //       - substitute_variables(expr, {1,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7})
  //     );
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CL4({1,2,3,4,5}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return to_lyndon_basis(
  //       + substitute_variables(expr, {1,2,3,4,5})
  //       - substitute_variables(expr, {1,2,3,4,6})
  //       + substitute_variables(expr, {1,2,3,5,6})
  //       - substitute_variables(expr, {1,2,4,5,6})
  //       + substitute_variables(expr, {1,3,4,5,6})
  //       - substitute_variables(expr, {2,3,4,5,6})
  //     );
  //   }
  // )) << "\n";


  // for (const int weight : range_incl(2, 4)) {
  //   for (const int num_points : range_incl(4, 7)) {
  //     const auto points = to_vector(range_incl(1, num_points));
  //     const auto ranks = space_mapping_ranks(
  //       concat(
  //         CGrL_test_space(weight, 2, points),
  //         CGrL_test_space(weight, 3, points),
  //         CGrL_test_space(weight, 4, points),
  //         CGrL_test_space(weight, 5, points),
  //         CGrL_test_space(weight, 6, points)
  //       ),
  //       DISAMBIGUATE(to_lyndon_basis),
  //       [&](const auto& expr) {
  //         // Note. This is actually a condition on complex cohomology, which is way more complicated
  //         //   in theory, but for practical intents and purposes we can do this since each element
  //         //   lies on one the source space bases.
  //         const auto q = GammaExpr();
  //         // const auto q = to_lyndon_basis(symmetrize_double(expr, num_points));
  //         // const auto x = to_lyndon_basis(symmetrize_loop(arrow_left(expr, num_points + 1), num_points + 1));
  //         // const auto y = to_lyndon_basis(symmetrize_loop(arrow_up(expr, num_points + 1), num_points + 1));
  //         const auto x = to_lyndon_basis(arrow_left(expr, num_points + 1));
  //         const auto y = to_lyndon_basis(arrow_up(expr, num_points + 1));
  //         const auto z = GammaExpr();
  //         switch (expr.dimension()) {
  //           // case 2: return std::tuple{x, y, z, z};
  //           // case 3: return std::tuple{z, x, y, z};
  //           // case 4: return std::tuple{z, z, x, y};
  //           // case 2: return std::tuple{x, y, z, z, z};
  //           // case 3: return std::tuple{z, x, y, z, z};
  //           // case 4: return std::tuple{z, z, x, y, z};
  //           // case 5: return std::tuple{z, z, z, x, y};
  //           case 2: return std::tuple{x, y, z, z, z, z,  q, z, z, z, z};
  //           case 3: return std::tuple{z, x, y, z, z, z,  z, q, z, z, z};
  //           case 4: return std::tuple{z, z, x, y, z, z,  z, z, q, z, z};
  //           case 5: return std::tuple{z, z, z, x, y, z,  z, z, z, q, z};
  //           case 6: return std::tuple{z, z, z, z, x, y,  z, z, z, z, q};
  //           default: FATAL("Unexpected dimension");
  //         };
  //       }
  //     );
  //     std::cout << "w=" << weight << ", n=" << num_points << ":  " << to_string(ranks) << "\n";
  //   }
  // }


  // TablePrinter t;
  // // for (const int weight : {3}) {
  // //   for (const int n : {3}) {
  // for (const int weight : range_incl(2, 6)) {
  //   for (const int n : range_incl(1, 8)) {
  //     const int fixed_a = 1;
  //     const int fixed_b = 2;
  //     const int nonfixed_start = 3;
  //     PolylogSpace space;
  //     for (const auto& seq : nondecreasing_sequences(n, weight)) {
  //       const auto symbol = tensor_product(absl::MakeConstSpan(mapped(seq, [](const int k) {
  //         return D(k + nonfixed_start, fixed_a) - D(fixed_b, fixed_a);
  //       })));
  //       // std::cout << to_lyndon_basis(symbol);
  //       space.push_back(symbol);
  //       // space.push_back(to_lyndon_basis(symbol));
  //     }
  //     // const auto ranks = space_mapping_ranks(
  //     //   space,
  //     //   DISAMBIGUATE(identity_function),
  //     //   [](const auto& expr) {
  //     //     return to_lyndon_basis(project_on(fixed_b, expr));
  //     //   }
  //     // );
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(identity_function),
  //       [](const auto& expr) {
  //         return to_lyndon_basis(expr.filtered([&](const std::vector<Delta>& term) {
  //           // return count_var(term, fixed_b) > 0;
  //           return count_var(term, fixed_b) == 1;
  //           // return true;
  //         }));
  //       }
  //     );
  //     // std::cout << "w=" << weight << ", n=" << n << ": " << to_string(ranks) << "\n";
  //     // t.set_content({n - 1, weight - 2}, to_string(ranks.kernel()));
  //     const int expected = sum(mapped(range_incl(0, weight), [&](const int k) { return binomial(n-1, k); }));
  //     t.set_content(
  //       {n - 1, weight - 2},
  //       // absl::StrCat(ranks.kernel(), "~", expected)
  //       absl::StrCat(ranks.kernel())
  //     );
  //     CHECK_EQ(ranks.kernel(), expected);
  //   }
  // }
  // std::cout << t.to_string();


  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto ranks = space_mapping_ranks(
  //     mapped(GrL3(3, points), DISAMBIGUATE(to_lyndon_basis)),
  //     DISAMBIGUATE(identity_function),
  //     [](const auto& expr) {
  //       return project_on(1, expr);
  //     }
  //   );
  //   const auto lower_rank = space_rank(GrL3(3, slice(points, 1)), DISAMBIGUATE(to_lyndon_basis));
  //   std::cout << "p=" << num_points << ":  " << to_string(ranks) << "  vs  " << lower_rank << "\n";
  // }


  // const auto triplet_tmpl =
  //   + GammaExpr::single({Gamma({1,2}), Gamma({1,3})})
  //   + GammaExpr::single({Gamma({1,3}), Gamma({1,4})})
  //   + GammaExpr::single({Gamma({1,4}), Gamma({1,2})})
  // ;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // for (const int num_points : range_incl(4, 8)) {
  //   const int dimension = 3;
  //   const int weight = 3;
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto coords = combinations(points, dimension);
  //   GrPolylogNCoSpace space_comult;
  //   for (const auto& word : get_lyndon_words(coords, weight)) {
  //     const auto expr = GammaExpr::single(mapped(word, convert_to<Gamma>));
  //     const auto expr_pr = pr(expr);
  //     if (!expr_pr.is_zero()) {
  //       space_comult.push_back(ncomultiply(expr_pr, {1,2}));
  //     }
  //   }
  //   GrPolylogSpace space_triplets;
  //   for (const auto& indices : permutations(slice(points, 1), 4)) {
  //     space_triplets.push_back(substitute_variables(triplet_tmpl, indices));
  //   }
  //   GrPolylogSpace fx = mapped(GrFx(dimension, points), pr);
  //   GrPolylogNCoSpace second_space = mapped(cartesian_product(space_triplets, fx), applied(DISAMBIGUATE(ncoproduct)));
  //   // TODO: Space coproduct helper function !!!
  //   const auto ranks = space_venn_ranks(
  //     space_comult,
  //     second_space,
  //     DISAMBIGUATE(identity_function)
  //   );
  //   std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  // }


  // const int dimension = 3;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto ranks = space_venn_ranks(
  //     mapped(GrFx(dimension, points), pr),
  //     mapped(GrL1(dimension, points), pr),
  //     DISAMBIGUATE(to_lyndon_basis)
  //   );
  //   std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  // }

  // const int dimension = 3;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // const int weight = 2;
  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   GrPolylogSpace l2 = mapped(GrL2(dimension, points), pr);
  //   GrPolylogSpace l1 = mapped(GrFx(dimension, points), pr);
  //   // GrPolylogSpace l2 = GrL2(dimension, points);
  //   // GrPolylogSpace l1 = GrL1(dimension, points);
  //   GrPolylogNCoSpace space = mapped(
  //     cartesian_combinations(std::vector{std::pair{l2, 1}, std::pair{l1, weight-2}}),
  //     DISAMBIGUATE(ncoproduct_vec)
  //   );
  //   const auto ranks = space_mapping_ranks(
  //     space,
  //     DISAMBIGUATE(identity_function),
  //     DISAMBIGUATE(ncomultiply)
  //   );
  //   // const auto lambda3_rank = space_rank(
  //   //   mapped(combinations(l1, 3), DISAMBIGUATE(ncoproduct_vec)),
  //   //   DISAMBIGUATE(to_lyndon_basis)
  //   // );
  //   const int l1_rank = space_rank(l1, DISAMBIGUATE(to_lyndon_basis));
  //   const int lambda3_l1_rank = binomial(l1_rank, weight);
  //   // std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  //   std::cout << "p=" << num_points << ": " << lambda3_l1_rank << " - " << ranks.image();
  //   std::cout << " = " << (lambda3_l1_rank - ranks.image()) << "\n";
  // }

  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const int rank = space_rank(mapped(GrFx(dimension, points), pr), DISAMBIGUATE(to_lyndon_basis));
  //   std::cout << "p=" << num_points << ": " << rank << "\n";
  // }


  // std::cout << space_rank(GrL4_Dim3({1,2,3,4,5,6,7}), DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // std::cout << space_rank(
  //   mapped(GrL4_Dim3({1,2,3,4,5,6,7}), [](const auto& expr) {
  //     return project_on(7, expr);
  //   }),
  //   DISAMBIGUATE(to_lyndon_basis)
  // ) << "\n";

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6,7};
  // const auto space = mapped(
  //   concat(
  //     cartesian_combinations(std::vector{std::pair{GrL3(dimension, points), 1}, std::pair{GrL1(dimension, points), 1}}),
  //     cartesian_combinations(std::vector{std::pair{GrL2(dimension, points), 2}})
  //   ),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return ncomultiply(expr);
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6};
  // const auto cl1 = CGrL_test_space(1, dimension, points);
  // const auto cl2 = CGrL_test_space(2, dimension, points);
  // const auto cl3 = CGrL_test_space(3, dimension, points);
  // const auto space = mapped(
  //   concat(
  //     cartesian_combinations(std::vector{std::pair{cl3, 1}, std::pair{cl1, 1}}),
  //     cartesian_combinations(std::vector{std::pair{cl2, 2}})
  //   ),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6,7};
  // const auto space = mapped(
  //   cartesian_combinations(std::vector{std::pair{GrL2(dimension, points), 1}, std::pair{GrL1(dimension, points), 2}}),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return ncomultiply(expr);
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  const int weight = 4;
  const int dimension = 3;
  const std::vector points = {1,2,3,4,5,6};
  const auto coords = combinations(slice(points, 1), dimension - 1);
  GrPolylogACoSpace space_words;
  // TODO: factor out as `gr_free_lie_coalgebra(weight, dimension, points)`
  for (const auto& word : get_lyndon_words(coords, weight)) {
    const auto expr = GammaExpr::single(mapped(word, convert_to<Gamma>));
    // if (is_totally_weakly_separated(expr)) {
    //   space_words.push_back(expand_into_glued_pairs(expr));
    // }
    space_words.push_back(expand_into_glued_pairs(expr));
  }
  const auto space_l = mapped(
    cartesian_combinations(std::vector{
      std::pair{GrL2(dimension, points), 1},
      std::pair{GrL1(dimension, points), 1},
      std::pair{GrL1(dimension, points), 1},
    }),
    [](const auto& exprs) {
      return abstract_coproduct_vec<GammaACoExpr>(mapped(exprs, [](const auto& e) {
        return project_on(1, e);
      }));
    }
  );
  const auto ranks = space_venn_ranks(
    space_words,
    space_l,
    DISAMBIGUATE(identity_function)
  );
  std::cout << to_string(ranks) << "\n";
}
