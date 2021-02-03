#include "loops.h"

#include <sstream>

#include "algebra.h"
#include "set_util.h"


LoopsNames loops_names;


std::string loops_description(const Loops& loops) {
  absl::flat_hash_set<int> fully_common = to_set(set_intersection(loops));
  absl::flat_hash_set<int> partially_common;
  for (int i = 0; i < loops.size(); ++i) {
    for (int j = i+1; j < loops.size(); ++j) {
      auto c = set_intersection(loops[i], loops[j]);
      partially_common.insert(c.begin(), c.end());
    }
  }
  return str_join(
    mapped(loops, [&](const std::vector<int>& loop) {
      return fmt::brackets(
        str_join(loop, ",", [&](int p) {
          const auto p_str = to_string(p);
          return fully_common.contains(p)
            ? fmt::colored(p_str, TextColor::bright_red)
            : partially_common.contains(p)
              ? fmt::colored(p_str, TextColor::bright_blue)
              : p_str;
        })
      );
    }),
    ""
  );
}

LoopsInvariant loops_invariant(Loops loops) {
  std::vector<int> five_loop;
  const auto it = absl::c_find_if(loops, [](const std::vector<int>& loop) {
    return loop.size() == 5;}
  );
  CHECK(it != loops.end());
  std::vector<int> invariant{static_cast<int>(it - loops.begin())};
  five_loop = *it;
  loops.erase(it);
  for (int num_loops_to_include = 2; num_loops_to_include <= loops.size(); ++num_loops_to_include) {
    for (const auto include_five_loop : {false, true}) {
      std::vector<int> loops_common_elements;
      for (const auto& loops_to_include : increasing_sequences(loops.size(), num_loops_to_include)) {
        Loops loops_group;
        if (include_five_loop) {
          loops_group.push_back(five_loop);
        }
        for (const int idx : loops_to_include) {
          loops_group.push_back(loops.at(idx));
        }
        loops_common_elements.push_back(set_intersection_size(loops_group));
      }
      invariant = concat(invariant, sorted(loops_common_elements));
    }
  }
  return invariant;
}

int LoopsNames::loops_index(const Loops& loops) {
  const auto invariant = loops_invariant(loops);
  if (!indices_.contains(invariant)) {
    indices_[invariant] = next_idx_++;
  }
  return indices_.at(invariant);
}

std::string LoopsNames::loops_name(const Loops& loops) {
  return fmt::braces(
    fmt::colored(
      pad_left(to_string(loops_index(loops)), 2),
      TextColor::bright_cyan
    )
  );
}

std::string LoopExprParam::object_to_string(const ObjectT& loops) {
  const std::string loops_str = loops_description(loops);
  const std::string loops_name_str = loops_names.loops_name(loops);
  return absl::StrCat(loops_str, "  ", loops_name_str);
}

void generate_loops_names(const std::vector<LoopExpr>& expressions) {
  for (const auto& expr : expressions) {
    std::stringstream() << expr;
  }
}

LiraExpr loop_expr_to_lira_expr(const LoopExpr& expr) {
  return expr.mapped_expanding([](const auto& loops) {
    using RatioExpr = Linear<SimpleLinearParam<std::vector<RatioOrUnity>>>;
    const std::vector<RatioExpr> ratio_exprs =
      mapped(loops, [](const std::vector<int>& loop) -> RatioExpr {
        switch (loop.size()) {
          case 4: {
            return RatioExpr::single({Ratio(loop)});
          }
          case 5: {
            RatioExpr ret;
            for (int i = 0; i < 5; ++i) {
              ret += neg_one_pow(i) * RatioExpr::single({Ratio(removed_index(loop, i))});
            }
            return ret;
          }
          default:
            FATAL(absl::StrCat("Unsupported loop size: ", loop.size()));
        }
      });
    return outer_product(absl::MakeConstSpan(ratio_exprs),
      [](const auto& lhs, const auto& rhs) {
        return concat(lhs, rhs);
      }, AnnNone()
    ).template mapped<LiraExpr>([](const auto& loops) {
      return LiraParamOnes(loops);
    });
  });
}

LoopExpr fully_normalize_loops(const LoopExpr& expr) {
  return expr.mapped_expanding([&](const auto& loops) -> LoopExpr {
    int sign = 1;
    auto new_loops = mapped(loops, [&](std::vector<int> loop) {
      sign *= sort_with_sign(loop);
      return loop;
    });
    return sign * LoopExpr::single(new_loops);
  });
}

LoopExpr remove_duplicate_loops(const LoopExpr& expr) {
  return expr.filtered([&](const auto& loops) {
    for (int i = 0; i < loops.size(); ++i) {
      for (int j = 0; j < loops.size(); ++j) {
        if (i == j) {
          continue;
        }
        const auto outer = loops[i];
        const auto inner = loops[j];
        CHECK(absl::c_is_sorted(outer));
        CHECK(absl::c_is_sorted(inner));
        if (absl::c_includes(outer, inner)) {
          return false;
        }
      }
    }
    return true;
  });
}

LoopExpr loop_expr_substitute(const LoopExpr& expr, const absl::flat_hash_map<int, int>& substitutions) {
  auto loop_subst = expr.mapped_expanding([&](const Loops& loops) -> LoopExpr {
    Loops new_loops;
    for (const auto& loop : loops) {
      auto new_loop = mapped(loop, [&](int p) {
        return substitutions.contains(p) ? substitutions.at(p) : p;
      });
      if (!all_unique(new_loop)) {
        return {};
      }
      new_loops.push_back(std::move(new_loop));
    }
    return LoopExpr::single(new_loops);
  });
  return to_canonical_permutation(arg9_semi_lyndon(remove_duplicate_loops(fully_normalize_loops(loop_subst))));
  // return arg9_semi_lyndon(remove_duplicate_loops(fully_normalize_loops(loop_subst)));
}

LoopExpr loop_expr_substitute(const LoopExpr& expr, const std::vector<int>& new_indices) {
  absl::flat_hash_map<int, int> substitutions;
  for (int i = 0; i < new_indices.size(); ++i) {
    substitutions[i+1] = new_indices[i];
  }
  return loop_expr_substitute(expr, substitutions);
}

LoopExpr loop_expr_cycle(
    const LoopExpr& expr, const std::vector<std::vector<int>>& cycles) {
  absl::flat_hash_map<int, int> substitutions;
  for (const auto& cycle : cycles) {
    for (int i = 0; i < cycle.size(); ++i) {
      const int p = cycle[i];
      const int q = cycle[(i+1) % cycle.size()];
      CHECK(!substitutions.contains(p));
      substitutions[p] = q;
    }
  }
  return loop_expr_substitute(expr, substitutions);
}

static LoopExpr arg11_shuffle_group3(const LoopExpr& group) {
  CHECK(group.size() == 3) << group;
  const int coeff = group.element().second;
  absl::flat_hash_map<std::vector<int>, std::vector<int>> loop_positions;
  group.foreach([&](const Loops& loops, int term_coeff) {
    CHECK_EQ(coeff, term_coeff) << group;
    for (int i = 0; i < loops.size(); ++i) {
      loop_positions[loops[i]].push_back(i);
    }
  });
  std::vector<int> running_loop;
  int running_loop_new_position = -1;
  for (const auto& [loop, positions] : loop_positions) {
    if (all_unique(positions)) {
      CHECK(running_loop.empty()) << group;
      running_loop = loop;
      auto missing_positions = set_difference({0,1,2,3}, positions);
      CHECK_EQ(missing_positions.size(), 1) << list_to_string(positions) << "\n" << group;
      running_loop_new_position = missing_positions.front();
    }
  }
  CHECK(!running_loop.empty()) << group;
  auto shuffled = group.element().first;
  shuffled.erase(absl::c_find(shuffled, running_loop));
  shuffled.insert(shuffled.begin() + running_loop_new_position, running_loop);
  return coeff * LoopExpr::single(shuffled);
}

static LoopExpr arg11_shuffle_cluster(const LoopExpr& expr) {
  if (expr.zero() || expr.element().first.size() != 4) {
    return expr;  // TODO: Generalize !!!
  }

  struct LoopsAndCoeff {
    Loops value;
    int coeff;
  };
  absl::flat_hash_map<Loops, LoopExpr> loops_groups;
  const auto expr_normalized = fully_normalize_loops(expr);
  expr_normalized.foreach([&](const Loops& loops, int coeff) {
    loops_groups[sorted(loops)].add_to(loops, coeff);
  });
  LoopExpr ret;
  for (const auto& [_, group] : loops_groups) {
    if (group.size() == 1 || group.size() == 2 || group.size() == 4) {
      ret += group;
      continue;
    }
    if (group.size() == 6) {
      std::vector<std::pair<Loops, int>> group_elements;
      group.foreach([&](const Loops& loops, int coeff) {
        group_elements.push_back({loops, coeff});
      });
      // Sorting gives:
      //   a b c
      //   a c b
      //   b a c
      //   b c a
      //   c a b
      //   c b a
      // which splits into shuffle perfectly.
      absl::c_sort(group_elements);
      for (const auto& subgroup_elements: {
            slice(group_elements, 0, 3),
            slice(group_elements, 3, 6),
          }) {
        LoopExpr subgroup;
        for (const auto& [element, coeff] : subgroup_elements) {
          subgroup.add_to(element, coeff);
        }
        ret += arg11_shuffle_group3(subgroup);
      }
    } else {
      ret += arg11_shuffle_group3(group);
    }
  }
  return to_canonical_permutation(ret);
}

LoopExpr loop_expr_degenerate(
    const LoopExpr& expr, const std::vector<std::vector<int>>& groups) {
  std::set<int> remaining_points;
  expr.foreach([&](const Loops& loops, int) {
    for (const auto& loop : loops) {
      remaining_points.insert(loop.begin(), loop.end());
    }
  });
  int next_idx = 1;
  absl::flat_hash_map<int, int> substitutions;
  for (const auto& group : groups) {
    for (int p : group) {
      CHECK(remaining_points.count(p));
      remaining_points.erase(p);
      substitutions[p] = next_idx;
    }
    next_idx++;
  }
  for (int p : remaining_points) {
    substitutions[p] = next_idx++;
  }
  // return loop_expr_substitute(expr, substitutions);
  return arg11_shuffle_cluster(loop_expr_substitute(expr, substitutions));
}

std::vector<int> loops_unique_common_variable(const Loops& loops, std::vector<int> loop_indices) {
  absl::c_sort(loop_indices);
  Loops target_batch = choose_indices(loops, loop_indices);
  std::vector<int> higher_level_common;
  for (const auto& seq : all_sequences(2, loops.size())) {
    bool skip = false;
    for (int i = 0; i < seq.size(); ++i) {
      if (seq[i] && absl::c_binary_search(loop_indices, i)) {
        skip = true;
        break;
      }
    }
    if (skip) {
      continue;
    }
    const Loops loops_batch = concat(target_batch, choose_by_mask(loops, seq));
    if (loops_batch.size() == target_batch.size()) {
      continue;
    }
    higher_level_common = set_union(higher_level_common, set_intersection(loops_batch));
  }
  return set_difference(set_intersection(target_batch), higher_level_common);
}

std::vector<int> decompose_loops(const Loops& loops) {
  std::vector<int> ret;
  for (const auto& seq : increasing_sequences(loops.size())) {
    if (!seq.empty()) {
      append_vector(ret, loops_unique_common_variable(loops, seq));
    }
  }
  return reversed(ret);
}

LoopExpr to_canonical_permutation(const LoopExpr& expr) {
  return expr.mapped_expanding([](const Loops& loops) {
    const std::vector<int> full_permutation = decompose_loops(loops);
    int sign = 1;
    // Optimization potential: inverse into a vector instead.
    absl::flat_hash_map<int, int> position_of;
    for (int i = 0; i < full_permutation.size(); ++i) {
      position_of[full_permutation[i]] = i + 1;
    }
    Loops new_loops;
    for (const auto& loop : loops) {
      const std::vector<std::pair<int, int>> loop_permutation_orig = mapped(loop, [&](int p) {
        return std::pair{position_of.at(p), p};
      });
      auto loop_permutation = loop_permutation_orig;
      const int loop_permutation_sign = sort_with_sign(loop_permutation);
      sign *= loop_permutation_sign;
      new_loops.push_back(mapped(loop_permutation, [&](auto p) { return p.second; }));
    }
    return sign * LoopExpr::single(new_loops);
  });
}

LiraExpr lira_expr_sort_args(const LiraExpr& expr) {
  return expr.mapped_expanding([](const LiraParamOnes& term) {
    int sign = 1;
    auto lira = LiraParamOnes(mapped(term.ratios(), [&](const RatioOrUnity& r) -> RatioOrUnity {
      if (r.is_unity()) {
        return r;
      }
      auto indices = r.as_ratio().indices();
      sign *= sort_with_sign(indices);
      return Ratio(indices);
    }));
    return sign * LiraExpr::single(lira);
  });
}

struct LoopCutResult {
  std::vector<int> loop;
  std::vector<int> remains;
};

static LoopCutResult cut_loop(const std::vector<int>& points, int from, int to) {
  CHECK_LT(from, to);
  return {
    slice(points, from, to),
    concat(slice(points, 0, from + 1), slice(points, to - 1))
  };
}

LoopExpr cut_loops_recursively(const std::vector<int>& points, const Loops& loops) {
  if (points.size() <= 5) {
    CHECK_EQ(points.size(), 5);
    return LoopExpr::single(appended(loops, points));
  }
  LoopExpr ret;
  for (int i = 0; i < points.size(); ++i) {
    auto cut_result = cut_loop(rotated_vector(points, i), 0, 4);
    ret += cut_loops_recursively(cut_result.remains, appended(loops, cut_result.loop));
  }
  return ret;
}

LoopExpr cut_loops(const std::vector<int>& points) {
  CHECK(points.size() >= 5 && points.size() % 2 == 1);
  return cut_loops_recursively(points, {});
}

LoopExpr reverse_loops(const LoopExpr& expr) {
  return expr.mapped([](const Loops& loops) {
    return reversed(loops);
  });
}


// TODO: Clean up this super ad hoc function.
LoopExpr arg9_semi_lyndon(const LoopExpr& expr) {
  return expr.mapped([&](auto loops) {
    // CHECK_EQ(loops.size(), 3);
    if (loops.size() != 3) {
      // TODO: More robust solution
      return loops;
    }
    if (loops[0].size() == loops[2].size()) {
      sort_two(loops[0], loops[2]);
    }
    return loops;
  });
}