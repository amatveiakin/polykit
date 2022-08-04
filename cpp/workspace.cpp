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

#if 1
#include "lib/epsilon.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/snowpal.h"
#include "lib/theta.h"
#elif defined(HAS_EPSILON_EXPR) || defined(HAS_THETA_EXPR) || defined(HAS_LIRA_EXPR)
#  error "Expression type leaked: check header structure"
#endif


#if 1
static constexpr auto cycle = loop_expr_cycle;

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
  return num_distinct_elements_unsorted(term_types);
}

LoopExpr preshow(const LoopExpr& expr) {
  return expr;
  // return keep_term(expr, 7);
  // return arg11_shuffle_cluster(expr);
}

std::string groups_to_string(const std::vector<std::vector<int>>& groups) {
  return str_join(
    mapped(groups, [&](const std::vector<int>& group) {
      return fmt::braces(str_join(group, ","));
    }),
    ""
  );
}

LoopExpr auto_kill_planar(LoopExpr victim, const LoopExpr& killer, int target_type) {
  static const std::vector<Permutation> symmetries{
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
        "symmetry " + to_string(permutation)
      );
    }
  }
  for (int rotation_pow : range(1, 8)) {
    for (int sign : {-1, 1}) {
      update_victim(
        cycle(killer, Permutation({{2,3,4,5,6,7,8}}).pow(rotation_pow)),
        sign,
        absl::StrCat("rotate ", rotation_pow, " positions")
      );
    }
  }
  return victim;
}

// TODO: Normalize cycles
//   Alternatively, should this return `Permutation`?
//   (And additionally, should permutations be normalized?)
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

// TODO: Add linear algebra support
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
            // std::cout << ">>> " << absl::StrCat(fmt::coeff(sign), "cycle ", to_string(Permutation(substitutions_to_cycles(subst))))
            //     << " : " << new_candidate << " => " << dump_to_string(expr_complexity(new_candidate)) << "\n\n";
            if (!best_candidate || expr_complexity(new_candidate) < expr_complexity(*best_candidate)) {
              best_candidate = new_candidate;
              best_candidate_description = absl::StrCat(
                  fmt::coeff(sign), "cycle ", to_string(Permutation(substitutions_to_cycles(subst))));
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
      std::cout << groups_to_string(groups) << " => " << preshow(expr_degenerated) << fmt::newline();
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

LoopExpr reduce_arg9_loop_expr(const LoopExpr& expr) {
  return expr.filtered([](const Loops& loops) {
    CHECK_EQ(loops.size(), 3);
    return loops[1].size() == 4;
    // CHECK_EQ(loops[1].size(), 4);
    return true;
  }).mapped_expanding([&](const Loops& loops) -> LoopExpr {
    constexpr int kLoopTypeKnownToBeFullyAntisymmetric = 1;
    if (loops_names.loops_index(loops) == kLoopTypeKnownToBeFullyAntisymmetric) {
      const Loops canonical_form = {{1,2,3,4}, {1,2,5,3}, {1,2,5,6,7}};
      CHECK(loop_lengths(loops) == loop_lengths(canonical_form)) << dump_to_string(loops);
      const int main_sign = permutation_sign(decompose_loops(loops));
      // Distinguish between what used to be type 1 and type 4.
      const int bonus_sign = loops_unique_common_variable(loops, {0,2}).empty() ? 1 : -1;
      return main_sign * bonus_sign * LoopExpr::single(canonical_form);
    } else {
      return LoopExpr::single(loops);
    }
  });
}
#endif




#define DUMP(expr) std::cout << STRINGIFY(expr) << " " << expr



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

  // // TODO: Factor out as tests
  // for (const auto& expr : exprs_any_num_points) {
  //   const int n = detect_num_variables(expr);
  //   CHECK((a_minus(a_minus_minus(expr, n+1), n+2)).is_zero());
  //   CHECK((a_plus(a_plus_plus(expr, n+1), n+2)).is_zero());
  //   CHECK((b_minus(b_minus_minus(expr, n+1), n+2)).is_zero());
  //   CHECK((b_plus(b_plus_plus(expr, n+1), n+2)).is_zero());
  //   CHECK(a_plus(b_plus_plus(expr, n+1), n+2) == -b_plus(a_plus_plus(expr, n+1), n+2));
  //   CHECK(a_minus_minus(b_plus(expr, n+1), n+2) == -b_plus_plus(a_minus(expr, n+1), n+2));
  //   CHECK(a_minus(b_minus_minus(expr, n+1), n+2) == -b_minus(a_minus_minus(expr, n+1), n+2));
  //   CHECK(a_plus_plus(b_minus(expr, n+1), n+2) == -b_minus_minus(a_plus(expr, n+1), n+2));
  // }
  // std::cout << "ok\n";


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


  // const int n = 6;
  // const auto x = ncoproduct(GLi3(1,2,3,4,5,6), plucker({4,5,6}));
  // const auto y =
  //   + b_minus(a_minus_minus(x, n+1), n+2)
  //   + a_plus(b_plus_plus(x, n+1), n+2)
  //   + a_plus(b_minus(x, n+1), n+2)
  //   - a_plus(b_minus_minus(x, n+1), n+2)
  // ;
  // std::cout << to_lyndon_basis(a_full(y, n+3));



  // for (const int p : range_incl(3, 4)) {
  //   std::cout << to_lyndon_basis(
  //     + GLiVec(p, seq_incl(1, 2*p+2))
  //     - neg_one_pow(p-1) * a_plus(b_minus(GLiVec(p, seq_incl(1, 2*p)), 2*p+1), 2*p+2)
  //   );
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

  // for (const auto p : range_incl(3, 4)) {
  //   std::cout << to_lyndon_basis(
  //     a_full(
  //       + GLiVec(p, seq_incl(1, 2*p))
  //       + neg_one_pow(p-1) * a_plus(b_minus_minus(GLiVec(p, seq_incl(1, 2*p-2)), 2*p-1), 2*p),
  //       2*p+1
  //     )
  //   );
  // }
  // for (const auto p : range_incl(3, 4)) {
  //   std::cout << to_lyndon_basis(
  //     b_full(
  //       + GLiVec(p, seq_incl(1, 2*p))
  //       + neg_one_pow(p) * b_plus(a_minus_minus(GLiVec(p, seq_incl(1, 2*p-2)), 2*p-1), 2*p),
  //       2*p+1
  //     )
  //   );
  // }

  // std::cout << to_lyndon_basis(
  //   + ncoproduct(GLi3(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   - a_plus(b_minus(ncoproduct(GLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // );


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
  //   return sum(mapped(get_unsorted_partitions_allow_zero(weight - num_points + 1, num_points), [&](const auto& subweights) {
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
  //   }));
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


  // // TODO: Make this the canonical definition on ChernCocycle,
  // //   test that it's equal to the old definition.
  // // TODO: Test the functional equations too.
  // for (const int n : range_incl(3, 4)) {
  //   const auto c_n =
  //     + GLiVec(n, seq_incl(1, 2*n))
  //     + neg_one_pow(n) * b_plus(a_minus_minus(GLiVec(n, seq_incl(1, 2*n-2)), 2*n-1), 2*n);
  //   ;
  //   const auto c_n1 =
  //     + neg_one_pow(n) * a_plus(a_minus_minus(GLiVec(n, seq_incl(1, 2*n-2)), 2*n-1), 2*n);
  //   ;
  //   std::cout << to_lyndon_basis(a_full(c_n1, 2*n+1));
  //   std::cout << to_lyndon_basis(b_full(c_n1, 2*n+1) + a_full(c_n, 2*n+1));
  //   std::cout << to_lyndon_basis(b_full(c_n, 2*n+1));
  //   std::cout << to_lyndon_basis(
  //     + ncoproduct(c_n)
  //     - neg_one_pow(n) * ChernCocycle(n, n, seq_incl(1, 2*n))
  //   );
  //   std::cout << to_lyndon_basis(
  //     + ncoproduct(c_n1)
  //     - neg_one_pow(n) * ChernCocycle(n, n-1, seq_incl(1, 2*n))
  //   );
  // }


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
#if 1
  // loop_expr = arg9_semi_lyndon(loop_expr);
  loop_expr = to_canonical_permutation(arg9_semi_lyndon(loop_expr));

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

  // DUMP(proto_a);
  // DUMP(proto_b);
  // DUMP(proto_c);
  // DUMP(proto_d);
  // DUMP(proto_e);
  // DUMP(proto_f);
  // DUMP(proto_g);
  // DUMP(proto_h);
  // DUMP(proto_i);
  // DUMP(proto_j);
  // DUMP(proto_k);
  // DUMP(proto_l);
  // DUMP(proto_x);
  // DUMP(proto_y);
  // DUMP(proto_z);
  // DUMP(proto_u);
  // DUMP(proto_w);
  // DUMP(proto_m);
  // DUMP(proto_n);
  // DUMP(proto_o);
  // return 0;

  generate_loops_names({
    proto_a,
    proto_b,
    proto_c,
    proto_d,
    proto_e,
    proto_f,
    proto_g,
    proto_h,
    proto_i,
    proto_j,
    proto_k,
    proto_l,
    proto_x,
    proto_y,
    proto_z,
    proto_u,
    proto_w,
    proto_m,
    proto_n,
    proto_o,
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

  // DUMP(a);  // zero
  // DUMP(b);  // zero
  DUMP(c);
  DUMP(d);
  DUMP(e0);
  DUMP(f);
  // DUMP(g);  // zero
  DUMP(h);
  DUMP(i);
  DUMP(j);
  DUMP(k);
  DUMP(l);
  DUMP(x0);
  // DUMP(y);  // == d after cycle (1,2)(3,7,6,5,4)
  DUMP(z);
  DUMP(u);
  DUMP(w);
  // DUMP(m);  // zero
  // DUMP(n);  // zero
  // DUMP(o);  // zero

  std::vector<LoopExpr> space;
  // const auto expr = LoopExpr::single(d.element().first);
  const auto type_3_normalize =  // based on `d`
    + LoopExpr::single({{3,1,4,2}, {3,5,4,2}, {3,5,1,7,6}})
    - LoopExpr::single({{3,5,4,2}, {3,1,4,2}, {3,1,5,7,6}})
  ;
  for (const auto& perm : permutations(seq_incl(1, 7))) {
    for (const auto& expr : {
      // c,
      d,
      // e0,
      // f,
      // h,
      // i,
      // j,
      // k,
      // l,
      // x0,
      // z,
      // u,
      // w,
      type_3_normalize,
    }) {
      space.push_back(loop_expr_substitute(expr, perm));
    }

    // space.push_back(loop_expr_substitute(expr, perm));
  }
  auto rank = space_rank(space, DISAMBIGUATE(identity_function));
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

  // DUMP(d6);
  // DUMP(d7);
  // DUMP(d8);
  // DUMP(d67);
  // DUMP(d68);
#endif


  // const auto expr = QLi4(1,2,3,4,5,6);
  // std::cout << icomultiply(expr, {2,2});
  // const auto expr = QLi6(1,2,3,4,5,6,7,8);
  // std::cout << icomultiply(expr, {2,2,2});

  // const auto prepare = [](const auto& expr) {
  //   // return expr;
  //   return to_lyndon_basis(fully_normalize_loops(expr));
  // };
  // // auto loop_expr = cut_loops({1,2,3,4,5,6,7,8,9});
  // loop_expr = prepare(loop_expr);
  // auto s_expr = S({1,2,3,4,5,6,7,8,9});
  // s_expr = prepare(s_expr);
  // std::cout << s_expr;
  // std::cout << loop_expr;
  // std::cout << s_expr + loop_expr;  // ZERO

  // std::cout << Q({1,2,3,4,5,6});
  // std::cout << S({1,2,3,4,5,6,7});
#endif


}
