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

#if 0
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

#if 1
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



// Gr_Space frozen_vars(int num_points) {
//   return mapped(range_incl(1, num_points), [&](const auto a) {
//     return plucker({a, a % num_points + 1});
//   });
// }



static constexpr auto cycle = loop_expr_cycle;

#define DUMP(expr) std::cout << STRINGIFY(expr) << " " << expr

int loops_united_permutation_sign(const Loops& loops) {
  absl::flat_hash_set<int> variables;
  std::vector<int> unique_variables;
  for (const auto& loop : loops) {
    for (const int v : loop) {
      if (variables.insert(v).second) {
        unique_variables.push_back(v);
      }
    }
  }
  return permutation_sign(unique_variables);
}

struct LoopIndexExprParam : SimpleLinearParam<int> {
  static std::string object_to_string(const ObjectT& index) {
    return pretty_print_loop_kind_index(index, true);
  }
};

using LoopIndexExpr = Linear<LoopIndexExprParam>;

LoopIndexExpr loop_expr_to_index_symmetrized(const LoopExpr& expr) {
  return expr.mapped<LoopIndexExpr>([](const auto& expr) {
    return loop_kinds.loops_index(expr);
  });
}

LoopIndexExpr loop_expr_to_index_antisymmetrized(const LoopExpr& expr) {
  return expr.mapped_expanding([](const auto& expr) {
    const int sign = loops_united_permutation_sign(expr);
    return sign * LoopIndexExpr::single(loop_kinds.loops_index(expr));
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
    .set_expression_line_limit(300)
    // .set_annotation_sorting(AnnotationSorting::length)
    .set_annotation_sorting(AnnotationSorting::lexicographic)
    .set_compact_x(true)
    .set_max_terms_in_annotations_one_liner(100)
  );

  // CHECK_EQ(argc, 2);
  // const int app_arg = atoi(argv[1]);


#if 0
  std::vector exprs_odd_num_points = {
    GLi2[{7}](1,2,3,4,5,6),
    GrQLi2(7)(1,2,3,4,5,6),
    GrLi(5)(1,2,3,4),
    G({1,2,3,4,5}),
    tensor_product(G({1,2,3}), G({3,4,5})),
  };
  std::vector exprs_even_num_points = {
    GLi2(1,2,3,4,5,6),
    GrQLi2()(1,2,3,4,5,6),
    GrLi(5,6)(1,2,3,4),
    G({1,2,3,4,5,6}),
    tensor_product(G({1,2,3,4}), G({3,4,5,6})),
  };
  std::vector exprs_any_num_points = concat(exprs_odd_num_points, exprs_even_num_points);

  const auto ab_functions = list_nco_ab_function(ABDoublePlusMinus::Include);
  const std::vector c_functions = {
    c_minus,
    c_plus,
  };

  // std::vector<std::string> zero_eqns;
  // for (const auto& ab : ab_functions) {
  //   for (const auto& c : c_functions) {
  //     for (const bool c_inside : {false, true}) {
  //       const auto make_eqn = [&](const auto& x, const int n) {
  //         return c_inside
  //           ? ab(c(x, n), n+1)
  //           : c(ab(x, n+1), n+1)
  //         ;
  //       };
  //       bool eqn_holds = true;
  //       for (const auto& expr : exprs_any_num_points) {
  //         const int n = detect_num_variables(expr);
  //         const auto eqn = make_eqn(ncoproduct(expr), n);
  //         if (!eqn.is_zero()) {
  //           eqn_holds = false;
  //           break;
  //         }
  //       }
  //       if (eqn_holds) {
  //         const auto x = GammaNCoExpr().annotate("x");
  //         const auto eqn = make_eqn(x, 0);
  //         std::cout << annotations_one_liner(eqn.annotations()) << " == 0";
  //       }
  //     }
  //   }
  // }

  for (const int l_ab_idx : range(ab_functions.size())) {
    for (const int l_c_idx : range(c_functions.size())) {
      for (const bool l_c_inside : {false, true}) {
        for (const int r_ab_idx : range(ab_functions.size())) {
          for (const int r_c_idx : range(c_functions.size())) {
            for (const bool r_c_inside : {false, true}) {
              const bool is_trivial =
                std::tie(l_ab_idx, l_c_idx, l_c_inside) <= std::tie(r_ab_idx, r_c_idx, r_c_inside)
              ;
              if (is_trivial) {
                continue;
              }
              const auto l_ab = ab_functions.at(l_ab_idx);
              const auto l_c = c_functions.at(l_c_idx);
              const auto r_ab = ab_functions.at(r_ab_idx);
              const auto r_c = c_functions.at(r_c_idx);
              for (const int sign : {-1, 1}) {
                const auto make_eqn = [&](const auto& x, const int n) {
                  const auto lhs = l_c_inside
                    ? l_ab(l_c(x, n), n+1)
                    : l_c(l_ab(x, n+1), n+1)
                  ;
                  const auto rhs = r_c_inside
                    ? r_ab(r_c(x, n), n+1)
                    : r_c(r_ab(x, n+1), n+1)
                  ;
                  return lhs + sign * rhs;
                };
                bool eqn_holds = true;
                for (const auto& expr : exprs_any_num_points) {
                  const int n = detect_num_variables(expr);
                  const auto eqn = make_eqn(ncoproduct(expr), n);
                  if (!eqn.is_zero()) {
                    eqn_holds = false;
                    break;
                  }
                }
                if (eqn_holds) {
                  const auto x = GammaNCoExpr().annotate("x");
                  const auto eqn = make_eqn(x, 0);
                  std::cout << annotations_one_liner(eqn.annotations()) << " == 0\n";
                }
              }
            }
          }
        }
      }
    }
  }
#endif


#if 0
  const auto cocycle = a_full(ChernCocycle(4, 1, {1,2,3,4,5}), 6);
  // const auto space = mapped(
  //   space_ncoproduct(
  //     ChernGrL(2, 2, {1,2,3,4,5,6}),
  //     GrFx(2, {1,2,3,4,5,6}),
  //     GrFx(2, {1,2,3,4,5,6})
  //   ),
  //   DISAMBIGUATE(ncomultiply)
  // );
  // const auto frozen_vars = mapped(range_incl(1, 6), [](const auto a) {
  //   return plucker({a, a % 6 + 1});
  // });
  // Gr_NCoSpace space;
  // for (const int i : range(6)) {
  //   const auto points = removed_index(seq_incl(1, 6), i);
  //   append_vector(
  //     space,
  //     space_ncoproduct(
  //       ChernGrL(2, 2, points),
  //       GrFx(2, points),
  //       frozen_vars
  //     )
  //   );
  // }
  // space = mapped(space, DISAMBIGUATE(ncomultiply));
  // const auto ranks = space_venn_ranks(space, {cocycle}, identity_function);
  // std::cout << to_string(ranks) << "\n";

  const auto chern_space = ChernGrL(2, 2, {1,2,3,4});
  const auto frozen_vars_4 = frozen_vars(4);
  const auto frozen_vars_5 = frozen_vars(5);
  // const auto frozen_vars_6 = frozen_vars(6);
  const auto frozen_vars_6 = space_ncoproduct(std::vector{
    plucker({1,2}),
    plucker({2,3}),
    // plucker({3,4}),
    plucker({4,5}),
    plucker({5,6}),
    // plucker({6,1}),
  });
  Gr_NCoSpace space;
  for (const auto& i_inner : range(5)) {
    for (const auto& i_outer : range(6)) {
      // `inner` and `outer` are effectively a_i functions.
      const auto inner = [&](const auto& expr) {
        return substitute_variables_1_based(expr, removed_index(seq_incl(1, 5), i_inner));
      };
      const auto outer = [&](const auto& expr) {
        return substitute_variables_1_based(expr, removed_index(seq_incl(1, 6), i_outer));
      };
      const auto apply_inner = [&](const auto& space) { return mapped(space, inner); };
      const auto apply_outer = [&](const auto& space) { return mapped(space, outer); };
      // append_vector(space,
      //   apply_outer(
      //     apply_inner(
      //       space_ncoproduct(
      //         chern_space,
      //         frozen_vars_4,
      //         frozen_vars_4
      //       )
      //     )
      //   )
      // );
      append_vector(space,
        apply_outer(
          space_ncoproduct(
            apply_inner(
              space_ncoproduct(
                chern_space,
                frozen_vars_4
              )
            ),
            frozen_vars_5
          )
        )
      );
      // append_vector(space,
      //   apply_outer(
      //     space_ncoproduct(
      //       apply_inner(
      //         chern_space
      //       ),
      //       frozen_vars_5,
      //       frozen_vars_5
      //     )
      //   )
      // );
      // append_vector(space,
      //   space_ncoproduct(
      //     apply_outer(
      //       apply_inner(
      //         space_ncoproduct(
      //           chern_space,
      //           frozen_vars_4
      //         )
      //       )
      //     ),
      //     frozen_vars_6
      //   )
      // );
      append_vector(space,
        space_ncoproduct(
          apply_outer(
            space_ncoproduct(
              apply_inner(
                chern_space
              ),
              frozen_vars_5
            )
          ),
          frozen_vars_6
        )
      );
      // append_vector(space,
      //   space_ncoproduct(
      //     apply_outer(
      //       apply_inner(
      //         chern_space
      //       )
      //     ),
      //     frozen_vars_6,
      //     frozen_vars_6
      //   )
      // );
    }
  }
  space = mapped(space, DISAMBIGUATE(ncomultiply));
  const auto ranks = space_venn_ranks(space, {cocycle}, identity_function);
  std::cout << to_string(ranks) << "\n";
#endif


  // const auto pl = [](auto... points) {
  //   return plucker({points...});
  // };
  // const auto cocycle = a_full(ChernCocycle(4, 1, {1,2,3,4,5}), 6);

  // std::vector space = {
  //   // first 3 term: expanded residue([t,1,2,3] ^ [t,4]/[t,3] ^ [t,5]/[t,3] ^ [t,6]/[t,3])
  //   ncoproduct(GLi2(4,1,2,3), pl(4,5) - pl(4,3), pl(4,6) - pl(4,3)),
  //   ncoproduct(GLi2(5,1,2,3), pl(5,4) - pl(5,3), pl(5,6) - pl(5,3)),
  //   ncoproduct(GLi2(6,1,2,3), pl(6,4) - pl(6,3), pl(6,5) - pl(6,3)),
  //   ncoproduct(ChernCocycle(3, 2, {1,3,4,5,6}) - ChernCocycle(3, 2, {2,3,4,5,6}), pl(1,2)),
  //   ncoproduct(ChernCocycle(3, 2, {1,3,4,5,6}), pl(1,3)),
  //   ncoproduct(ChernCocycle(3, 2, {2,3,4,5,6}), pl(2,3)),
  //   ncoproduct(GLi2(3,4,5,6), pl(1,2), pl(2,3)),
  //   ncoproduct(GLi2(3,4,5,6), pl(2,3), pl(3,1)),
  //   ncoproduct(GLi2(3,4,5,6), pl(3,1), pl(1,2)),
  // };

  // // const auto cocyle_3_2 = [](const std::vector<int>& points, const auto& rhs) {
  // //   const auto args = [&](const auto... idx) {
  // //     return choose_indices_one_based(points, {idx...});
  // //   };
  // //   return std::vector{
  // //     ncoproduct(GLiVec(2, args(1,2,3,5)), plucker(args(3,4)), rhs),
  // //     ncoproduct(GLiVec(2, args(1,2,4,5)), plucker(args(3,4)), rhs),
  // //     ncoproduct(GLiVec(2, args(1,2,3,5)), plucker(args(3,5)), rhs),
  // //     ncoproduct(GLiVec(2, args(1,2,4,5)), plucker(args(4,5)), rhs),
  // //     ncoproduct(GLiVec(2, args(1,3,4,5)), plucker(args(1,2)), rhs),
  // //     ncoproduct(GLiVec(2, args(2,3,4,5)), plucker(args(1,2)), rhs),
  // //     ncoproduct(GLiVec(2, args(1,3,4,5)), plucker(args(1,5)), rhs),
  // //     ncoproduct(GLiVec(2, args(2,3,4,5)), plucker(args(2,5)), rhs),
  // //   };
  // // };
  // // auto space = concat(
  // //   std::vector{
  // //     ncoproduct(GLi2(4,1,2,3), pl(4,5), pl(4,6)),
  // //     ncoproduct(GLi2(4,1,2,3), pl(4,3), pl(4,6)),
  // //     ncoproduct(GLi2(4,1,2,3), pl(4,5), pl(4,3)),
  // //     ncoproduct(GLi2(5,1,2,3), pl(5,4), pl(5,6)),
  // //     ncoproduct(GLi2(5,1,2,3), pl(5,3), pl(5,6)),
  // //     ncoproduct(GLi2(5,1,2,3), pl(5,4), pl(5,3)),
  // //     ncoproduct(GLi2(6,1,2,3), pl(6,4), pl(6,5)),
  // //     ncoproduct(GLi2(6,1,2,3), pl(6,3), pl(6,5)),
  // //     ncoproduct(GLi2(6,1,2,3), pl(6,4), pl(6,3)),
  // //     ncoproduct(GLi2(3,4,5,6), pl(1,2), pl(2,3)),
  // //     ncoproduct(GLi2(3,4,5,6), pl(2,3), pl(3,1)),
  // //     ncoproduct(GLi2(3,4,5,6), pl(3,1), pl(1,2)),
  // //   },
  // //   cocyle_3_2({1,3,4,5,6}, pl(1,2)),
  // //   cocyle_3_2({2,3,4,5,6}, pl(1,2)),
  // //   cocyle_3_2({1,3,4,5,6}, pl(1,3)),
  // //   cocyle_3_2({2,3,4,5,6}, pl(2,3))
  // // );

  // space = mapped(space, DISAMBIGUATE(ncomultiply));
  // const auto eqn = find_equation(cocycle, space, {0,-1,1});
  // std::cout << eqn;


  // for (const int n : range_incl(2, 6)) {
  //   for (const int m : range_incl(2, 6)) {
  //     const int w = m - 1;
  //     const auto space = mapped(
  //       nondecreasing_sequences(n, m),
  //       [](const auto& args) {
  //         return CorrVec(args);
  //       }
  //     );
  //     // const auto rank = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
  //     // std::cout << "w=" << w << ", p=" << n << ": " << rank << "\n";
  //     const bool ok = to_lyndon_basis(sum(space)).is_zero();
  //     std::cout << "w=" << w << ", p=" << n << ": " << (ok ? "OK" : "FAIL") << "\n";
  //   }
  // }

  // // std::cout << to_lyndon_basis(
  // //   + Corr(1,1,2,3)
  // //   + Corr(1,2,2,3)
  // //   + Corr(1,2,3,3)
  // // );  // ZERO
  // const std::vector space = {
  //   + Corr(1,1,1,2,3)
  //   + Corr(1,1,2,2,3)
  //   + Corr(1,1,2,3,3)
  //   + Corr(1,2,2,2,3)
  //   + Corr(1,2,2,3,3)
  //   + Corr(1,2,3,3,3)
  // };
  // const auto rank = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << rank << "\n";






#if 0
  LoopExpr loop_templates;

  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6}, {1,6,7,8,9}});
  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6,7}, {1,7,8,9}});
  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,8,9}, {4,5,6,7,8}});
  loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,9}, {5,6,7,8,9}});
  loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,8,9}, {5,6,7,8}});

  // // In Lyndon basis:
  // loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6}, {1,6,7,8,9}});
  // loop_templates += LoopExpr::single({{1,2,3,4}, {1,7,8,9}, {1,4,5,6,7}});
  // loop_templates += LoopExpr::single({{1,7,8,9}, {1,2,3,4}, {1,4,5,6,7}});
  // loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,8,9}, {4,5,6,7,8}});
  // loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,9}, {5,6,7,8,9}});
  // loop_templates -= LoopExpr::single({{1,2,3,4}, {5,6,7,8}, {1,4,5,8,9}});
  // loop_templates -= LoopExpr::single({{5,6,7,8}, {1,2,3,4}, {1,4,5,8,9}});

  auto loop_expr = loop_templates.mapped_expanding([](const Loops& loops) {
    return sum_looped_vec([&](const std::vector<int>& args) {
      return LoopExpr::single(
        mapped(loops, [&](const std::vector<int>& loop) {
          return choose_indices_one_based(args, loop);
        })
      );
    }, 9, {1,2,3,4,5,6,7,8,9}, SumSign::plus);
  });
  // loop_expr = arg9_semi_lyndon(loop_expr);
  loop_expr = to_canonical_permutation(arg9_semi_lyndon(loop_expr));


  const auto a = loop_expr_degenerate(loop_expr, {{1,3}, {2,4}});
  const auto b = loop_expr_degenerate(loop_expr, {{1,3}, {2,5}});
  const auto c = loop_expr_degenerate(loop_expr, {{1,4}, {2,5}});
  const auto d = loop_expr_degenerate(loop_expr, {{1,3}, {4,6}});  // == cycle(d, {{1,2}, {3,4}, {5,7}})
  const auto e = loop_expr_degenerate(loop_expr, {{1,3}, {4,7}});
  const auto f = loop_expr_degenerate(loop_expr, {{1,3}, {5,7}});  // == cycle(f, {{1,2}, {3,5}, {6,7}})
  const auto g = loop_expr_degenerate(loop_expr, {{1,3}, {2,6}});
  const auto h = loop_expr_degenerate(loop_expr, {{1,3}, {5,8}});
  const auto i = loop_expr_degenerate(loop_expr, {{1,4}, {2,6}});
  const auto j = loop_expr_degenerate(loop_expr, {{1,5}, {2,4}});  // == cycle(j, {{4,7}, {5,6}})
  const auto k = loop_expr_degenerate(loop_expr, {{1,6}, {2,4}});
  const auto l = loop_expr_degenerate(loop_expr, {{1,6}, {2,5}});
  const auto x = loop_expr_degenerate(loop_expr, {{1,4}, {2,7}});
  const auto y = loop_expr_degenerate(loop_expr, {{1,4}, {2,8}});
  const auto z = loop_expr_degenerate(loop_expr, {{1,4}, {5,8}});
  const auto u = loop_expr_degenerate(loop_expr, {{1,5}, {2,6}});
  const auto w = loop_expr_degenerate(loop_expr, {{1,5}, {3,7}});
  const auto q = loop_expr_degenerate(loop_expr, {{1,5}, {2,7}});  // note: this was missing originally; found by `make_degenerations`
  const auto m = loop_expr_degenerate(loop_expr, {{1,3,5}});
  const auto n = loop_expr_degenerate(loop_expr, {{1,3,6}});
  const auto o = loop_expr_degenerate(loop_expr, {{1,4,7}});

  generate_loops_names({a, b, c, d, e, f, g, h, i, j, k, l, x, y, z, u, w, m, n, o, q});

  loop_kinds.list_all_kinds(std::cout);
  std::cout << "\n";

  // DUMP(a);
  // DUMP(b);
  // DUMP(c);
  // DUMP(d);
  // DUMP(e);
  // DUMP(f);
  // DUMP(g);
  // DUMP(h);
  // DUMP(i);
  // DUMP(j);
  // DUMP(k);
  // DUMP(l);
  // DUMP(x);
  // DUMP(y);
  // DUMP(z);
  // DUMP(u);
  // DUMP(w);
  // DUMP(m);
  // DUMP(n);
  // DUMP(o);


  // const auto a1 = cycle(a, {{2,4}, {5,7}});
  // const auto v = n + m - a + a1;

  // const auto o0 =
  //   + o
  //   - cycle(m, {{2,6}, {3,5}})
  //   - cycle(m, {{3,7}, {4,6}})
  //   - cycle(m, {{2,4}, {5,7}})
  //   - a
  //   - v
  //   + cycle(a, {{2,4}, {5,7}})
  //   + cycle(v, {{2,3,4,5,6,7}})
  //   - cycle(a, {{2,3}, {4,7}, {5,6}})
  //   - cycle(v, {{2,3}, {4,7}, {5,6}})
  // ;
  // const auto m0 = m - cycle(m, {{2,6}});
  // const auto v0 = v + cycle(v, {{1,6}});

  // DUMP(a);
  // DUMP(m);
  // DUMP(n);
  // DUMP(o);
  // DUMP(a1);
  // DUMP(v);
  // DUMP(cycle(m, {{2,6}, {3,5}}));
  // DUMP(cycle(m, {{3,7}, {4,6}}));
  // DUMP(cycle(m, {{2,4}, {5,7}}));
  // DUMP(cycle(a, {{2,4}, {5,7}}));
  // DUMP(cycle(v, {{2,3,4,5,6,7}}));
  // DUMP(cycle(a, {{2,3}, {4,7}, {5,6}}));
  // DUMP(cycle(v, {{2,3}, {4,7}, {5,6}}));
  // DUMP(o0);
  // DUMP(m0);
  // DUMP(v0);

  // std::cout << o0;
  // std::cout << m0;
  // std::cout << v0;

  // std::vector<LoopExpr> space;
  // for (const auto& expr : {o0, m0}) {
  //   for (const auto& p : permutations(seq_incl(1, 7))) {
  //     space.push_back(loop_expr_substitute(expr, p));
  //   }
  // }
  // // TEST: co-dimension == 1
  // std::cout << space_rank(space, identity_function) << " (of " << space.size() << ")\n";

  // // const std::vector exprs = {a, b, c, d, e, f, g, h, i, j, k, l, x, y, z, u, w, m, n, o};
  // const std::vector exprs = {a, m, n, c, d, e, f, h, i, j, k, l, o};
  // for (const auto kind : range_incl(1, loop_kinds.total_kinds())) {
  //   const auto kind_repr = LoopExpr::single(loop_kinds.kinds().at(kind - 1).representative);
  //   std::vector<LoopExpr> space_a;
  //   std::vector<LoopExpr> space_b;
  //   for (const auto& perm : permutations(seq_incl(1, 7))) {
  //     for (const auto& expr : exprs) {
  //       space_a.push_back(loop_expr_substitute(expr, perm));
  //     }
  //     space_b.push_back(loop_expr_substitute(kind_repr, perm));
  //   }
  //   std::cout << pretty_print_loop_kind_index(kind, true) << ": "
  //     << to_string(space_venn_ranks(space_a, space_b, identity_function)) << "\n";
  // }

  // Based on: [2,1,4,3][2,1,5,4][2,1,5,7,6]
  const auto kind1_sum = loop_expr_expand_len_6_loop_into_sum({{2,1,4,3}, {2,1,5,4}, {2,1,5,7,6,4}});
  // Based on: [1,2,5,4][1,6,3,5][1,6,3,2,7]
  const auto kind4_sum = loop_expr_expand_len_6_loop_into_sum({{1,2,5,4}, {1,6,3,5}, {1,6,3,2,7,4}});
  // Based on: [1,2,7,3][1,6,7,3][1,6,2,5,4]
  const auto kind6_sum = loop_expr_expand_len_6_loop_into_sum({{1,2,7,3}, {1,6,7,3}, {1,6,2,5,4,3}});

  // // Note. This computation only makes sense without Lyndon.
  // const auto perms = to_vector(permutations(seq_incl(1, 7)));
  // // const std::vector exprs = {a, b, c, d, e, f, g, h, i, j, k, l, x, y, z, u, w, m, n, o};
  // // const std::vector exprs = {a, m, n, c, d, e, f, h, i, j, k, l, o};
  // const std::vector exprs = {a, m, n, c, d, e, f, h, i, j, k, l, o, kind1_sum, kind4_sum, kind6_sum};
  // // for (const auto kind : range_incl(1, loop_kinds.total_kinds())) {
  // for (const auto kind : {app_arg}) {
  //   const auto kind_repr = LoopExpr::single(loop_kinds.kinds().at(kind - 1).representative);
  //   const std::vector<LoopExpr> space_a = flatten(mapped_parallel(perms, [&](const auto& perm) {
  //     return mapped(exprs, [&](const auto& expr) {
  //       return loop_expr_substitute(expr, perm);
  //     });
  //   }));
  //   const std::vector<LoopExpr> space_b = mapped_parallel(perms, [&](const auto& perm) {
  //     return loop_expr_substitute(kind_repr, perm);
  //   });
  //   std::cout << pretty_print_loop_kind_index(kind, true) << ": "
  //     << to_string(space_venn_ranks(space_a, space_b, DISAMBIGUATE(to_lyndon_basis))) << "\n";
  // }
#endif



#if 1
  const int N = 11;
  const int num_points = N;
  auto source = sum_looped_vec(
    [&](const std::vector<int>& args) {
      return LiQuad(num_points / 2 - 1, args);
    },
    num_points,
    seq_incl(1, num_points - 1)
  );

  auto lira_expr = theta_expr_to_lira_expr_without_products(source.without_annotations());

  auto loop_expr = reverse_loops(cut_loops(seq_incl(1, num_points)));
  auto loop_lira_expr = loop_expr_to_lira_expr(loop_expr);

  lira_expr = lira_expr_sort_args(lira_expr);
  loop_lira_expr = lira_expr_sort_args(loop_lira_expr);


  // std::cout << "Via LiQuad " << lira_expr;
  // std::cout << "Loops " << loop_expr_recursive;
  // std::cout << "Via loops " << loop_lira_expr;
  // std::cout << "Diff " << to_lyndon_basis(lira_expr + loop_lira_expr);

  // All possible ways of gluing 4 points together. Gives terms {1}-{6}.
  const auto a = loop_expr_degenerate(loop_expr, {{1,3,5,7}});
  const auto b = loop_expr_degenerate(loop_expr, {{1,3,5,8}});
  const auto c = loop_expr_degenerate(loop_expr, {{1,3,6,8}});  // == expr(a, b)
  const auto d = loop_expr_degenerate(loop_expr, {{1,3,6,9}});  // == expr(a, b, e)
  // const auto e = loop_expr_degenerate(loop_expr, {{1,3,7,9}});  // == `c` under dihedral symmetry

  // All expressions limited to terms {1}-{6}.
  const auto f = loop_expr_degenerate(loop_expr, {{1,3,5}, {2,4}});
  const auto g = loop_expr_degenerate(loop_expr, {{1,3,5}, {2,11}});
  const auto fg = loop_expr_degenerate(loop_expr, {{1,3,6}, {2,4}});  // == -(f1 + f2)
  const auto h = loop_expr_degenerate(loop_expr, {{1,4,7}, {3,5}});  // == expr(f, g, a)
  const auto gg = loop_expr_degenerate(loop_expr, {{2,5}, {1,3}, {4,6}});  // ==  g(symmetry: (2,3)(4,8)(5,7) + rotate 7 positions)
  const auto m = loop_expr_degenerate(loop_expr, {{2,6}, {1,3}, {5,7}});
  const auto n = loop_expr_degenerate(loop_expr, {{2,7}, {1,3}, {6,8}});

  // All expressions that have a common variable in each term. Gives terms {1}-{7}.
  const auto p = loop_expr_degenerate(loop_expr, {{1,3,6}, {2,11}});  // == -r + f
  const auto q = loop_expr_degenerate(loop_expr, {{1,3,6}, {5,7}});
  const auto r = loop_expr_degenerate(loop_expr, {{1,3,7}, {2,4}});
  const auto s = loop_expr_degenerate(loop_expr, {{1,3,7}, {2,11}});
  const auto t = loop_expr_degenerate(loop_expr, {{1,3,7}, {6,8}});
  const auto u = loop_expr_degenerate(loop_expr, {{1,4,7}, {2,11}});
  const auto v = loop_expr_degenerate(loop_expr, {{1,4,8}, {2,11}});
  const auto w = loop_expr_degenerate(loop_expr, {{1,4,8}, {7,9}});

  // TODO: Generate all degenerations
  //    + find simple ones (e.g. little terms)
  //    + compute stats (how widespread are different loop kinds, for example)

  generate_loops_names({a, b, c, d, r});
  generate_loops_names({f, g, fg, h, gg, m, n, p, q, s, t, u, v, w});

  // Converts between {5}+{6} and {8}+{8} pairs.
  const auto sh =
    - LoopExpr::single({{1,5,3,4}, {1,7,5,2,3}, {1,7,2,8}, {1,7,5,6}})
    - LoopExpr::single({{1,5,3,4}, {1,7,5,2,3}, {1,7,5,6}, {1,7,2,8}})
    + LoopExpr::single({{1,7,5,6}, {1,7,5,2,3}, {1,7,2,8}, {1,5,3,4}})
    + LoopExpr::single({{1,7,5,6}, {1,7,5,2,3}, {1,5,3,4}, {1,7,2,8}})
  ;

  // loop_kinds.list_all_kinds(std::cout);
  // std::cout << "\n";

  // DUMP(b);
  // DUMP(g);
  // DUMP(h);
  // DUMP(m);
  // DUMP(n);
  // DUMP(q);
  // DUMP(r);
  // DUMP(s);
  // DUMP(t);
  // DUMP(w);
  // DUMP(sh);

  // // This gives the same space rank as all variables. And it's full: 154.
  // std::vector exprs = {b, g, h, m, n, q, r, s, t, w, sh};
  // // Note. It also makes sense to keep an eye on `a` and `f`, because they are neat.
  // std::vector<LoopExpr> space;
  // for (const int shift : range(7)) {
  //   for (const auto& expr : exprs) {
  //     space.push_back(loop_expr_substitute(expr, concat({1}, rotated_vector(seq_incl(2, 8), shift))));
  //     space.push_back(loop_expr_substitute(expr, concat({1}, rotated_vector(reversed(seq_incl(2, 8)), shift))));
  //   }
  // }
  // std::cout << space_rank(space, identity_function) << "\n";

  // Kill {8} terms with `sh`:
  const auto ssh = s + cycle(sh, {{6,8},{3,4},{2,5}});
  const auto wsh = w + cycle(sh, {{6,3},{4,5},{7,8}}) - cycle(sh, {{6,4},{3,2,7}});

  const auto qb = q + cycle(b, {{3,6,4,5,2,7}});
  const auto rb = r + cycle(b, {{3,7,4,8,5,2,6}});
  const auto sshb = ssh + cycle(b, {{3,7,4,8,5,2,6}}) - cycle(b, {{3,4},{6,8},{5,2}});
  const auto tb = t + cycle(b, {{3,5},{6,8}});
  const auto wshb = wsh + cycle(b, {{3,2,7},{6,4}}) - cycle(b, {{3,6},{5,4},{7,8}});

  const auto qbt = qb - cycle(tb, {{4,7,2,6,5,8}}) - cycle(tb, {{4,5,6,8},{2,7,3}});
  const auto rbt = rb + cycle(tb, {{4,7},{8,5},{6,3}}) + cycle(tb, {{4,5,6,8,3},{2,7}});
  const auto sshbt = sshb + cycle(tb, {{4,7},{8,5},{6,3}}) + cycle(tb, {{4,5,6,8,3},{2,7}}) + cycle(tb, {{4,3,2,5}});
  const auto wshbt = wshb  + cycle(tb, {{4,5,6,7,8,3}}) - cycle(tb, {{4,7,5,8,6},{2,3}});

  const auto hg = h - cycle(g, {{6,7,8,3,4,5}}) - cycle(g, {{8,4},{5,7}});
  const auto mg = m - cycle(g, {{6,4,8,5,2},{3,7}}) - cycle(g, {{6,4,5,3},{2,7}});
  const auto ng = n + cycle(g, {{3,4,5}}) + cycle(g, {{3,2,4,8,5,7}}) + cycle(g, {{6,8,4},{2,5,7}}) + cycle(g, {{6,8},{3,5,2}});
  const auto qbtg = qbt - cycle(g, {{6,4,8,5,3,7,2}}) - cycle(g, {{6,4,7,2},{3,8,5}}) - cycle(g, {{6,5,2,7}}) - cycle(g, {{6,8},{3,2,5}}) - cycle(g, {{6,4,5,2,7,3}});
  const auto rbtg = rbt + cycle(g, {{6,8},{3,4},{2,5}}) + cycle(g, {{6,3},{2,7},{5,4}}) + cycle(g, {{6,5},{3,8},{4,7}}) - g;
  const auto sshbtg = sshbt + cycle(g, {{6,8},{3,4},{2,5}}) + cycle(g, {{6,5},{3,8},{4,7}}) + cycle(g, {{6,3},{2,7},{5,4}}) + cycle(g, {{6,8,3,5,7,2,4}}) + cycle(g, {{3,2},{8,4},{5,7}}) + cycle(g, {{6,8},{3,4},{2,5}});
  const auto wshbtg = wshbt - cycle(g, {{6,4,8},{3,7,5}}) - cycle(g, {{2,8,7}}) - cycle(g, {{6,4},{3,2,7}}) - cycle(g, {{3,8,4,7,5,2}});

  const auto hga = hg - cycle(a, {{3,2}});
  const auto qbtga = qbtg - cycle(a, {{4,2,3}});
  const auto rbtga = rbtg + cycle(a, {{7,4},{6,5},{3,8}});
  const auto sshbtga = sshbtg + cycle(a, {{7,4},{6,5},{3,8}});
  const auto wshbtga = wshbtg - cycle(a, {{7,5,2,3,8,4}}) + cycle(a, {{7,6,2,4,8,5}});

  const auto hgaf = hga - cycle(f, {{6,7,8,3,4,5}});  // ZERO
  const auto mgf = mg + cycle(f, {{6,5},{3,8,4,7}}) + cycle(f, {{2,3,5,7},{6,8,4}});
  const auto ngf = ng - cycle(f, {{2,7,3,8,6,5,4}});
  const auto qbtgaf = qbtga + cycle(f, {{2,3,4}});
  const auto rbtgaf = rbtga - cycle(f, {{6,5},{3,8},{7,4}});
  const auto sshbtgaf = sshbtga - cycle(f, {{2,7,5,3,8,6,4}});
  const auto wshbtgaf = wshbtga + cycle(f, {{2,5,7,3,6,8,4}});

  // DUMP(mgf);
  // DUMP(ngf);
  // DUMP(qbtgaf);
  // DUMP(rbtgaf);
  // DUMP(sshbtgaf);
  // DUMP(wshbtgaf);

  // std::cout << qbtgaf - cycle(qbtgaf, {{1,2}});  // kills {7}; left: {1},{2},{3}
  // std::cout << qbtgaf - cycle(qbtgaf, {{7,8}});  // kills {7}; left: {1},{2},{3}
  const auto sr = sshbtgaf - rbtgaf;  // kills {1}; left: {2},{3}

  // // FACT: {wshbtgaf} space == {mgf, ngf, wshbtgaf, sr} space under permutations of 2..=8
  // const std::vector exprs = {wshbtgaf};
  // std::vector<LoopExpr> space;
  // for (const auto& perm : permutations(seq_incl(2, 8))) {
  //   for (const auto& expr : exprs) {
  //     space.push_back(loop_expr_substitute(expr, concat({1}, perm)));
  //   }
  // }
  // std::cout << space_rank(space, identity_function) << " (of " << space.size() << ")\n";

  // const auto perms = to_vector(permutations(seq_incl(1, 8)));
  // const std::vector exprs = {wshbtgaf, qbtgaf};
  // // const std::vector exprs = {wshbtgaf, qbtgaf, rbtgaf, sshbtgaf};
  // const auto kind_repr = LoopExpr::single(loop_kinds.kinds().at(2 - 1).representative);
  // const std::vector<LoopExpr> space_a = flatten(mapped_parallel(perms, [&](const auto& perm) {
  //   return mapped(exprs, [&](const auto& expr) {
  //     return loop_expr_substitute(expr, perm);
  //   });
  // }));
  // const std::vector<LoopExpr> space_b = mapped_parallel(perms, [&](const auto& perm) {
  //   return loop_expr_substitute(kind_repr, perm);
  // });
  // std::cout << to_string(space_venn_ranks(space_a, space_b, identity_function)) << "\n";

  // const auto perms = to_vector(permutations(seq_incl(1, 8)));
  // const std::vector exprs = {b, g, h, m, n, q, r, s, t, w, sh};
  // const auto kinds56 =
  //   - LoopExpr::single({{1,5,3,4}, {1,7,5,2,3}, {1,7,2,8}, {1,7,5,6}})
  //   - LoopExpr::single({{1,5,3,4}, {1,7,5,2,3}, {1,7,5,6}, {1,7,2,8}})
  // ;
  // const auto kinds99 =
  //   + LoopExpr::single({{1,2,3,5,4}, {1,7,2,3}, {1,7,3,8}, {1,7,2,6}})
  //   + LoopExpr::single({{1,3,2,5,4}, {1,7,3,2}, {1,7,2,6}, {1,7,3,8}})
  // ;
  // const std::vector kind_exprs = {kinds56, kinds99};
  // for (const auto& kind_repr : kind_exprs) {
  // // for (const int kind : range_incl(1, loop_kinds.total_kinds())) {
  // //   const auto kind_repr = LoopExpr::single(loop_kinds.kinds().at(kind - 1).representative);
  //   Profiler profiler;
  //   const std::vector<LoopExpr> space_a = flatten(mapped_parallel(perms, [&](const auto& perm) {
  //     return mapped(exprs, [&](const auto& expr) {
  //       return loop_expr_substitute(expr, perm);
  //     });
  //   }));
  //   const std::vector<LoopExpr> space_b = mapped_parallel(perms, [&](const auto& perm) {
  //     return loop_expr_substitute(kind_repr, perm);
  //   });
  //   profiler.finish("exprs");
  //   const auto ranks = space_venn_ranks(space_a, space_b, identity_function);
  //   profiler.finish("ranks");
  //   // std::cout << pretty_print_loop_kind_index(kind, true) << ": " << to_string(ranks) << "\n";
  //   std::cout << to_string(ranks) << "\n";
  // }

  // const std::vector exprs = {hg, mg, ng, qbtg, rbtg, sshbtg, wshbtg};
  // std::vector<LoopExpr> space;
  // for (const auto& perm : permutations(seq_incl(2, 8))) {
  //   // for (const auto& expr : exprs) {
  //   //   space.push_back(loop_expr_substitute(expr, concat({1}, perm)));
  //   // }
  //   for (const auto& kind : {1,2,3,7}) {
  //     const auto expr = LoopExpr::single(loop_kinds.kinds().at(kind - 1).representative);
  //     space.push_back(loop_expr_substitute(expr, concat({1}, perm)));
  //   }
  // }
  // std::cout << space_rank(space, identity_function) << " (of " << space.size() << ")\n";

  // Apply (abcd + abdc = dbca + dbac) to convert {5}+{6} into {8}+{8}
  // const auto e56 =
  //   - LoopExpr::single({{1,5,3,4}, {1,7,5,2,3}, {1,7,2,8}, {1,7,5,6}})
  //   - LoopExpr::single({{1,5,3,4}, {1,7,5,2,3}, {1,7,5,6}, {1,7,2,8}})
  // ;
  // const auto e56_shuffled =
  //   - LoopExpr::single({{1,7,5,6}, {1,7,5,2,3}, {1,7,2,8}, {1,5,3,4}})
  //   - LoopExpr::single({{1,7,5,6}, {1,7,5,2,3}, {1,5,3,4}, {1,7,2,8}})
  // ;
  // CHECK(to_lyndon_basis(e56) == to_lyndon_basis(e56_shuffled));
  // DUMP(e56);
  // DUMP(e56_shuffled);

  // std::vector<LoopExpr> space;
  // for (const auto& expr : {a}) {
  //   for (const auto& p : permutations(seq_incl(1, 8))) {
  //     space.push_back(loop_expr_substitute(expr, p));
  //   }
  // }
  // std::cout << space_rank(space, identity_function) << " (of " << space.size() << ")\n";

  const auto degenrations = make_degenerations(11, 3);
  for (const auto& degenration : degenrations) {
    std::cout << dump_to_string(degenration) << " ";
    const auto expr = loop_expr_degenerate(loop_expr, degenration);
    std::cout << expr;
  }
  std::cout << "\n";
  std::cout << "total degenerations = " << degenrations.size() << "\n";
  std::cout << "\n";
  loop_kinds.list_all_kinds(std::cout);
#endif
}
