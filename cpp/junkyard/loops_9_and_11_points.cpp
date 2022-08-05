#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/loops.h"
#include "lib/loops_aux.h"
#include "lib/lyndon.h"
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
#include "lib/zip.h"


static constexpr auto cycle = loop_expr_cycle;


void loops_args11() {
  const int N = 11;
  const int num_points = N;
  auto source = sum_looped_vec(
    [&](const std::vector<X>& args) {
      return LiQuad(
        num_points / 2 - 1,
        mapped(args, [](X x) { return x.var(); })
      );
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
  const auto e = loop_expr_degenerate(loop_expr, {{1,3,7,9}});

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

  generate_loops_names({a, b, c, d, e, r});

  // list_all_degenerations(loop_expr);
  // return 0;

  // const auto a_c = to_canonical_permutation(a);
  // const auto b_c = to_canonical_permutation(b);
  // const auto c_c = to_canonical_permutation(c);
  // const auto d_c = to_canonical_permutation(d);
  // const auto e_c = to_canonical_permutation(e);

  // std::cout << loop_expr_degenerate(loop_expr, {{1,3}, {2,5}, {4,6}});

  // std::cout << "a " << a << fmt::newline();
  // std::cout << "b " << b << fmt::newline();
  // std::cout << "c " << c << fmt::newline();
  // std::cout << "d " << d << fmt::newline();
  // std::cout << "e " << e << fmt::newline();

  std::cout << "a " << preshow(a) << fmt::newline();
  std::cout << "b " << preshow(b) << fmt::newline();
  // std::cout << "c " << preshow(c) << fmt::newline();
  // std::cout << "d " << preshow(d) << fmt::newline();
  std::cout << "e " << preshow(e) << fmt::newline();
  std::cout << "---\n\n";
  std::cout << "f " << preshow(f) << fmt::newline();
  std::cout << "g " << preshow(g) << fmt::newline();
  // std::cout << "h " << preshow(h) << fmt::newline();
  std::cout << "---\n\n";
  std::cout << "m " << preshow(m) << fmt::newline();
  std::cout << "n " << preshow(n) << fmt::newline();
  std::cout << "---\n\n";
  // std::cout << "p " << preshow(p) << fmt::newline();
  std::cout << "q " << preshow(q) << fmt::newline();
  std::cout << "r " << preshow(r) << fmt::newline();
  std::cout << "s " << preshow(s) << fmt::newline();
  std::cout << "t " << preshow(t) << fmt::newline();
  std::cout << "u " << preshow(u) << fmt::newline();
  std::cout << "v " << preshow(v) << fmt::newline();
  std::cout << "w " << preshow(w) << fmt::newline();

  std::cout << "===\n\n";

  const auto qr = q - cycle(r, {{3,4}, {5,8}, {6,7}});  // without {7}

  // LEGACY TERM NUMBEERS
  // const auto mf = auto_kill(m, f, 6);  // {1,2,4}
  // const auto mfa = auto_kill(mf, a, 1);  // {2,4}
  // const auto mfg = auto_kill(mf, g, 4);  // {2,3}
  // const auto mfg1 = mfg + cycle(mfg, {{3,5}});
  // const auto mfg2 = mfg1 + cycle(mfg1, {{2,8}});  // only {2}
  // const auto nf = auto_kill(n, f, 6);  // {1,2,3,4}
  // const auto nfg = auto_kill(nf, g, 4);  // {2,3}
  // const auto nfg1 = nfg + cycle(nfg, {{3,5}});
  // const auto nfg2 = nfg1 + cycle(nfg1, {{2,4}});  // only {2}
  // std::cout << "\n";
  // std::cout << mfg;
  // std::cout << nfg;
  // std::cout << arg11_expr_type_2_to_column(mfg2);
  // std::cout << arg11_expr_type_2_to_column(nfg2);




  const auto mf = auto_kill(m, f, 7);  // {1,2,4}
  const auto mfa = auto_kill(mf, a, 1);  // {2,4}
  const auto mfg = auto_kill(mf, g, 4);  // {2,3}
  const auto mfg1 = mfg + cycle(mfg, {{3,5}});
  const auto mfg2 = mfg1 + cycle(mfg1, {{2,8}});  // only {2}

  const auto nf = auto_kill(n, f, 7);  // {1,2,3,4}
  const auto nfg = auto_kill(nf, g, 4);  // {2,3}
  const auto nfg1 = nfg + cycle(nfg, {{3,5}});
  const auto nfg2 = nfg1 + cycle(nfg1, {{2,4}});  // only {2}

  std::cout << "\n";
  std::cout << mfg;
  std::cout << nfg;
  std::cout << arg11_expr_type_2_to_column(mfg2);
  std::cout << arg11_expr_type_2_to_column(nfg2);



  // const auto hf = h - cycle(f, {{3,4,5,6,7,8}});
  // ZERO:  hf - cycle(g, {{4,8}, {5,7}}) - cycle(g, {{3,4,5,6,7,8}}) - cycle(a, {{2,3}})

  // const auto cb =  // {1,2};  but then we discarded c
  //   + c
  //   + b
  //   + cycle(b, {{2,5}, {3,4}, {6,8}})
  // ;

  // ZERO:  cb + a + cycle(a, {{2,5}, {3,4}, {6,8}})

  const auto db =  // {1,2,4}
    + d
    - b
    - cycle(b, {{2,5}, {3,4}, {6,8}})
    + cycle(b, {{2,7}, {3,6}, {4,5}})
    - cycle(b, {{3,8}, {4,7}, {5,6}})
  ;

  // const auto eb = auto_kill_planar(e, b, 7);
  const auto eb =
    + e
    - cycle(b, {{2,7}, {3,6}, {4,5}})
    + cycle(b, {{3,8}, {4,7}, {5,6}})
  ;

  const auto eba = auto_kill_planar(eb, a, 1);
  // const auto eba1 = eba + cycle(eba, {{4,5}});
  // const auto eba2 = eba1 + cycle(eba1, {{2,8}});  // BEFORE: just one variable (how?)

  std::cout << eb;
  std::cout << eba;
  // std::cout << eba1;
  // std::cout << eba2;
  // std::cout << arg11_expr_type_2_to_column(eba2);

  const auto dbe =  // {1,2}
    + db
    + eb
    + cycle(eb, {{2,3}, {4,8}, {5,7}})
  ;

  // ZERO:  + dbe - a + cycle(a, {{2,6}, {3,5}, {7,8}})

  const auto a1 = a - cycle(a, {{4,6}});
  const auto a2 = a1 - cycle(a1, {{2,7}});
  // std::cout << "a " << a;
  // std::cout << a1;
  // std::cout << a2;
  // std::cout << arg11_expr_type_2_to_column(a2);

  // const auto df =
  //   + d
  //   - f1
  //   + cycle(f1, {{2,4}, {5,8}, {6,7}})
  //   + cycle(f1, {{2,7}, {3,6}, {4,5}})
  // ;
  // const auto df_1 =
  //   + df
  //   - cycle(df, {{1,5}})
  // ;

  // const auto eb1 =
  //   + eb
  //   + cycle(eb, {{2,8}})
  // ;
  // const auto eb2 =  // {1,2}
  //   + eb1
  //   + cycle(eb1, {{4,5}})
  // ;
  // const auto eb3 =  // {1,2}
  //   + eb
  //   - cycle(eb, {{2,4}, {5,8}, {6,7}})
  // ;
  // std::cout << preshow(
  //   + eb2
  //   + a
  //   // + cycle(a, {{2,4}, {5,8}, {6,7}})  // bad
  //   // + cycle(a, {{2,5}, {3,4}, {6,8}})  // bad
  //   // + cycle(a, {{2,6}, {3,5}, {7,8}})  // bad
  //   - cycle(a, {{2,7}, {3,6}, {4,5}})
  //   + cycle(a, {{3,8}, {4,7}, {5,6}})
  //   // + cycle(a, {{2,3}, {4,8}, {5,7}})  // bad
  //   // + cycle(a, {{2,8}, {3,7}, {4,6}})  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 1)  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 2)  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 3)  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 4)  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 5)  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 6)  // bad
  //   // + cycle_pow(a, {{2,3,4,5,6,7,8}}, 7)  // bad
  // );

  // std::cout << preshow(eb);

  // std::cout << preshow(
  //   + eb
  //   // + cycle(eb, {{2,8}, {3,7}, {4,6}})
  // );

  // std::cout << preshow(
  //   + eb3
  //   // + a
  //   // + cycle(a, {{2,3}, {4,8}, {5,7}})
  //   // + cycle(a, {{2,4}, {5,8}, {6,7}})
  //   // + cycle(a, {{2,5}, {3,4}, {6,8}})
  //   - cycle(a, {{2,6}, {3,5}, {7,8}})
  //   - cycle(a, {{2,7}, {3,6}, {4,5}})
  //   + cycle(a, {{2,8}, {3,7}, {4,6}})
  //   + cycle(a, {{3,8}, {4,7}, {5,6}})
  // );
}


void loops_args9() {
  static const int N = 9;
  LoopExpr loop_templates;

  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6}, {1,6,7,8,9}});
  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6,7}, {1,7,8,9}});
  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,8,9}, {4,5,6,7,8}});
  loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,9}, {5,6,7,8,9}});
  loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,8,9}, {5,6,7,8}});

  // In Lyndon basis:
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

#if 0
  LoopExpr loop_templates2;
  for (int i : range(7)) {
    const auto seven = rotated_vector(std::vector{4,5,6,7,8,9,1}, i);
    loop_templates2 += LoopExpr::single({{1,2,3,4}, slice(seven, 0, 4), concat(slice(seven, 3), {seven[0]})});
  }
  auto loop_expr2 = loop_templates2.mapped_expanding([](const Loops& loops) {
    return sum_looped_vec([&](const std::vector<X>& x_args) {
      const auto args = mapped(x_args, [](X x) { return x.var(); });
      return LoopExpr::single(
        mapped(loops, [&](const std::vector<int>& loop) {
          return choose_indices_one_based(args, loop);
        })
      );
    }, 9, {1,2,3,4,5,6,7,8,9}, SumSign::plus);
  });
  // loop_expr2 = arg9_semi_lyndon(loop_expr2);
  loop_expr2 = to_canonical_permutation(arg9_semi_lyndon(loop_expr2));
  std::cout << "Orig " << loop_expr;
  std::cout << "New " << loop_expr2;
  std::cout << "Diff " << (loop_expr + loop_expr2);
  std::cout << to_lyndon_basis_3(lira_expr - loop_expr_to_lira_expr(loop_expr));
  return 0;
#endif


#if 1  // computations for 2022-07
  // Note. It important to first generate these excat expressions in this exact order.
  //   This helps keep loop names stable.
  const auto proto_a = loop_expr_degenerate(loop_expr, {{1,3}, {2,4}});
  const auto proto_b = loop_expr_degenerate(loop_expr, {{1,3}, {2,5}});
  const auto proto_c = loop_expr_degenerate(loop_expr, {{1,4}, {2,5}});
  const auto proto_d = loop_expr_degenerate(loop_expr, {{1,3}, {4,6}});  // == cycle(d, {{1,2}, {3,4}, {5,7}})
  const auto proto_e = loop_expr_degenerate(loop_expr, {{1,3}, {4,7}});
  const auto proto_f = loop_expr_degenerate(loop_expr, {{1,3}, {5,7}});  // == cycle(f, {{1,2}, {3,5}, {6,7}})
  const auto proto_g = loop_expr_degenerate(loop_expr, {{1,3}, {2,6}});
  const auto proto_h = loop_expr_degenerate(loop_expr, {{1,3}, {5,8}});
  const auto proto_i = loop_expr_degenerate(loop_expr, {{1,4}, {2,6}});
  const auto proto_j = loop_expr_degenerate(loop_expr, {{1,5}, {2,4}});  // == cycle(j, {{4,7}, {5,6}})
  const auto proto_k = loop_expr_degenerate(loop_expr, {{1,6}, {2,4}});
  const auto proto_l = loop_expr_degenerate(loop_expr, {{1,6}, {2,5}});
  const auto proto_x = loop_expr_degenerate(loop_expr, {{1,4}, {2,7}});
  const auto proto_y = loop_expr_degenerate(loop_expr, {{1,4}, {2,8}});
  const auto proto_z = loop_expr_degenerate(loop_expr, {{1,4}, {5,8}});
  const auto proto_u = loop_expr_degenerate(loop_expr, {{1,5}, {2,6}});
  const auto proto_w = loop_expr_degenerate(loop_expr, {{1,5}, {3,7}});
  const auto proto_m = loop_expr_degenerate(loop_expr, {{1,3,5}});
  const auto proto_n = loop_expr_degenerate(loop_expr, {{1,3,6}});
  const auto proto_o = loop_expr_degenerate(loop_expr, {{1,4,7}});

  generate_loops_names({
    proto_a, proto_b, proto_c, proto_d, proto_e, proto_f, proto_g, proto_h, proto_i, proto_j,
    proto_k, proto_l, proto_x, proto_y, proto_z, proto_u, proto_w, proto_m, proto_n, proto_o,
  });

  const auto a = reduce_arg9_loop_expr(proto_a);
  const auto b = reduce_arg9_loop_expr(proto_b);
  const auto c = reduce_arg9_loop_expr(proto_c);
  const auto d = reduce_arg9_loop_expr(proto_d);
  const auto e = reduce_arg9_loop_expr(proto_e);
  const auto f = reduce_arg9_loop_expr(proto_f);
  const auto g = reduce_arg9_loop_expr(proto_g);
  const auto h = reduce_arg9_loop_expr(proto_h);
  const auto i = reduce_arg9_loop_expr(proto_i);
  const auto j = reduce_arg9_loop_expr(proto_j);
  const auto k = reduce_arg9_loop_expr(proto_k);
  const auto l = reduce_arg9_loop_expr(proto_l);
  const auto x = reduce_arg9_loop_expr(proto_x);
  const auto y = reduce_arg9_loop_expr(proto_y);
  const auto z = reduce_arg9_loop_expr(proto_z);
  const auto u = reduce_arg9_loop_expr(proto_u);
  const auto w = reduce_arg9_loop_expr(proto_w);
  const auto m = reduce_arg9_loop_expr(proto_m);
  const auto n = reduce_arg9_loop_expr(proto_n);
  const auto o = reduce_arg9_loop_expr(proto_o);

  const auto e0 = e + f;  // != d
  const auto x0 = x - cycle(f, {{4,7}, {5,6}});

  // DUMP_EXPR(a);  // zero
  // DUMP_EXPR(b);  // zero
  DUMP_EXPR(c);
  DUMP_EXPR(d);
  DUMP_EXPR(e0);
  DUMP_EXPR(f);
  // DUMP_EXPR(g);  // zero
  DUMP_EXPR(h);
  DUMP_EXPR(i);
  DUMP_EXPR(j);
  DUMP_EXPR(k);
  DUMP_EXPR(l);
  DUMP_EXPR(x0);
  // DUMP_EXPR(y);  // == -cycle(d, {{1,2}, {3,7,6,5,4}})
  DUMP_EXPR(z);
  DUMP_EXPR(u);
  DUMP_EXPR(w);
  // DUMP_EXPR(m);  // zero
  // DUMP_EXPR(n);  // zero
  // DUMP_EXPR(o);  // zero

  std::vector<LoopExpr> space;
  const auto type_3_normalize =  // based on `d`
    + LoopExpr::single({{3,1,4,2}, {3,5,4,2}, {3,5,1,7,6}})
    - LoopExpr::single({{3,5,4,2}, {3,1,4,2}, {3,1,5,7,6}})
  ;
  for (const auto& perm : permutations(seq_incl(1, 7))) {
    // for (const auto& expr : {c, d, e0, f, h, i, j, k, l, x0, z, u, w}) {
    for (const auto& expr : {d, type_3_normalize}) {
      space.push_back(loop_expr_substitute(expr, perm));
    }
  }
  auto rank = space_rank(space, identity_function);
  std::cout << rank << "\n";


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

  // DUMP_EXPR(o0);
  // DUMP_EXPR(m0);
  // DUMP_EXPR(v0);


  // const auto& d6 = m0;
  // const auto& d7 = v0;
  // const auto& d8 = o0;

  // const auto d6a = d6 + cycle(d6, {{1,5}});
  // const auto d6b = cycle(d6a, {{1,2}, {3,5}});
  // const auto d7a = d7 + cycle(d7, {{2,3}});
  // const auto d67 = d7a + d6b;

  // const auto d8a = d8 + cycle(d8, {{1,5}});
  // const auto d8b = cycle(d8a, {{3,4}, {5,6,7}});
  // const auto d6c = d6 + cycle(d6, {{1,6}});
  // const auto d68 = d6c - d8b;

  // DUMP_EXPR(d6);
  // DUMP_EXPR(d7);
  // DUMP_EXPR(d8);
  // DUMP_EXPR(d67);
  // DUMP_EXPR(d68);
#endif


#if 0
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
  const auto m = loop_expr_degenerate(loop_expr, {{1,3,5}});
  const auto n = loop_expr_degenerate(loop_expr, {{1,3,6}});
  const auto o = loop_expr_degenerate(loop_expr, {{1,4,7}});

  generate_loops_names({a, b, c, d, e, f, g, h, i, j, k, l, m, o, x, y, z, u, w});

  const auto a1 = cycle(a, {{2,4}, {5,7}});
  const auto v = n + m - a + a1;  // NICE  ({1},{1},{2})

  // std::cout << "a " << a;
  // std::cout << "m " << m;
  // std::cout << "v " << v;
  // // std::cout << "b " << b;
  // std::cout << "c " << c;
  // std::cout << "d " << d;
  // std::cout << "e " << e;
  // std::cout << "f " << f;
  // // std::cout << "g " << g;
  // std::cout << "h " << h;
  // std::cout << "i " << i;
  // std::cout << "j " << j;
  // std::cout << "k " << k;
  // std::cout << "l " << l;
  // // std::cout << "n " << n;
  // std::cout << "o " << o;
  // std::cout << "x " << x;
  // std::cout << "y " << y;
  // std::cout << "z " << z;
  // std::cout << "u " << u;
  // // std::cout << "w " << w;

  std::cout << "m " << m;
  std::cout << "n " << n;
  std::cout << "o " << o;

  // std::cout << (
  //   + o
  //   - n
  //   - cycle(n, {{2,3}, {4,7}, {5,6}})
  // );

  // std::cout << "===\n\n";

  // const auto onm =
  //   + o
  //   - n
  //   - cycle(n, {{2,3}, {4,7}, {5,6}})
  //   - 3 * m
  //   - cycle(m, {{3,5,7}, {4,6}})
  //   - cycle(m, {{2,7}, {3,6}, {4,5}})
  // ;
  const auto onm =
    + o
    - n
    - cycle(n, {{2,3}, {4,7}, {5,6}})
    - 3 * m
    - cycle(m, {{7,6,5,4,3,2}})
    - cycle(m, {{2,7}, {3,6}, {4,5}})
  ;
  // std::cout << "n " << preshow(n);
  // std::cout << "m " << preshow(m);
  // std::cout << "o " << preshow(o);
  // std::cout << onm;
  // std::cout << arg9_expr_type_1_to_column(to_canonical_permutation(onm));

  // const auto qqq_tmpl =
  //   - LoopExpr::single({{2,1,4,3}, {2,1,5,4}, {2,1,5,7,6}})
  //   + LoopExpr::single({{2,1,6,7}, {2,1,5,6}, {2,1,5,4,3}})
  // ;
  // LoopExpr qqq;
  // for (int i : range(6)) {
  //   qqq += neg_one_pow(i) * loop_expr_substitute(qqq_tmpl, concat({1}, rotated_vector(seq_incl(2, 7), i)));
  // }
  // qqq = to_canonical_permutation(qqq);
  // std::cout << qqq;
  // std::cout << qqq + onm_c;


  std::cout << "===\n\n";

  const auto p = n + m;
  // std::cout << p;

  // std::cout << loop_expr_substitute(c, {{3,6}, {6,7}, {7,3}});
  // const auto q =
  //   + c
  //   + loop_expr_substitute(c, {{3,6}, {6,7}, {7,3}})
  //   + loop_expr_substitute(c, {{6,3}, {7,6}, {3,7}})
  // ;

  // const auto r = c + i;
  // std::cout << r;
  // const auto p1 = loop_expr_substitute(p, {{1,2}, {2,3}, {3,1}});
  // std::cout << p1;
  // std::cout << r + p1;
  // const auto d1 = loop_expr_substitute(d, {{4,2}, {2,4}, {5,7}, {7,5}});
  // const auto s = j + d1;
  // std::cout << d1;
  // std::cout << s;

  // const auto d1 = loop_expr_substitute(d, {{2,3}, {3,2}});
  // const auto t = j + d1;
  // std::cout << j;
  // std::cout << d1;
  // std::cout << t;

  // ZERO:  n - cycle(g, {{1,2}}) - a
  // ZERO (but above is simpler):  g - cycle(v, {{1,2}}) - cycle(a, {{1,4}, {5,7}}) + cycle(m, {{1,2}})
  // ZERO:  n + m - cycle(g + b, {{1,2}})

  const auto v1 = cycle(a, {{2,6}, {3,4,5}});
  // std::cout << v1;
  const auto va = v1 + m;
  const auto va1 = cycle(va, {{1,2,3}, {5,6}, {4,7}});
  const auto vb = b - va1;
  // std::cout << va;
  // std::cout << va1;
  // std::cout << vb;
  // std::cout << cycle(vb, {{1,4}, {5,6}});
  // std::cout << cycle(vb, {{1,4,7}, {5,6}});
  // std::cout << cycle(vb, {{1,4}, {5,6,7}});
  // std::cout << cycle(vb, {{1,6}, {5,4}});

  // const auto k1 = cycle(k, {{1,2}});
  // const auto w = i + k1;
  // // std::cout << k1;
  // // std::cout << w;

  const auto a2 = cycle(a, {{2,4}, {5,7}});
  const auto b1 = cycle(b, {{1,2}});
  // std::cout << n - a;
  // std::cout << a2;
  // std::cout << n - a + a2;
  // std::cout << b1;
  // std::cout << n - a + a2 + b1;
  // std::cout << n - a + a2 + b1 - a;
  // std::cout << n - a + a2 + b1 - a - v;
  // ZERO:  n - a + a2 + b1 - a - v

  const auto m1 = cycle(m, {{1,2,5}, {6,7}});
  const auto m2 = cycle(m, {{2,5,6}, {4,7}});
  const auto fm = f + m1 + m2;  // NICE: {1,1,9,9}
  // std::cout << m1;
  // std::cout << m2;
  // std::cout << fm;

  const auto efd = e + f + d;
  // std::cout << efd;

  // std::cout << cycle(fm, {{3,1}, {5,2}});
  const auto c1 = cycle(c, {{4,7}, {5,6}});
  const auto c2 = c - c1;
  // std::cout << c1;
  // std::cout << c2;
  // std::cout << cycle(c2, {{6,7}});
  // std::cout << c2 + cycle(c2, {{6,7}});
  // std::cout << c1 + j;

  const auto c3 = cycle(c, {{1,2}});
  const auto c4 = c - c3;
  // std::cout << c;
  // std::cout << c3;
  // std::cout << c4;
  // std::cout << cycle(c4, {{1,6}});
  // std::cout << c4 - cycle(c4, {{1,6}});

  // const auto x1 =
  //   + x
  //   - cycle(fm, {{5,6}, {4,7}})
  //   - cycle(d, {{5,6}, {4,7}})
  // ;
  // std::cout << x1;
  // const auto x2 =
  //   + x
  //   + cycle(e, {{5,6}, {4,7}})
  // ;
  // // std::cout << x2;
  // const auto x3 =
  //   + x2
  //   - cycle(m, {{1,2}, {4,6}, {3,7}})
  //   - m
  // ;
  // // std::cout << x3;
  // const auto x4 =  // :(
  //   + x3
  //   + cycle(v, {{1,2}, {4,6}, {3,7}})
  //   - cycle(a, {{1,6}, {3,5}})
  // ;
  // // std::cout << x4;

  const auto o0 =
    + o
    // - m
    - cycle(m, {{2,6}, {3,5}})
    - cycle(m, {{3,7}, {4,6}})
    - cycle(m, {{2,4}, {5,7}})
    - a
    - v
    + cycle(a, {{2,4}, {5,7}})
    // + cycle(a, {{2,7}, {3,4}, {5,6}})
    + cycle(v, {{2,3,4,5,6,7}})
    - cycle(a, {{2,3}, {4,7}, {5,6}})
    - cycle(v, {{2,3}, {4,7}, {5,6}})
  ;
  const auto m0 = m - cycle(m, {{2,6}});  //  ~=  m - cycle(m, {{3,5}})
  const auto v0 = v + cycle(v, {{1,6}});

  std::cout << onm;
  // std::cout << o0;
  std::cout << m0;
  // std::cout << v0;

  // std::cout << "===\n\n";

  // std::cout << to_canonical_permutation(cycle(m0, {{2,5,7}, {3,6}}));

  // std::cout << "===\n\n";

  std::cout << arg9_expr_type_1_to_column(onm);
  // std::cout << arg9_expr_type_1_to_column(o0);
  std::cout << arg9_expr_type_1_to_column(m0);
  // std::cout << arg9_expr_type_1_to_column(v0);

  // std::cout << "%%%\n\n";

  // const auto ac = to_canonical_permutation(a);
  // const auto mc = to_canonical_permutation(m);
  // const auto vc = to_canonical_permutation(v);

  // std::cout << o0c;
  // std::cout << ac;
  // std::cout << mc;
  // std::cout << vc;

  // std::cout << "===\n\n";

  // const auto mc1 = to_canonical_permutation(cycle(mc, {{4,6}, {3,7}}));
  // std::cout << mc1;
  // std::cout << o0c + mc1;

  // std::cout << o0 + cycle(mc, {{4,6}, {3,7}});
#endif

#if 0
  std::cout << "%%%\n\n";

  std::cout << to_canonical_permutation(
    + m0c
    - cycle(m0c, {{5,6}, {4,7}, {2,3}})
  );
  const auto m0c1 = to_canonical_permutation(
    + m0c
    - cycle(m0c, {{5,6}, {4,7}, {2,3}})
    + cycle(o0c, {{5,3,7}, {4,6}})
  );
  const auto m0c2 = to_canonical_permutation(
    + m0c
    - cycle(m0c, {{5,6}, {4,7}, {2,3}})
    + cycle(o0c, {{5,3,7}, {4,6}})
  );
  const auto m0c2x = to_canonical_permutation(
    + m0c2
    + cycle(m0c2, {{1,3}})
  );
  std::cout << m0c1;
  std::cout << m0c2;

  std::cout << arg9_expr_type_1_to_column(m0c1);
  std::cout << arg9_expr_type_1_to_column(m0c2);

  std::cout << m0c2x;
  std::cout << arg9_expr_type_1_to_column(m0c2x);
#endif
}
