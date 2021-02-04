#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/packed.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"
#include "lib/zip.h"


static constexpr auto cycle = loop_expr_cycle;

LoopExpr cycle_pow(LoopExpr expr, const std::vector<std::vector<int>>& cycles, int power) {
  for (int _ : range(power)) {
    expr = cycle(expr, cycles);
  }
  return expr;
}

LoopExpr keep_var(const LoopExpr& expr, int var) {
  return expr.filtered([&](const Loops& loops) {
    return loops_names.loops_index(loops) == var;
  });
}

LoopExpr preshow(const LoopExpr& expr) {
  return expr;
  // return keep_var(expr, 5);
  // return arg11_shuffle_cluster(expr);
}

std::string permutation_to_string(const std::vector<std::vector<int>>& permutation) {
  return str_join(
    mapped(permutation, [&](const std::vector<int>& loop) {
      return fmt::parens(str_join(loop, ","));
    }),
    ""
  );
}

LoopExpr auto_kill_planar(LoopExpr victim, const LoopExpr& killer, int target_type) {
  static const std::vector<std::vector<std::vector<int>>> symmetries{
    {{2,4}, {5,8}, {6,7}},
    {{2,5}, {3,4}, {6,8}},
    {{2,6}, {3,5}, {7,8}},
    {{2,7}, {3,6}, {4,5}},
    {{3,8}, {4,7}, {5,6}},
    {{2,3}, {4,8}, {5,7}},
    {{2,8}, {3,7}, {4,6}},
  };
  const auto update_victim = [&](const LoopExpr& bonus, int sign, std::string description) {
    const auto new_victim_candidate = victim + sign * bonus;
    if (keep_var(new_victim_candidate, target_type).l1_norm() < keep_var(victim, target_type).l1_norm()) {
      victim = new_victim_candidate;
      std::cout << fmt::coeff(sign) << description << "\n";
    }
  };
  for (const auto& permutation : symmetries) {
    for (int sign : {-1, 1}) {
      update_victim(
        cycle(killer, permutation),
        sign,
        "symmetry: " + permutation_to_string(permutation)
      );
    }
  }
  for (int rotation_pow : range(1, 8)) {
    for (int sign : {-1, 1}) {
      update_victim(
        cycle_pow(killer, {{2,3,4,5,6,7,8}}, rotation_pow),
        sign,
        absl::StrCat("rotate ", rotation_pow, " positions")
      );
    }
  }
  return victim;
}

// TODO: Move to strings.h
template<typename Map>
std::string map_to_string(const Map& map) {
  std::string ret;
  for (const auto& [a, b] : map) {
    ret += absl::StrCat(to_string(a), " => ", to_string(b), "\n");
  }
  return ret;
}

// TODO: Normalize cycles
std::vector<std::vector<int>> substitutions_to_cycles(absl::flat_hash_map<int, int> substitutions) {
  std::vector<std::vector<int>> ret;
  while (!substitutions.empty()) {
    const int start = substitutions.begin()->first;
    int next = substitutions.begin()->second;
    substitutions.erase(start);
    if (start == next) {
      continue;
    } else {
      std::vector<int> cycle = {start};
      while (next != start) {
        cycle.push_back(next);
        const int prev = next;
        next = substitutions.at(prev);
        substitutions.erase(prev);
      }
      ret.push_back(std::move(cycle));
    }
  }
  return ret;
}

absl::flat_hash_map<int, int> get_substitution(const Loops& from, const Loops& to) {
  absl::flat_hash_map<int, int> ret;
  for (auto [a, b] : zip(flatten(from), flatten(to))) {
    if (ret.contains(a)) {
      CHECK(ret.at(a) == b);
    } else {
      ret[a] = b;
    }
  }
  return ret;
}

LoopExpr auto_kill(LoopExpr victim, const LoopExpr& killer, int target_type) {
  std::cout << ".\n";
  Loops killer_term;
  killer.foreach([&](const Loops& loops, int) {
    if (loops_names.loops_index(loops) == target_type) {
      killer_term = loops;
    }
  });
  CHECK(!killer_term.empty());
  bool stuck = false;
  while (!stuck) {
    stuck = true;
    Loops victim_term;
    victim.foreach([&](const Loops& loops, int) {
      if (loops_names.loops_index(loops) == target_type) {
        victim_term = loops;
      }
    });
    if (victim_term.empty()) {
      break;  // everything killed already
    }
    // NOTE. Assumes canonical form.
    // NOTE. Doesn't take into account different ways of treating symmetries.
    const auto subst = get_substitution(killer_term, victim_term);
    for (int sign : {-1, 1}) {
      const auto new_victim_candidate = victim + sign * loop_expr_substitute(killer, subst);
      if (keep_var(new_victim_candidate, target_type).l1_norm() < keep_var(victim, target_type).l1_norm()) {
        stuck = false;
        victim = new_victim_candidate;
        std::cout << fmt::coeff(sign) << "cycle " << permutation_to_string(substitutions_to_cycles(subst)) << "\n";
      }
    }
  }
  return victim;
}

StringExpr arg9_expr_type_1_to_column(const LoopExpr& expr) {
  return expr.mapped<StringExpr>([](const Loops& loops) {
    std::vector<int> v = concat(
      loops_unique_common_variable(loops, {0,1,2}),
      loops_unique_common_variable(loops, {2}),
      loops_unique_common_variable(loops, {0}),
      loops_unique_common_variable(loops, {0,1}),
      loops_unique_common_variable(loops, {1,2})
    );
    CHECK_EQ(v.size(), 7);
    return fmt::brackets(str_join(v, ","));
  });
}

StringExpr arg11_expr_type_2_to_column(const LoopExpr& expr) {
  return expr.mapped<StringExpr>([](const Loops& loops) {
    CHECK_EQ(loops.size(), 4);
    std::vector<int> v = concat(
      loops_unique_common_variable(loops, {0,1,2,3}),  // x2
      loops_unique_common_variable(loops, {0}),        // x2
      loops_unique_common_variable(loops, {3}),
      loops_unique_common_variable(loops, {0,1}),
      loops_unique_common_variable(loops, {1,2}),
      loops_unique_common_variable(loops, {2,3})
    );
    CHECK_EQ(v.size(), 8);
    return fmt::brackets(str_join(v, ","));
  });
}

using Degenerations = std::vector<std::vector<int>>;

bool is_degenerations_ok(const Degenerations& groups) {
  for (int i : range(groups.size())) {
    for (int j : range(i+1, groups.size())) {
      if (set_intersection_size(groups[i], groups[j]) > 0) {
        return false;
      }
    }
  }
  return true;
}

void normalize_degenerations(Degenerations& groups) {
  for (auto& group : groups) {
    absl::c_sort(group);
  }
  absl::c_sort(groups);
}

Degenerations normalized_degenerations(Degenerations groups) {
  normalize_degenerations(groups);
  return groups;
}

Degenerations flip_variables(const Degenerations& groups, int total_vars) {
  return mapped(groups, [&](const auto& group) {
    return mapped(group, [&](int v) {
      return total_vars + 1 - v;
    });
  });
}

Degenerations rotate_variables(const Degenerations& groups, int total_vars, int shift) {
  return mapped(groups, [&](const auto& group) {
    return mapped(group, [&](int v) {
      return (v - 1 + shift) % total_vars + 1;
    });
  });
}

bool contains_only_expression_of_type(const LoopExpr& expr, const std::vector<int>& types) {
  absl::flat_hash_set<int> types_set(types.begin(), types.end());
  bool ret = true;
  expr.foreach([&](const auto& loops, int) {
    if (!types_set.contains(loops_names.loops_index(loops))) {
      ret = false;
      return;
    }
  });
  return ret;
}

void list_all_degenerations(const LoopExpr& expr) {
  const int N = 11;
  const int a1 = 1;
  absl::flat_hash_set<Degenerations> degenerations_seen;

  const auto add_degeneration = [&](Degenerations groups) {
    if (!is_degenerations_ok(groups)) {
      return;
    }
    normalize_degenerations(groups);
    auto groups_flipped = flip_variables(groups, N);
    for (int rotation_pow : range(N)) {
      if (degenerations_seen.contains(normalized_degenerations(rotate_variables(groups, N, rotation_pow))) ||
          degenerations_seen.contains(normalized_degenerations(rotate_variables(groups_flipped, N, rotation_pow)))) {
        return;
      }
    }
    CHECK(degenerations_seen.insert(groups).second);
    const auto expr_degenerated = loop_expr_degenerate(expr, groups);
    if (!expr_degenerated.zero() && contains_only_expression_of_type(expr_degenerated, {1,2,3,4,5,6})) {
      std::cout << permutation_to_string(groups) << " => " << preshow(expr_degenerated) << fmt::newline();
    }
  };

  for (int a2 : range_incl(a1+2, N)) {
  for (int b1 : range_incl(1,    N)) {
  for (int b2 : range_incl(b1+2, N)) {
  for (int c1 : range_incl(1,    N)) {
  for (int c2 : range_incl(c1+2, N)) {
    add_degeneration({{a1, a2}, {b1, b2}, {c1, c2}});
  }
  }
  }
  }
  }

  for (int a2 : range_incl(a1+2, N)) {
  for (int a3 : range_incl(a2+2, N)) {
  for (int b1 : range_incl(1,    N)) {
  for (int b2 : range_incl(b1+2, N)) {
    add_degeneration({{a1, a2, a3}, {b1, b2}});
  }
  }
  }
  }
}


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_formatter(Formatter::ascii)
    .set_formatter(Formatter::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_annotation_sorting(AnnotationSorting::length)
  );


#if 1
  const int N = 11;
  const int num_points = N;
  const int num_args = num_points / 2 - 1;
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


  // std::cout << "Via LiQuad " << lira_expr << "\n";
  // std::cout << "Loops " << loop_expr_recursive << "\n";
  // std::cout << "Via loops " << loop_lira_expr << "\n";
  // std::cout << "Diff " << to_lyndon_basis(lira_expr + loop_lira_expr) << "\n";

  const auto a = loop_expr_degenerate(loop_expr, {{1,3,5,7}});
  const auto b = loop_expr_degenerate(loop_expr, {{1,3,5,8}});
  const auto c = loop_expr_degenerate(loop_expr, {{1,3,6,8}});  // == expr(a, b)
  const auto d = loop_expr_degenerate(loop_expr, {{1,3,6,9}});  // == expr(a, b, e)
  const auto e = loop_expr_degenerate(loop_expr, {{1,3,7,9}});

  const auto f = loop_expr_degenerate(loop_expr, {{1,3,5}, {2,4}});
  const auto g = loop_expr_degenerate(loop_expr, {{1,3,5}, {2,11}});
  const auto h = loop_expr_degenerate(loop_expr, {{1,4,7}, {3,5}});  // expr(f, g, a)
  const auto fg = loop_expr_degenerate(loop_expr, {{1,3,6}, {2,4}});  // == -(f1 + f2)

  const auto gg = loop_expr_degenerate(loop_expr, {{2,5}, {1,3}, {4,6}});  // ==  g(symmetry: (2,3)(4,8)(5,7) + rotate 7 positions)
  const auto m = loop_expr_degenerate(loop_expr, {{2,6}, {1,3}, {5,7}});
  const auto n = loop_expr_degenerate(loop_expr, {{2,7}, {1,3}, {6,8}});

  generate_loops_names({a, b, c, d, e});

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

  std::cout << "===\n\n";

  const auto mf = auto_kill(m, f, 6);  // {1,2,4}
  const auto mfa = auto_kill(mf, a, 1);  // {2,4}
  const auto mfg = auto_kill(mf, g, 4);  // {2,3}
  const auto mfg1 = mfg + cycle(mfg, {{3,5}});
  const auto mfg2 = mfg1 + cycle(mfg1, {{2,8}});  // only {2}

  const auto nf = auto_kill(n, f, 6);  // {1,2,3,4}
  const auto nfg = auto_kill(nf, g, 4);  // {2,3}
  const auto nfg1 = nfg + cycle(nfg, {{3,5}});
  const auto nfg2 = nfg1 + cycle(nfg1, {{2,4}});  // only {2}

  std::cout << "\n";
  std::cout << mfg << "\n";
  std::cout << nfg << "\n";

  std::cout << arg11_expr_type_2_to_column(mfg2) << "\n";
  std::cout << arg11_expr_type_2_to_column(nfg2) << "\n";


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

  const auto eb =  // {1,2,4}
    + e
    - cycle(b, {{2,7}, {3,6}, {4,5}})
    + cycle(b, {{3,8}, {4,7}, {5,6}})
  ;

  // const auto eba = auto_kill_planar(eb, a, 1);
  // const auto eba1 = eba + cycle(eba, {{4,5}});
  // const auto eba2 = eba1 + cycle(eba1, {{2,8}});  // just one variable (but need correction for new indices)

  // std::cout << eb << "\n";
  // std::cout << eba << "\n";
  // std::cout << eba1 << "\n";
  // std::cout << eba2 << "\n";
  // std::cout << arg11_expr_type_2_to_column(eba2) << "\n";

  const auto dbe =  // {1,2}
    + db
    + eb
    + cycle(eb, {{2,3}, {4,8}, {5,7}})
  ;

  // ZERO:  + dbe - a + cycle(a, {{2,6}, {3,5}, {7,8}})

  const auto a1 = a - cycle(a, {{4,6}});
  const auto a2 = a1 - cycle(a1, {{2,7}});
  // std::cout << "a " << a << "\n";
  // std::cout << a1 << "\n";
  // std::cout << a2 << "\n";
  // std::cout << arg11_expr_type_2_to_column(a2) << "\n";

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

  const auto eb1 =
    + eb
    + cycle(eb, {{2,8}})
  ;

  const auto eb2 =  // {1,2}
    + eb1
    + cycle(eb1, {{4,5}})
  ;

  const auto eb3 =  // {1,2}
    + eb
    - cycle(eb, {{2,4}, {5,8}, {6,7}})
  ;

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
  // ) << "\n";

  // std::cout << preshow(eb) << "\n";

  // std::cout << preshow(
  //   + eb
  //   // + cycle(eb, {{2,8}, {3,7}, {4,6}})
  // ) << "\n";

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
  // ) << "\n";

#endif

#if 0
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
    return sum_looped_vec([&](const std::vector<X>& x_args) {
      const auto args = mapped(x_args, [](X x) { return x.var(); });
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
  std::cout << "Orig " << loop_expr << "\n";
  std::cout << "New " << loop_expr2 << "\n";
  std::cout << "Diff " << (loop_expr + loop_expr2) << "\n";
  std::cout << to_lyndon_basis_3(lira_expr - loop_expr_to_lira_expr(loop_expr)) << "\n";
  return 0;
#endif


  // std::cout << loop_expr_degenerate(loop_expr, {{1,3}});
  // return 0;


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

  // std::cout << "a " << a << "\n";
  // std::cout << "m " << m << "\n";
  // std::cout << "v " << v << "\n";
  // // std::cout << "b " << b << "\n";
  // std::cout << "c " << c << "\n";
  // std::cout << "d " << d << "\n";
  // std::cout << "e " << e << "\n";
  // std::cout << "f " << f << "\n";
  // // std::cout << "g " << g << "\n";
  // std::cout << "h " << h << "\n";
  // std::cout << "i " << i << "\n";
  // std::cout << "j " << j << "\n";
  // std::cout << "k " << k << "\n";
  // std::cout << "l " << l << "\n";
  // // std::cout << "n " << n << "\n";
  // std::cout << "o " << o << "\n";
  // std::cout << "x " << x << "\n";
  // std::cout << "y " << y << "\n";
  // std::cout << "z " << z << "\n";
  // std::cout << "u " << u << "\n";
  // // std::cout << "w " << w << "\n";

  std::cout << "m " << m << "\n";
  std::cout << "n " << n << "\n";
  std::cout << "o " << o << "\n";

  std::cout << (
    + o
    - n
    - cycle(n, {{2,3}, {4,7}, {5,6}})
  ) << "\n";

  return 0;

  // std::cout << "a " << a << "\n";
  // std::cout << "m " << m << "\n";
  // std::cout << "v " << v << "\n";
  // std::cout << "b " << b << "\n";
  // std::cout << "c " << c << "\n";
  // std::cout << "d " << d << "\n";
  // std::cout << "e " << e << "\n";
  // std::cout << "f " << f << "\n";
  // std::cout << "g " << g << "\n";
  // std::cout << "h " << h << "\n";
  // std::cout << "i " << i << "\n";
  // std::cout << "j " << j << "\n";
  // std::cout << "k " << k << "\n";
  // std::cout << "l " << l << "\n";
  // std::cout << "n " << n << "\n";
  // std::cout << "o " << o << "\n";
  // std::cout << "x " << x << "\n";
  // std::cout << "y " << y << "\n";
  // std::cout << "z " << z << "\n";
  // std::cout << "u " << u << "\n";
  // std::cout << "w " << w << "\n";

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
  const auto onm_c = to_canonical_permutation(onm);
  // std::cout << "n " << preshow(n) << "\n";
  // std::cout << "m " << preshow(m) << "\n";
  // std::cout << "o " << preshow(o) << "\n";
  // std::cout << onm << "\n";
  std::cout << onm_c << "\n";
  // std::cout << arg9_expr_type_1_to_column(to_canonical_permutation(onm)) << "\n";

  const auto qqq_tmpl =
    - LoopExpr::single({{2,1,4,3}, {2,1,5,4}, {2,1,5,7,6}})
    + LoopExpr::single({{2,1,6,7}, {2,1,5,6}, {2,1,5,4,3}})
  ;
  LoopExpr qqq;
  for (int i : range(6)) {
    qqq += neg_one_pow(i) * loop_expr_substitute(qqq_tmpl, concat({1}, rotated_vector(seq_incl(2, 7), i)));
  }
  qqq = to_canonical_permutation(qqq);
  std::cout << qqq << "\n";
  std::cout << qqq + onm_c << "\n";


  std::cout << "===\n\n";

  const auto p = n + m;
  // std::cout << p << "\n";

  // std::cout << loop_expr_substitute(c, {{3,6}, {6,7}, {7,3}}) << "\n";
  // const auto q =
  //   + c
  //   + loop_expr_substitute(c, {{3,6}, {6,7}, {7,3}})
  //   + loop_expr_substitute(c, {{6,3}, {7,6}, {3,7}})
  // ;

  // const auto r = c + i;
  // std::cout << r << "\n";
  // const auto p1 = loop_expr_substitute(p, {{1,2}, {2,3}, {3,1}});
  // std::cout << p1 << "\n";
  // std::cout << r + p1 << "\n";
  // const auto d1 = loop_expr_substitute(d, {{4,2}, {2,4}, {5,7}, {7,5}});
  // const auto s = j + d1;
  // std::cout << d1 << "\n";
  // std::cout << s << "\n";

  // const auto d1 = loop_expr_substitute(d, {{2,3}, {3,2}});
  // const auto t = j + d1;
  // std::cout << j << "\n";
  // std::cout << d1 << "\n";
  // std::cout << t << "\n";

  // ZERO:  n - cycle(g, {{1,2}}) - a
  // ZERO (but above is simpler):  g - cycle(v, {{1,2}}) - cycle(a, {{1,4}, {5,7}}) + cycle(m, {{1,2}})
  // ZERO:  n + m - cycle(g + b, {{1,2}})

  const auto v1 = cycle(a, {{2,6}, {3,4,5}});
  // std::cout << v1 << "\n";
  const auto va = v1 + m;
  const auto va1 = cycle(va, {{1,2,3}, {5,6}, {4,7}});
  const auto vb = b - va1;
  // std::cout << va << "\n";
  // std::cout << va1 << "\n";
  // std::cout << vb << "\n";
  // std::cout << cycle(vb, {{1,4}, {5,6}}) << "\n";
  // std::cout << cycle(vb, {{1,4,7}, {5,6}}) << "\n";
  // std::cout << cycle(vb, {{1,4}, {5,6,7}}) << "\n";
  // std::cout << cycle(vb, {{1,6}, {5,4}}) << "\n";

  // const auto k1 = cycle(k, {{1,2}});
  // const auto w = i + k1;
  // // std::cout << k1 << "\n";
  // // std::cout << w << "\n";

  const auto a2 = cycle(a, {{2,4}, {5,7}});
  const auto b1 = cycle(b, {{1,2}});
  // std::cout << n - a << "\n";
  // std::cout << a2 << "\n";
  // std::cout << n - a + a2 << "\n";
  // std::cout << b1 << "\n";
  // std::cout << n - a + a2 + b1 << "\n";
  // std::cout << n - a + a2 + b1 - a << "\n";
  // std::cout << n - a + a2 + b1 - a - v << "\n";
  // ZERO:  n - a + a2 + b1 - a - v

  const auto m1 = cycle(m, {{1,2,5}, {6,7}});
  const auto m2 = cycle(m, {{2,5,6}, {4,7}});
  const auto fm = f + m1 + m2;  // NICE: {1,1,9,9}
  // std::cout << m1 << "\n";
  // std::cout << m2 << "\n";
  // std::cout << fm << "\n";

  const auto efd = e + f + d;
  // std::cout << efd << "\n";

  // std::cout << cycle(fm, {{3,1}, {5,2}}) << "\n";
  const auto c1 = cycle(c, {{4,7}, {5,6}});
  const auto c2 = c - c1;
  // std::cout << c1 << "\n";
  // std::cout << c2 << "\n";
  // std::cout << cycle(c2, {{6,7}}) << "\n";
  // std::cout << c2 + cycle(c2, {{6,7}}) << "\n";
  // std::cout << c1 + j << "\n";

  const auto c3 = cycle(c, {{1,2}});
  const auto c4 = c - c3;
  // std::cout << c << "\n";
  // std::cout << c3 << "\n";
  // std::cout << c4 << "\n";
  // std::cout << cycle(c4, {{1,6}}) << "\n";
  // std::cout << c4 - cycle(c4, {{1,6}}) << "\n";

  // const auto x1 =
  //   + x
  //   - cycle(fm, {{5,6}, {4,7}})
  //   - cycle(d, {{5,6}, {4,7}})
  // ;
  // std::cout << x1 << "\n";
  // const auto x2 =
  //   + x
  //   + cycle(e, {{5,6}, {4,7}})
  // ;
  // // std::cout << x2 << "\n";
  // const auto x3 =
  //   + x2
  //   - cycle(m, {{1,2}, {4,6}, {3,7}})
  //   - m
  // ;
  // // std::cout << x3 << "\n";
  // const auto x4 =  // :(
  //   + x3
  //   + cycle(v, {{1,2}, {4,6}, {3,7}})
  //   - cycle(a, {{1,6}, {3,5}})
  // ;
  // // std::cout << x4 << "\n";

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

  const auto o0c = to_canonical_permutation(o0);
  const auto m0c = to_canonical_permutation(m0);
  const auto v0c = to_canonical_permutation(v0);

  // std::cout << o0 << "\n";
  // std::cout << m0 << "\n";
  // std::cout << v0 << "\n";

  // std::cout << "===\n\n";

  // std::cout << o0c << "\n";
  // std::cout << m0c << "\n";
  // std::cout << v0c << "\n";

  // std::cout << to_canonical_permutation(cycle(m0, {{2,5,7}, {3,6}}));

  // std::cout << "===\n\n";

  // std::cout << arg9_expr_type_1_to_column(o0c) << "\n";
  // std::cout << arg9_expr_type_1_to_column(m0c) << "\n";
  // std::cout << arg9_expr_type_1_to_column(v0c) << "\n";

  // std::cout << "%%%\n\n";

  // const auto ac = to_canonical_permutation(a);
  // const auto mc = to_canonical_permutation(m);
  // const auto vc = to_canonical_permutation(v);

  // std::cout << o0c << "\n";
  // std::cout << ac << "\n";
  // std::cout << mc << "\n";
  // std::cout << vc << "\n";

  // std::cout << "===\n\n";

  // const auto mc1 = to_canonical_permutation(cycle(mc, {{4,6}, {3,7}}));
  // std::cout << mc1 << "\n";
  // std::cout << o0c + mc1 << "\n";

  // std::cout << o0 + cycle(mc, {{4,6}, {3,7}}) << "\n";
#endif

#if 0
  std::cout << "%%%\n\n";

  std::cout << to_canonical_permutation(
    + m0c
    - cycle(m0c, {{5,6}, {4,7}, {2,3}})
  ) << "\n";
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
  std::cout << m0c1 << "\n";
  std::cout << m0c2 << "\n";

  std::cout << arg9_expr_type_1_to_column(m0c1) << "\n";
  std::cout << arg9_expr_type_1_to_column(m0c2) << "\n";

  std::cout << m0c2x << "\n";
  std::cout << arg9_expr_type_1_to_column(m0c2x) << "\n";
#endif
}
