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


static constexpr auto cycle = loop_expr_cycle;

int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    .set_unicode_version(UnicodeVersion::simple)
    .set_expression_line_limit(300)
    .set_annotation_sorting(AnnotationSorting::lexicographic)
    .set_compact_x(true)
    .set_max_terms_in_annotations_one_liner(100)
  );


#if 1
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

  // DUMP_EXPR(a);
  // DUMP_EXPR(b);
  // DUMP_EXPR(c);
  // DUMP_EXPR(d);
  // DUMP_EXPR(e);
  // DUMP_EXPR(f);
  // DUMP_EXPR(g);
  // DUMP_EXPR(h);
  // DUMP_EXPR(i);
  // DUMP_EXPR(j);
  // DUMP_EXPR(k);
  // DUMP_EXPR(l);
  // DUMP_EXPR(x);
  // DUMP_EXPR(y);
  // DUMP_EXPR(z);
  // DUMP_EXPR(u);
  // DUMP_EXPR(w);
  // DUMP_EXPR(m);
  // DUMP_EXPR(n);
  // DUMP_EXPR(o);


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

  // DUMP_EXPR(a);
  // DUMP_EXPR(m);
  // DUMP_EXPR(n);
  // DUMP_EXPR(o);
  // DUMP_EXPR(a1);
  // DUMP_EXPR(v);
  // DUMP_EXPR(cycle(m, {{2,6}, {3,5}}));
  // DUMP_EXPR(cycle(m, {{3,7}, {4,6}}));
  // DUMP_EXPR(cycle(m, {{2,4}, {5,7}}));
  // DUMP_EXPR(cycle(a, {{2,4}, {5,7}}));
  // DUMP_EXPR(cycle(v, {{2,3,4,5,6,7}}));
  // DUMP_EXPR(cycle(a, {{2,3}, {4,7}, {5,6}}));
  // DUMP_EXPR(cycle(v, {{2,3}, {4,7}, {5,6}}));
  // DUMP_EXPR(o0);
  // DUMP_EXPR(m0);
  // DUMP_EXPR(v0);

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
  // std::cout << space_rank(space, std::identity{}) << " (of " << space.size() << ")\n";

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
  //     << to_string(space_venn_ranks(space_a, space_b, std::identity{})) << "\n";
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



#if 0
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

  // DUMP_EXPR(b);
  // DUMP_EXPR(g);
  // DUMP_EXPR(h);
  // DUMP_EXPR(m);
  // DUMP_EXPR(n);
  // DUMP_EXPR(q);
  // DUMP_EXPR(r);
  // DUMP_EXPR(s);
  // DUMP_EXPR(t);
  // DUMP_EXPR(w);
  // DUMP_EXPR(sh);

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
  // std::cout << space_rank(space, std::identity{}) << "\n";

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

  // DUMP_EXPR(mgf);
  // DUMP_EXPR(ngf);
  // DUMP_EXPR(qbtgaf);
  // DUMP_EXPR(rbtgaf);
  // DUMP_EXPR(sshbtgaf);
  // DUMP_EXPR(wshbtgaf);

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
  // std::cout << space_rank(space, std::identity{}) << " (of " << space.size() << ")\n";

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
  // std::cout << to_string(space_venn_ranks(space_a, space_b, std::identity{})) << "\n";

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
  //   const auto ranks = space_venn_ranks(space_a, space_b, std::identity{});
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
  // std::cout << space_rank(space, std::identity{}) << " (of " << space.size() << ")\n";

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
  // DUMP_EXPR(e56);
  // DUMP_EXPR(e56_shuffled);

  // std::vector<LoopExpr> space;
  // for (const auto& expr : {a}) {
  //   for (const auto& p : permutations(seq_incl(1, 8))) {
  //     space.push_back(loop_expr_substitute(expr, p));
  //   }
  // }
  // std::cout << space_rank(space, std::identity{}) << " (of " << space.size() << ")\n";

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



#if 0
  const int num_points = 7;
  const int dimension = 3;
  const auto points = seq_incl(1, num_points);
  const auto fx_prime = space_ncoproduct(mapped(
    combinations(seq_incl(1, num_points - 1), dimension - 1),
    [](const auto& args) {
      return plucker(concat(args, {num_points}));
    }
  ));
  // const auto fx = GrFx(dimension, points);
  // const auto l2 = GrL2(dimension, points);
  // const auto space_a = space_ncoproduct(fx, fx, fx);
  // // const auto space_a = space_ncoproduct(fx, fx, fx_prime);
  // // const auto space_a = space_ncoproduct(fx, fx_prime, fx_prime);
  // // const auto space_a = space_ncoproduct(fx_prime, fx_prime, fx_prime);
  // const auto space_b = mapped(
  //   space_ncoproduct(l2, fx),
  //   DISAMBIGUATE(ncomultiply)
  // );
  // const auto ranks = space_venn_ranks(space_a, space_b, std::identity{});
  // std::cout << to_string(ranks) << "\n";

  const auto fx = space_ncoproduct(GrFx(dimension, points));
  const auto l2 = space_ncoproduct(GrL2(dimension, points));
  for (const int weight : range_incl(2, 10)) {
    Profiler profiler(true);
    const auto space_a = mapped_parallel(combinations(fx, weight), DISAMBIGUATE(ncoproduct));
    const auto space_b = mapped_parallel(cartesian_combinations(std::vector{
      std::pair{l2, 1},
      std::pair{fx, weight - 2},
    }), [](const auto& exprs) {
      return ncomultiply(ncoproduct(exprs));
    });
    profiler.finish("spaces");
    const auto ranks = space_venn_ranks(space_a, space_b, std::identity{});
    profiler.finish("ranks");
    std::cout << "w=" << weight << ": " << to_string(ranks) << "\n";
  }

  // const auto fx = space_ncoproduct(GrFx(dimension, points));
  // const auto l2 = space_ncoproduct(GrL2(dimension, points));
  // const int weight = 5;
  // const int num_prime = app_arg;
  // // for (const int num_prime : range_incl(0, weight)) {
  //   Profiler profiler(false);
  //   const auto space_a = mapped_parallel(cartesian_combinations(std::vector{
  //     std::pair{fx, weight - num_prime},
  //     std::pair{fx_prime, num_prime},
  //   }), DISAMBIGUATE(ncoproduct));
  //   const auto space_b = mapped_parallel(cartesian_combinations(std::vector{
  //     std::pair{l2, 1},
  //     std::pair{fx, weight - 2},
  //   }), [](const auto& exprs) {
  //     return ncomultiply(ncoproduct(exprs));
  //   });
  //   profiler.finish("spaces");
  //   const auto ranks = space_venn_ranks(space_a, space_b, std::identity{});
  //   profiler.finish("ranks");
  //   std::cout << "w=" << weight << ", #f'=" << num_prime << ": " << to_string(ranks) << "\n";
  // // }


  // // Generalized Arnold's relationship.
  // const int num_points = 5;
  // const int dimension = 3;
  // const auto points = seq_incl(1, num_points);
  // const auto fx = GrFx(dimension, points);
  // const auto l2_prime = mapped(
  //   to_vector(permutations(points)),
  //   [](const auto& args) {
  //     const auto pl = [&](const std::vector<int>& subargs) {
  //       return plucker(choose_indices_one_based(args, subargs));
  //     };
  //     return
  //       + ncoproduct(pl({1,2,3}), pl({1,2,4}))
  //       + ncoproduct(pl({1,2,4}), pl({1,3,4}))
  //       + ncoproduct(pl({1,3,4}), pl({1,2,3}))
  //     ;
  //   }
  // );
  // const auto space_a = space_ncoproduct(fx, fx, fx);
  // const auto space_b = space_ncoproduct(l2_prime, fx);
  // const auto ranks = space_venn_ranks(space_a, space_b, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // const int num_points = 5;
  // const int dimension = 3;
  // const auto points = seq_incl(1, num_points);
  // const auto fx = GrFx(dimension, points);
  // const auto l2_prime = mapped(
  //   to_vector(permutations(points)),
  //   [](const auto& args) {
  //     const auto pl = [&](const std::vector<int>& subargs) {
  //       return plucker(choose_indices_one_based(args, subargs));
  //     };
  //     return
  //       + ncoproduct(pl({1,2,3}), pl({1,2,4}))
  //       + ncoproduct(pl({1,2,4}), pl({1,3,4}))
  //       + ncoproduct(pl({1,3,4}), pl({1,2,3}))
  //     ;
  //   }
  // );
  // // Basis:
  // const std::vector space_a = {
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({1,2,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({1,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({1,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({2,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({3,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({1,2,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({1,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({1,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({2,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({3,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({1,2,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({1,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({1,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({2,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({3,4,5})),

  //   ncoproduct(plucker({1,2,3}), plucker({1,2,5}), plucker({3,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,5}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,4,5}), plucker({2,3,5})),
  // };
  // const auto space_b = space_ncoproduct(l2_prime, fx);
  // const auto ranks = space_venn_ranks(space_a, space_b, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // for (const int weight : range_incl(2, 3)) {
  //   for (const int dimension : range_incl(2, 3)) {
  //     for (const int num_points : range_incl(7, 9)) {
  //       Profiler profiler(true);
  //       const auto points = seq_incl(1, num_points);
  //       const auto fx = space_ncoproduct(GrFx(dimension, points));
  //       const auto space_a = mapped(
  //         combinations(fx, weight),
  //         DISAMBIGUATE(ncoproduct)
  //       );
  //       const auto space_b = mapped(
  //         cartesian_combinations(std::vector{
  //           std::pair{GrLArnold2(dimension, points), 1},
  //           std::pair{fx, weight - 2},
  //         }),
  //         DISAMBIGUATE(ncoproduct)
  //       );
  //       profiler.finish("spaces");
  //       const auto ranks = space_venn_ranks(space_a, space_b, std::identity{});
  //       profiler.finish("ranks");
  //       const int result = ranks.a() - ranks.intersected();
  //       std::cout << "w=" << weight << ", d=" << dimension << ", p=" << num_points << ": " << result << "\n";
  //     }
  //   }
  // }
#endif
}
