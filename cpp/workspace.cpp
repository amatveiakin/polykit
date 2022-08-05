// Optimization potential. To improve compilation time:
//   - Add DECLARE_EXPR/DEFINE_EXPR macros, move lyndon and co-product to .cpp;
//   - Add a way to disable co-products and corresponding expressions.

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/integer_math.h"
#include "lib/itertools.h"
#include "lib/linalg.h"
#include "lib/linalg_solvers.h"
#include "lib/lyndon.h"
#include "lib/polylog_qli.h"
#include "lib/polylog_type_ac_space.h"
#include "lib/polylog_type_c_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/space_algebra.h"
#include "lib/summation.h"
#include "lib/zip.h"

// In order to reduce compilation time enable expressions only when necessary:

#if 1
#include "lib/bigrassmannian_complex_cohomologies.h"
#include "lib/gamma.h"
#include "lib/chern_arrow.h"
#include "lib/chern_cocycle.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_gr_space.h"
#elif defined(HAS_GAMMA_EXPR)
#  error "Expression type leaked: check header structure"
#endif

#if 0
#include "lib/kappa.h"
#include "lib/polylog_type_d_space.h"
#elif defined(HAS_KAPPA_EXPR)
#  error "Expression type leaked: check header structure"
#endif

#if 0
#include "lib/corr_expression.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_via_correlators.h"
#elif defined(HAS_CORR_EXPR)
#  error "Expression type leaked: check header structure"
#endif

#if 0
#include "lib/epsilon.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/loops_aux.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/snowpal.h"
#include "lib/theta.h"
#elif defined(HAS_EPSILON_EXPR) || defined(HAS_THETA_EXPR) || defined(HAS_LIRA_EXPR)
#  error "Expression type leaked: check header structure"
#endif


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
    .set_expression_line_limit(30)
    // .set_annotation_sorting(AnnotationSorting::length)
    .set_annotation_sorting(AnnotationSorting::lexicographic)
    .set_compact_x(true)
    .set_max_terms_in_annotations_one_liner(100)
  );



  // const auto prepare = [](const GammaExpr& expr) {
  //   return prnt::set_object_to_string(
  //     expr.filtered([](const auto& term) {
  //       return all_vars(term).size() == 5 && common_vars(term).size() == 1;
  //     }),
  //     [](const auto& term) {
  //       const auto vars = common_vars(term);
  //       return absl::StrCat(
  //         internal::GammaExprParam::object_to_string(term),
  //         "   ",
  //         str_join(vars, ",")
  //       );
  //     }
  //   );
  // };
  // auto lhs = to_lyndon_basis(GLi3(1,2,3,4,5,6));
  // lhs = lhs.filtered([](const auto& term) {
  //   return absl::c_all_of(
  //     transposed(mapped(term, [](const Gamma& g) { return g.index_vector(); })),
  //     DISAMBIGUATE(is_strictly_increasing)
  //   );
  // });
  // std::cout << prepare(lhs);

  // GammaExpr rhs;
  // for (const auto& [points, sign] : permutations_with_sign({1,2,3,4,5,6})) {
  //   rhs += sign * tensor_product(absl::MakeConstSpan({
  //     G(choose_indices_one_based(points, {1,2,3})),
  //     G(choose_indices_one_based(points, {2,3,4})),
  //     G(choose_indices_one_based(points, {3,4,5})),
  //   }));
  // }
  // rhs = to_lyndon_basis(rhs)
  //   .dived_int(2)
  //   .filtered(DISAMBIGUATE(is_weakly_separated))
  // ;
  // std::cout << prepare(rhs);
  // std::cout << prepare(lhs - rhs);


  // GammaExpr p, q;
  // for (const auto& [points, sign] : permutations_with_sign({1,2,3,4})) {
  //   p += sign * tensor_product(absl::MakeConstSpan({
  //     G(choose_indices_one_based(points, {1,2})),
  //     G(choose_indices_one_based(points, {2,3})),
  //   }));
  //   q += sign * GLiVec(2, points);
  // }
  // p = to_lyndon_basis(p);
  // q = to_lyndon_basis(q).without_annotations();
  // std::cout << p;
  // std::cout << q;
  // std::cout << p.dived_int(2) + q.dived_int(24);

  // GammaExpr p, q;
  // for (const auto& [points, sign] : permutations_with_sign({1,2,3,4,5,6})) {
  //   p += sign * tensor_product(absl::MakeConstSpan({
  //     G(choose_indices_one_based(points, {1,2,3})),
  //     G(choose_indices_one_based(points, {2,3,4})),
  //     G(choose_indices_one_based(points, {3,4,5})),
  //   }));
  //   q += sign * GLiVec(3, points);
  // }
  // p = to_lyndon_basis(p);
  // q = to_lyndon_basis(q).without_annotations();
  // std::cout << p;
  // std::cout << q;
  // std::cout << p.dived_int(2) - q.dived_int(240);


  // Profiler profiler;
  // const auto expr = GLi5(1,2,3,4,5,6,7,8);
  // profiler.finish("expr");
  // auto coexpr = ncomultiply(expr, {1,4});
  // profiler.finish("comult");
  // std::cout << "\n";
  // // This zeroes out terms where we take three points from {1,2,3,4} and one point from {5,6,7,8}.
  // // Should also do vice versa to the the full equation.
  // coexpr +=
  //   + ncoproduct(GLi4(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   + ncoproduct(
  //     + GLiVec(4, {5}, {1,2,4,6,7,8})
  //     - GLiVec(4, {6}, {1,2,4,5,7,8})
  //     + GLiVec(4, {7}, {1,2,4,5,6,8})
  //     ,
  //     plucker({1,2,4,8})
  //   )
  //   - ncoproduct(
  //     + GLiVec(4, {5}, {1,3,4,6,7,8})
  //     - GLiVec(4, {6}, {1,3,4,5,7,8})
  //     + GLiVec(4, {7}, {1,3,4,5,6,8})
  //     ,
  //     plucker({1,3,4,8})
  //   )
  //   + ncoproduct(
  //     + GLiVec(4, {5}, {2,3,4,6,7,8})
  //     - GLiVec(4, {6}, {2,3,4,5,7,8})
  //     + GLiVec(4, {7}, {2,3,4,5,6,8})
  //     ,
  //     plucker({2,3,4,8})
  //   )
  //   + ncoproduct(
  //     + GLiVec(4, {5}, {1,2,4,6,7,8})
  //     - GLiVec(4, {5}, {1,3,4,6,7,8})
  //     + GLiVec(4, {5}, {2,3,4,6,7,8})
  //     ,
  //     plucker({1,2,3,5})
  //   )
  //   - ncoproduct(
  //     + GLiVec(4, {6}, {1,2,4,5,7,8})
  //     - GLiVec(4, {6}, {1,3,4,5,7,8})
  //     + GLiVec(4, {6}, {2,3,4,5,7,8})
  //     ,
  //     plucker({1,2,3,6})
  //   )
  //   + ncoproduct(
  //     + GLiVec(4, {7}, {1,2,4,5,6,8})
  //     - GLiVec(4, {7}, {1,3,4,5,6,8})
  //     + GLiVec(4, {7}, {2,3,4,5,6,8})
  //     ,
  //     plucker({1,2,3,7})
  //   )
  //   - ncoproduct(GLiVec(4, {5}, {1,2,4,6,7,8}), plucker({1,2,4,5}))
  //   + ncoproduct(GLiVec(4, {6}, {1,2,4,5,7,8}), plucker({1,2,4,6}))
  //   - ncoproduct(GLiVec(4, {7}, {1,2,4,5,6,8}), plucker({1,2,4,7}))
  //   + ncoproduct(GLiVec(4, {5}, {1,3,4,6,7,8}), plucker({1,3,4,5}))
  //   - ncoproduct(GLiVec(4, {6}, {1,3,4,5,7,8}), plucker({1,3,4,6}))
  //   + ncoproduct(GLiVec(4, {7}, {1,3,4,5,6,8}), plucker({1,3,4,7}))
  //   - ncoproduct(GLiVec(4, {5}, {2,3,4,6,7,8}), plucker({2,3,4,5}))
  //   + ncoproduct(GLiVec(4, {6}, {2,3,4,5,7,8}), plucker({2,3,4,6}))
  //   - ncoproduct(GLiVec(4, {7}, {2,3,4,5,6,8}), plucker({2,3,4,7}))
  //   - ncoproduct(
  //     + GLiVec(4, {1,2,3,4,5,6,7,8})
  //     + GLiVec(4, {5}, {2,3,4,6,7,8})
  //     - GLiVec(4, {5}, {1,3,4,6,7,8})
  //     + GLiVec(4, {5}, {1,2,4,6,7,8})
  //     - GLiVec(4, {6}, {2,3,4,5,7,8})
  //     + GLiVec(4, {6}, {1,3,4,5,7,8})
  //     - GLiVec(4, {6}, {1,2,4,5,7,8})
  //     + GLiVec(4, {7}, {2,3,4,5,6,8})
  //     - GLiVec(4, {7}, {1,3,4,5,6,8})
  //     + GLiVec(4, {7}, {1,2,4,5,6,8})
  //     ,
  //     plucker({1,2,3,8})
  //   )
  // ;
  // std::cout << coexpr;
  // std::cout << coexpr.termwise_abs().mapped<GammaExpr>([](const auto& term) {
  //   return term.at(0);
  // });
  // // const auto lhs = filter_coexpr(coexpr, 0, std::vector{Gamma({1,2,3,8})});
  // // const auto rhs = ;
  // // std::cout << lhs;
  // // std::cout << rhs;
  // // std::cout << lhs + rhs;


  // const auto expr =
  //   + GLi4(1,2,3,4,5,6,7,8)
  //   - GLi4(1,2,3,4,5,6,7,9)
  //   + GLi4(1,2,3,4,5,6,8,9)
  //   - GLi4(1,2,3,4,5,7,8,9)
  //   + GLi4(1,2,3,4,6,7,8,9)
  // ;
  // auto coexpr = ncomultiply(expr, {1,});
  // std::cout << coexpr.termwise_abs().mapped<GammaExpr>([](const auto& term) {
  //   return term.at(0);
  // });
  // // std::cout << to_lyndon_basis(
  // //   + expr
  // //   + substitute_variables_1_based(expr, {1,2,4,3,5,6,7,8,9})
  // // );


  // for (const int p : range_incl(3, 5)) {
  //   const auto lhs = GLiVec(p-1, seq_incl(1, 2*p));
  //   const auto rhs = neg_one_pow(p) * a_minus(b_plus(GLiVec(p-1, seq_incl(1, 2*p-2)), 2*p-1), 2*p);
  //   std::cout << to_lyndon_basis(lhs - rhs);
  // }

  // for (const int p : range_incl(3, 5)) {
  //   const auto lhs = GLiVec(p-1, seq_incl(1, 2*p));
  //   const auto x = GLiVec(p-1, seq_incl(1, 2*p-2));
  //   const auto rhs = neg_one_pow(p) * b_minus(a_full(x, 2*p-1) - a_plus(x, 2*p-1), 2*p);
  //   std::cout << to_lyndon_basis(lhs - rhs);
  // }

  // for (const int p : range_incl(3, 4)) {
  //   const auto x = GLiVec(p, seq_incl(1, 2*p-2));
  //   const auto expr = a_full(
  //     + GLiVec(p, seq_incl(1, 2*p))
  //     + neg_one_pow(p-1) * (
  //       + a_plus(b_full(x, 2*p-1), 2*p)
  //       - a_plus(b_plus(x, 2*p-1), 2*p)
  //     )
  //     , 2*p+1
  //   );
  //   std::cout << to_lyndon_basis(expr);
  // }



  // for (const int n : range_incl(4, 5)) {
  //   for (const int p : range_incl(3, 5)) {
  //     if (!are_GLi_args_ok(n-1, 2*p)) {
  //       continue;
  //     }

  //     const auto gli_large = GLiVec(n-1, seq_incl(1, 2*p));
  //     const auto gli_small = GLiVec(n-1, seq_incl(1, 2*p-2));

  //     const auto s = neg_one_pow(p);
  //     const auto lhs = ncomultiply(GLiVec(n, seq_incl(1, 2*p)), {1,n-1});
  //     const auto rhs =
  //       + ncoproduct(gli_large, plucker(seq_incl(1, p)))
  //       + ncoproduct(gli_large, plucker(seq_incl(p+1, 2*p)))

  //       - s * a_minus(ncoproduct(
  //         b_plus(gli_small, 2*p-1),
  //         plucker(concat(seq_incl(1, p-1), {2*p-1}))
  //       ), 2*p)
  //       - s * a_plus(ncoproduct(
  //         b_minus(gli_small, 2*p-1),
  //         plucker(seq_incl(p, 2*p-1))
  //       ), 2*p)

  //       + s * b_plus(ncoproduct(
  //         a_minus(gli_small, 2*p-1),
  //         plucker(seq_incl(1, p-1))
  //       ), 2*p)
  //       + s * b_minus(ncoproduct(
  //         a_plus(gli_small, 2*p-1),
  //         plucker(seq_incl(p, 2*p-2))
  //       ), 2*p)

  //       + s *  a_minus(b_plus(
  //         ncoproduct(gli_small, plucker(seq_incl(1, p-1)))
  //       , 2*p-1), 2*p)
  //       + s * a_plus(b_minus(
  //         ncoproduct(gli_small, plucker(seq_incl(p, 2*p-2)))
  //       , 2*p-1), 2*p)

  //       - ncoproduct(
  //         + gli_large
  //         - s * a_minus(b_plus(gli_small, 2*p-1), 2*p)
  //         ,
  //         plucker(concat(seq_incl(1, p-1), {2*p}))
  //       )
  //       - ncoproduct(
  //         + gli_large
  //         - s * a_plus(b_minus(gli_small, 2*p-1), 2*p)
  //         ,
  //         plucker(seq_incl(p, 2*p-1))
  //       )
  //     ;
  //     const auto coexpr = lhs + rhs;
  //     // std::cout << coexpr.termwise_abs().mapped<GammaExpr>([](const auto& term) {
  //     //   return term.at(0);
  //     // });
  //     std::cout << coexpr;
  //   }
  // }


  // const std::vector exprs_odd_num_points = {
  //   GLi2[{5}](1,2,3,4),
  //   GrQLi2(5)(1,2,3,4),
  //   GrLi(5)(1,2,3,4),
  //   G({1,2,3,4,5}),
  //   tensor_product(G({1,2,3}), G({3,4,5})),
  // };
  // const std::vector exprs_even_num_points = {
  //   GLi2(1,2,3,4),
  //   GrQLi2()(1,2,3,4),
  //   GrLi(5,6)(1,2,3,4),
  //   G({1,2,3,4,5,6}),
  //   tensor_product(G({1,2,3,4}), G({3,4,5,6})),
  // };
  // const auto exprs_any_num_points = concat(exprs_odd_num_points, exprs_even_num_points);

  // for (const auto& expr : exprs_odd_num_points) {
  //   const int n = detect_num_variables(expr);
  //   CHECK(a_minus_minus(expr, n+1) == a_minus(expr, n+1));
  //   CHECK(a_plus_plus(expr, n+1) == a_plus(expr, n+1));
  //   CHECK(b_minus_minus(expr, n+1) == b_minus(expr, n+1));
  //   CHECK(b_plus_plus(expr, n+1) == b_plus(expr, n+1));
  // }
  // using ArrowF = std::function<GammaExpr(const GammaExpr&, int)>;
  // const std::vector<ArrowF> basic_arrows = {
  //   DISAMBIGUATE(a_full),
  //   DISAMBIGUATE(a_minus),
  //   DISAMBIGUATE(a_plus),
  //   DISAMBIGUATE(b_full),
  //   DISAMBIGUATE(b_minus),
  //   DISAMBIGUATE(b_plus),
  // };
  // const std::vector<ArrowF> extra_arrows = {
  //   DISAMBIGUATE(a_minus_minus),
  //   DISAMBIGUATE(a_plus_plus),
  //   DISAMBIGUATE(b_minus_minus),
  //   DISAMBIGUATE(b_plus_plus),
  // };
  // for (const bool even_num_point : {false, true}) {
  //   std::cout << "\n\n# " << (even_num_point ? "Even" : "Odd") << " num points:\n\n";
  //   const auto& test_exprs = even_num_point ? exprs_even_num_points : exprs_odd_num_points;
  //   // TODO: Only use extra arrows when makes sense
  //   // const auto arrows = <?> ? basic_arrows : concat(basic_arrows, extra_arrows);
  //   const auto arrows = concat(basic_arrows, extra_arrows);
  //   absl::flat_hash_set<std::pair<int, int>> zeros;
  //   for (const int out : range(arrows.size())) {
  //     for (const int in : range(arrows.size())) {
  //       const auto make_eqn = [&](const auto& expr, const int n) {
  //         return arrows[out](arrows[in](expr, n+1), n+2);
  //       };
  //       bool eqn_holds = true;
  //       for (const auto& expr : test_exprs) {
  //         const int n = detect_num_variables(expr);
  //         const auto eqn = make_eqn(expr, n);
  //         if (!eqn.is_zero()) {
  //           eqn_holds = false;
  //           break;
  //         }
  //       }
  //       if (eqn_holds) {
  //         const auto expr = GammaExpr().annotate("x");
  //         const auto eqn = make_eqn(expr, 0);
  //         std::cout << annotations_one_liner(eqn.annotations()) << " == 0\n";
  //         zeros.insert({out, in});
  //       }
  //     }
  //   }
  //   std::cout << "\n";
  //   for (const int l_out : range(arrows.size())) {
  //     for (const int l_in : range(arrows.size())) {
  //       for (const int r_out : range(arrows.size())) {
  //         for (const int r_in : range(arrows.size())) {
  //           const bool is_trivial =
  //             std::tie(l_out, l_in) >= std::tie(r_out, r_in)
  //             || zeros.contains({l_out, l_in})
  //             || zeros.contains({r_out, r_in})
  //           ;
  //           if (is_trivial) {
  //             continue;
  //           }
  //           for (const int sign : {-1, 1}) {
  //             const auto make_eqn = [&](const auto& expr, const int n) {
  //               return
  //                 + arrows[l_out](arrows[l_in](expr, n+1), n+2)
  //                 + sign * arrows[r_out](arrows[r_in](expr, n+1), n+2)
  //               ;
  //             };
  //             bool eqn_holds = true;
  //             for (const auto& expr : test_exprs) {
  //               const int n = detect_num_variables(expr);
  //               const auto eqn = make_eqn(expr, n);
  //               if (!eqn.is_zero()) {
  //                 eqn_holds = false;
  //                 break;
  //               }
  //             }
  //             if (eqn_holds) {
  //               const auto expr = GammaExpr().annotate("x");
  //               const auto eqn = make_eqn(expr, 0);
  //               std::cout << annotations_one_liner(eqn.annotations()) << " == 0\n";
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }


  // for (const int dimension : range_incl(3, 5)) {
  //   const int weight = dimension - 1;
  //   const int num_points = dimension * 2;
  //   const auto points = seq_incl(1, num_points);
  //   const auto space = ChernGrL(weight, dimension, points);
  //   const auto ranks = space_mapping_ranks(
  //     space,
  //     DISAMBIGUATE(to_lyndon_basis),
  //     [&](const auto& expr) {
  //       return std::tuple{
  //         to_lyndon_basis(a_minus(expr, num_points + 1)),
  //         to_lyndon_basis(a_plus(expr, num_points + 1)),
  //         to_lyndon_basis(b_minus(expr, num_points + 1)),
  //         to_lyndon_basis(b_plus(expr, num_points + 1)),
  //         to_lyndon_basis(expr + neg_one_pow(dimension) * plucker_dual(expr, points)),
  //       };
  //     }
  //   );
  //   std::cout << to_string(ranks) << "\n";
  //   // for (const auto& expr : space) {
  //   //   if (
  //   //     to_lyndon_basis(a_minus(expr, num_points + 1)).is_zero() &&
  //   //     to_lyndon_basis(a_plus(expr, num_points + 1)).is_zero() &&
  //   //     to_lyndon_basis(b_minus(expr, num_points + 1)).is_zero() &&
  //   //     to_lyndon_basis(b_plus(expr, num_points + 1)).is_zero()
  //   //   ) {
  //   //     std::cout << expr;
  //   //   }
  //   // }
  // }



  // using ArrowF = std::function<GammaNCoExpr(const GammaNCoExpr&, int)>;
  // using ArrowF = std::function<GammaExpr(const GammaExpr&, int)>;
  // const std::vector<ArrowF> arrows = {
  //   DISAMBIGUATE(a_full),
  //   DISAMBIGUATE(a_minus),
  //   DISAMBIGUATE(a_plus),
  //   DISAMBIGUATE(b_full),
  //   DISAMBIGUATE(b_minus),
  //   DISAMBIGUATE(b_plus),
  //   DISAMBIGUATE(a_minus_minus),
  //   DISAMBIGUATE(a_plus_plus),
  //   DISAMBIGUATE(b_minus_minus),
  //   DISAMBIGUATE(b_plus_plus),
  // };
  // for (const auto p : range_incl(3, 4)) {
  //   for (const auto& out : arrows) {
  //     for (const auto& in : arrows) {
  //       for (const int sign : {-1, 1}) {
  //         const auto eqn = to_lyndon_basis(
  //           + GLiVec(p, seq_incl(1, 2*p+2))
  //           + sign * neg_one_pow(p) * out(in(GLiVec(p, seq_incl(1, 2*p)), 2*p+1), 2*p+2)
  //         );
  //         if (eqn.is_zero()) {
  //           std::cout << annotations_one_liner(eqn.annotations()) << " = 0\n";
  //         }
  //       }
  //     }
  //   }
  //   std::cout << "\n";
  // }


  // // TODO: factor out sigma_i (a.k.a "co-degeneration maps")
  // static constexpr auto sigma = [](int i, const auto& expr, int dst_vars) {
  //   CHECK_LT(i, dst_vars);
  //   return substitute_variables_0_based(expr, concat(seq_incl(0, i), seq(i, dst_vars)));
  // };
  // static constexpr auto add_to_each_multiple = [](X new_var, const auto& expr) {
  //   return expr.mapped_expanding([&](const auto& term) {
  //     return tensor_product(absl::MakeConstSpan(mapped(term, [&](const X var) {
  //       return
  //         + ProjectionExpr::single({var})
  //         + ProjectionExpr::single({new_var})
  //       ;
  //     })));
  //   });
  // };
  // static constexpr auto diffs = [](std::vector<int> v) {
  //   absl::c_adjacent_difference(v, v.begin());
  //   return slice(v, 1);
  // };
  // static constexpr auto get_unsorted_partitions_allow_zero = [](int n, int num_summands) {
  //   const int q = n + num_summands - 1;
  //   return mapped(combinations(seq_incl(1, q), num_summands - 1), [&](const auto& s) {
  //     return mapped(diffs(concat(std::vector{0}, s, std::vector{q+1})), [](const int k) {
  //       return k - 1;
  //     });
  //   });
  // };
  // static constexpr auto kernel_element = [](const int weight, const int num_points) {
  //   return sum(get_unsorted_partitions_allow_zero(weight - num_points + 1, num_points), [&](const auto& subweights) {
  //     // std::cout << dump_to_string(mapped(range(num_points * 2 - 1), [&](const int k) {
  //     //   const int i = k / 2;
  //     //   return (k % 2 == 0)
  //     //     ? absl::StrCat(i, "^", subweights.at(i))
  //     //     : absl::StrCat(i, "-", i + 1)
  //     //   ;
  //     // })) << "\n";
  //     return tensor_product(absl::MakeConstSpan(mapped(range(num_points * 2 - 1), [&](const int k) {
  //       const int i = k / 2;
  //       return (k % 2 == 0)
  //         ? ProjectionExpr::single(std::vector(subweights.at(i), X(i)))
  //         : ProjectionExpr::single({X(i)}) - ProjectionExpr::single({X(i + 1)})
  //       ;
  //     })));
  //   });
  // };

  // for (const int num_points : range_incl(2, 6)) {
  //   for (const int weight : range_incl(2, 6)) {
  //     const int m = num_points - 1;
  //     const auto coords = seq(num_points);
  //     // const auto space = mapped(get_lyndon_words(coords, weight), [](const auto& word) {
  //     //   return ProjectionExpr::single(mapped(word, convert_to<X>));
  //     // });
  //     const auto space = mapped(combinations_with_replacement(coords, weight), [](const auto& word) {
  //       return ProjectionExpr::single(mapped(word, convert_to<X>));
  //     });
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(to_lyndon_basis),
  //       [&](const auto& expr) {
  //         return concat(
  //           mapped(
  //             range(m),
  //             [&](const int i) { return to_lyndon_basis(sigma(i, expr, m)); }
  //           ),
  //           mapped(
  //             range(num_points),
  //             [&](const int i) { return to_lyndon_basis(expr - add_to_each_multiple(i, expr)); }
  //           )
  //         );
  //       }
  //     );
  //     std::cout << "m=" << m << ", w=" << weight << ": " << to_string(ranks) << "\n";
  //     const auto expr = kernel_element(weight, num_points - 1);
  //     for (const int i : range(m - 1)) {
  //       CHECK(to_lyndon_basis(sigma(i, expr, m)).is_zero()) << i;
  //     }
  //     for (const int i : range(num_points - 1)) {
  //       CHECK(to_lyndon_basis(expr - add_to_each_multiple(i, expr)).is_zero());
  //     }
  //     // std::cout << "kernel ok\n";
  //   }
  // }


}
