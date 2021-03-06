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
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
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
  for (EACH : range(power)) {
    expr = cycle(expr, cycles);
  }
  return expr;
}

LoopExpr keep_term(const LoopExpr& expr, int type) {
  return expr.filtered([&](const Loops& loops) {
    return loops_names.loops_index(loops) == type;
  });
}

int num_distinct_term_types(const LoopExpr& expr) {
  std::vector<int> term_types;
  expr.foreach([&](const Loops& loops, int) {
    term_types.push_back(loops_names.loops_index(loops));
  });
  return num_distinct_elements(term_types);
}

LoopExpr preshow(const LoopExpr& expr) {
  return expr;
  // return keep_term(expr, 7);
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
    if (keep_term(new_victim_candidate, target_type).l1_norm() < keep_term(victim, target_type).l1_norm()) {
      victim = new_victim_candidate;
      std::cout << fmt::coeff(sign) << description << "\n";
    }
  };
  std::cout << ".\n";
  for (const auto& permutation : symmetries) {
    for (int sign : {-1, 1}) {
      update_victim(
        cycle(killer, permutation),
        sign,
        "symmetry " + permutation_to_string(permutation)
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
      CHECK(ret.at(a) == b) << "\n" << LoopExprParam::object_to_string(from) << "\n" << LoopExprParam::object_to_string(to);
    } else {
      ret[a] = b;
    }
  }
  return ret;
}

LoopExpr auto_kill(LoopExpr victim, const LoopExpr& killer, int target_type) {
  const auto expr_complexity = [&](const LoopExpr& expr) {
    return std::tuple{
      keep_term(expr, target_type).l1_norm(),
      num_distinct_term_types(expr),
      expr.l1_norm(),
    };
  };
  std::cout << ".\n";
  std::vector<Loops> killer_terms;
  killer.foreach([&](const Loops& loops, int) {
    if (loops_names.loops_index(loops) == target_type) {
      killer_terms.push_back(loops);
    }
  });
  CHECK(!killer_terms.empty()) << "Term type " << target_type << " not found in\n" << killer;
  bool stuck = false;
  while (!stuck) {
    stuck = true;
    std::vector<Loops> victim_terms;
    victim.foreach([&](const Loops& loops, int) {
      if (loops_names.loops_index(loops) == target_type) {
        victim_terms.push_back(loops);
      }
    });
    if (victim_terms.empty()) {
      break;  // everything killed already
    }
    std::optional<LoopExpr> best_candidate;
    std::string best_candidate_description;
    for (const Loops& killer_term : killer_terms) {
      for (const Loops& victim_term : victim_terms) {
        // NOTE. Assumes canonical form.
        // TODO: take into account different ways of treating symmetries.
        const auto subst = get_substitution(killer_term, victim_term);
        const auto killer_subst = loop_expr_substitute(killer, subst);
        for (int sign : {-1, 1}) {
          const auto new_candidate = victim + sign * killer_subst;
          if (keep_term(new_candidate, target_type).l1_norm() < keep_term(victim, target_type).l1_norm()) {
            // std::cout << ">>> " << absl::StrCat(fmt::coeff(sign), "cycle ", permutation_to_string(substitutions_to_cycles(subst)))
            //     << " : " << new_candidate << " => " << dump_to_string(expr_complexity(new_candidate)) << "\n\n";
            if (!best_candidate || expr_complexity(new_candidate) < expr_complexity(*best_candidate)) {
              best_candidate = new_candidate;
              best_candidate_description = absl::StrCat(
                  fmt::coeff(sign), "cycle ", permutation_to_string(substitutions_to_cycles(subst)));
            }
          }
        }
      }
    }
    if (best_candidate) {
      stuck = false;
      victim = *best_candidate;
      std::cout << best_candidate_description << "\n";
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

bool has_common_variable_in_each_term(const LoopExpr& expr) {
  bool ret = true;
  expr.foreach([&](const auto& loops, int) {
    if (set_intersection(loops).empty()) {
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
    // if (!expr_degenerated.is_zero() && contains_only_expression_of_type(expr_degenerated, {1,2,3,4,5,6})) {
    if (!expr_degenerated.is_zero() && has_common_variable_in_each_term(expr_degenerated)) {
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
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(10)
    .set_annotation_sorting(AnnotationSorting::length)
  );


#if 0
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

#endif

#if 1
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
  std::cout << "Orig " << loop_expr;
  std::cout << "New " << loop_expr2;
  std::cout << "Diff " << (loop_expr + loop_expr2);
  std::cout << to_lyndon_basis_3(lira_expr - loop_expr_to_lira_expr(loop_expr));
  return 0;
#endif


  // std::cout << loop_expr;
  // std::cout << loop_expr_degenerate(loop_expr, {{1,3,5}});
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

  // std::cout << "a " << a;
  // std::cout << "m " << m;
  // std::cout << "v " << v;
  // std::cout << "b " << b;
  // std::cout << "c " << c;
  // std::cout << "d " << d;
  // std::cout << "e " << e;
  // std::cout << "f " << f;
  // std::cout << "g " << g;
  // std::cout << "h " << h;
  // std::cout << "i " << i;
  // std::cout << "j " << j;
  // std::cout << "k " << k;
  // std::cout << "l " << l;
  // std::cout << "n " << n;
  // std::cout << "o " << o;
  // std::cout << "x " << x;
  // std::cout << "y " << y;
  // std::cout << "z " << z;
  // std::cout << "u " << u;
  // std::cout << "w " << w;

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
