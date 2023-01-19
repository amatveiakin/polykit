#include "loops_aux.h"

#include "set_util.h"


static constexpr auto cycle = loop_expr_cycle;
static constexpr auto keep_term = loop_expr_keep_term_type;

static int num_distinct_term_types(const LoopExpr& expr) {
  std::vector<int> term_types;
  expr.foreach([&](const Loops& loops, int) {
    term_types.push_back(loop_kinds.loops_index(loops));
  });
  return num_distinct_elements_unsorted(term_types);
}

static std::string groups_to_string(const std::vector<std::vector<int>>& groups) {
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

// TODO: Normalize permutation cycles (here or in `Permutation` constructor).
static Permutation substitutions_to_permutation(absl::flat_hash_map<int, int> substitutions) {
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
  return Permutation(ret);
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
    if (loop_kinds.loops_index(loops) == target_type) {
      killer_terms.push_back(loops);
    }
  });
  CHECK(!killer_terms.empty()) << "Term type " << target_type << " not found in\n" << killer;
  bool stuck = false;
  while (!stuck) {
    stuck = true;
    std::vector<Loops> victim_terms;
    victim.foreach([&](const Loops& loops, int) {
      if (loop_kinds.loops_index(loops) == target_type) {
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
        const auto subst = loop_expr_recover_substitution(killer_term, victim_term);
        const auto killer_subst = loop_expr_substitute(killer, subst);
        for (int sign : {-1, 1}) {
          const auto new_candidate = victim + sign * killer_subst;
          if (keep_term(new_candidate, target_type).l1_norm() < keep_term(victim, target_type).l1_norm()) {
            // std::cout << ">>> " << absl::StrCat(fmt::coeff(sign), "cycle ", to_string(substitutions_to_permutation(subst)))
            //     << " : " << new_candidate << " => " << dump_to_string(expr_complexity(new_candidate)) << "\n\n";
            if (!best_candidate || expr_complexity(new_candidate) < expr_complexity(*best_candidate)) {
              best_candidate = new_candidate;
              best_candidate_description = absl::StrCat(
                  fmt::coeff(sign), "cycle ", to_string(substitutions_to_permutation(subst)));
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

using Degenerations = std::vector<std::vector<int>>;

static bool is_degenerations_ok(const Degenerations& groups) {
  for (int i : range(groups.size())) {
    for (int j : range(i+1, groups.size())) {
      if (set_intersection_size(groups[i], groups[j]) > 0) {
        return false;
      }
    }
  }
  return true;
}

static void normalize_degenerations(Degenerations& groups) {
  for (auto& group : groups) {
    absl::c_sort(group);
  }
  absl::c_sort(groups);
}

static Degenerations normalized_degenerations(Degenerations groups) {
  normalize_degenerations(groups);
  return groups;
}

static Degenerations flip_variables(const Degenerations& groups, int total_vars) {
  return mapped(groups, [&](const auto& group) {
    return mapped(group, [&](int v) {
      return total_vars + 1 - v;
    });
  });
}

static Degenerations rotate_variables(const Degenerations& groups, int total_vars, int shift) {
  return mapped(groups, [&](const auto& group) {
    return mapped(group, [&](int v) {
      return (v - 1 + shift) % total_vars + 1;
    });
  });
}

// static bool contains_only_expression_of_type(const LoopExpr& expr, const std::vector<int>& types) {
//   absl::flat_hash_set<int> types_set(types.begin(), types.end());
//   bool ret = true;
//   expr.foreach([&](const auto& loops, int) {
//     if (!types_set.contains(loop_kinds.loops_index(loops))) {
//       ret = false;
//       return;
//     }
//   });
//   return ret;
// }

static bool has_common_variable_in_each_term(const LoopExpr& expr) {
  bool ret = true;
  expr.foreach([&](const auto& loops, int) {
    if (set_intersection(loops).empty()) {
      ret = false;
      return;
    }
  });
  return ret;
}

void arg11_list_all_degenerations(const LoopExpr& expr) {
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
      std::cout << groups_to_string(groups) << " => " << expr_degenerated << fmt::newline();
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

LoopExpr arg9_kill_middle(const LoopExpr& expr) {
  return expr.mapped_expanding([&](auto loops) -> LoopExpr {
    CHECK_EQ(loops.size(), 3);
    int sign = 1;
    if (loops[0].size() == loops[1].size()) {
      if (loops[0] > loops[1]) {
        sign *= -1;
        std::swap(loops[0], loops[1]);
      }
    }
    return sign * LoopExpr::single(loops);
  });
}

LoopExpr reduce_arg9_loop_expr(const LoopExpr& expr) {
  return expr.filtered([](const Loops& loops) {
    CHECK_EQ(loops.size(), 3);
    return loops[1].size() == 4;
    return true;
  }).mapped_expanding([&](const Loops& loops) -> LoopExpr {
    // TODO: Add an asserting for the type OR fetch the type dynamically.
    constexpr int kLoopTypeKnownToBeFullyAntisymmetric = 1;
    if (loop_kinds.loops_index(loops) == kLoopTypeKnownToBeFullyAntisymmetric) {
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
