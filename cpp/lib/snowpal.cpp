#include "snowpal.h"

#include "absl/container/flat_hash_set.h"

#include "polylog_qli.h"


int num_distinct_ratio_variables(const std::vector<RatioOrUnity>& ratios) {
// TODO: What was meant here? `ratios_sorted` is unused!
  std::vector<std::array<int, 4>> ratios_sorted;
  for (const auto& r : ratios) {
    if (!r.is_unity()) {
      ratios_sorted.push_back(sorted(r.as_ratio().indices()));
    }
  };
  return num_distinct_elements_unsorted(ratios);
}

int num_ratio_points(const std::vector<Ratio>& ratios) {
  absl::flat_hash_set<int> all_points;
  for (const auto& r : ratios) {
    all_points.insert(r.indices().begin(), r.indices().end());
  }
  return all_points.size();
}

// TODO: Check if this is a valid criterion
// Optimiation potential: try to avoid exponential explosion
bool are_ratios_independent(const std::vector<Ratio>& ratios) {
  if (num_ratio_points(ratios) < ratios.size() + 3) {
    return false;
  }
  if (ratios.size() > 1) {
    for (int i : range(ratios.size())) {
      if (!are_ratios_independent(removed_index(ratios, i))) {
        return false;
      }
    }
  }
  return true;
}

// TODO: Use generic Lyndon utils here and below.
LiraExpr to_lyndon_basis_2(const LiraExpr& expr) {
  return expr.mapped_expanding([&](const LiraParamOnes& formal_symbol) {
    const auto& ratios = formal_symbol.ratios();
    CHECK_EQ(ratios.size(), 2);
    if (ratios[0] == ratios[1]) {
      // skip
      return LiraExpr{};
    } else if (ratios[0] < ratios[1]) {
      // already ok
      return LiraExpr::single(formal_symbol);
    } else {
      // swap:  ba -> ab
      return -LiraExpr::single(LiraParamOnes({ratios[1], ratios[0]}));
    }
  });
}

LiraExpr to_lyndon_basis_3(const LiraExpr& expr, LyndonMode mode) {
  // There are two equations in case of three ratios:
  //   * xyz == zyx
  //   * xyz + yxz + yzx == 0
  // First, apply the first one:
  LiraExpr expr_symm = expr.mapped([](const LiraParamOnes& formal_symbol) {
    auto ratios = formal_symbol.ratios();
    CHECK_EQ(ratios.size(), 3);
    sort_two(ratios[0], ratios[2]);
    return LiraParamOnes(ratios);
  });
  LiraExpr ret;
  expr_symm.foreach([&](const LiraParamOnes& formal_symbol, int coeff) {
    ret += [&]() -> LiraExpr {
      const auto original_expr = coeff * LiraExpr::single(formal_symbol);
      const auto& ratios = formal_symbol.ratios();
      CHECK_EQ(ratios.size(), 3);
      const int distinct = num_distinct_elements_unsorted(ratios);
      LiraExpr replacement;
      if (distinct == 3) {
        //  abc  acb  bac  bca  cab  cba  -- original expr
        //  abc  acb  bac  acb  bac  abc  -- expr_symm
        //  ^^^^^^^^  ^^^  ^^^^^^^^^^^^^
        //  already    |   duplicates
        //  Lyndon     |
        //             needs shuffle: (b)(ac) = bac + abc + acb

        if (ratios[1] >= ratios[0]) {
          return original_expr;
        } else {
          auto ratios1 = choose_indices(ratios, {1,0,2});
          auto ratios2 = choose_indices(ratios, {1,2,0});
          replacement.add_to(LiraParamOnes(ratios1), -1);
          replacement.add_to(LiraParamOnes(ratios2), -1);
        }
      } else if (distinct == 2) {
        //  aab  abb  aba  bab  baa  bba  -- original expr
        //  aab  abb  aba  bab  aab  abb  -- expr_symm
        //  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^
        //  already   |         duplicates
        //  Lyndon    |
        //            needs shuffle: (ab)(a) = aba + 2*aab
        //                           (b)(ab) = bab + 2*abb

        if (ratios[0] != ratios[2]) {
          return original_expr;
        } else if (ratios[1] >= ratios[0]) {
          auto ratios1 = choose_indices(ratios, {0,2,1});
          replacement.add_to(LiraParamOnes(ratios1), -2);
        } else {
          auto ratios1 = choose_indices(ratios, {1,0,2});
          replacement.add_to(LiraParamOnes(ratios1), -2);
        }
      } else if (distinct == 1) {
        // skip: zero
        return LiraExpr{};
      } else {
        FATAL(absl::StrCat("Bad number of distinct elements: ", distinct));
      }
      replacement *= coeff;
      CHECK(!replacement.is_zero());
      if (mode == LyndonMode::hard ||
          (expr_symm - original_expr + replacement).l1_norm() <= expr_symm.l1_norm()) {
        return replacement;
      } else {
        return original_expr;
      }
    }();
  });
  return ret;
}

// Alternative to Lyndon basis.
/*
template<typename Container>
std::vector<Container> all_permutations(const Container& c) {
  std::vector<int> indices(c.size());
  absl::c_iota(indices, 0);
  std::vector<Container> ret;
  do {
    ret.push_back(choose_indices(c, indices));
  } while (absl::c_next_permutation(indices));
  return ret;
}

std::pair<LiraExpr, LiraExpr> lira_expr_cancel_shuffle(const LiraExpr& expr) {
  using Key = std::array<RatioOrUnity, 3>;
  struct Value {
    LiraParam term;
    int coeff;
  };
  static auto lira_param_to_key = [](const LiraParam& term) {
    return to_array<3>(mapped(term.ratios(), to_cross_ratio_or_unity));
  };
  absl::flat_hash_map<Key, std::vector<Value>> sorted_ratios;
  expr.foreach([&](const LiraParam& term, int coeff) {
    const Key key = sorted(lira_param_to_key(term));
    sorted_ratios[key].push_back({term, coeff});
  });

  LiraExpr no_shuffle_expr;
  LiraExpr shuffle_expr;
  for (const auto& [key, value] : sorted_ratios) {
    // Consider: use Lyndon basis
    // CHECK_LE(value.size(), 3);  // this is probably possible, but not supported by shuffle finder yet
    if (value.size() > 3) {
      std::cout << "WARNING: some shuffle relations may have been missed\n";
    }
    bool is_shuffle = false;
    if (value.size() == 3) {
      if (num_distinct_elements_unsorted(mapped(value, [](const Value& v) { return v.coeff; })) == 1) {
        auto keys = mapped(value, [](const Value& v) {
          return lira_param_to_key(v.term);
        });
        for (const auto& k : all_permutations(keys)) {
          // Check  0 == ABC + BAC + BCA == A ⧢ BC
          if (all_equal({k[0][0], k[1][1], k[2][2]}) &&
              all_equal({k[0][1], k[1][0], k[2][0]}) &&
              all_equal({k[0][2], k[1][2], k[2][1]})) {
            is_shuffle = true;
            break;
          }
        }
      }
    }
    if (value.size() == 2) {
      auto keys = to_array<2>(mapped(value, [](const Value& v) {
        return lira_param_to_key(v.term);
      }));
      // Check  0 == ABC - CBA
      if (keys == reversed(keys) && value[0].coeff == -value[1].coeff) {
        is_shuffle = true;
      }
    }
    for (const auto& [term, coeff] : value) {
      if (is_shuffle) {
        shuffle_expr.add_to(term, coeff);
      } else {
        no_shuffle_expr.add_to(term, coeff);
      }
    }
  }
  return {no_shuffle_expr, shuffle_expr};
}
*/

LiraExpr without_unities(const LiraExpr& expr) {
  return expr.filtered([](const LiraParamOnes& formal_symbol) {
    return absl::c_none_of(formal_symbol.ratios(), [](const RatioOrUnity& r) {
      return r.is_unity();
    });
  });
}

LiraExpr keep_distinct_ratios(const LiraExpr& expr) {
  return expr.filtered([](const LiraParamOnes& formal_symbol) {
    return num_distinct_ratio_variables(formal_symbol.ratios()) ==
        formal_symbol.ratios().size();
  });
}

LiraExpr keep_independent_ratios(const LiraExpr& expr) {
  return expr.filtered([](const LiraParamOnes& formal_symbol) {
    std::vector<Ratio> ratios;
    for (const auto& r : formal_symbol.ratios()) {
      if (r.is_unity()) {
        return false;
      }
      ratios.push_back(r.as_ratio());
    }
    return are_ratios_independent(ratios);
  });
}

LiraExpr normalize_inverse(const LiraExpr& expr) {
  return expr.mapped_expanding([&](const LiraParamOnes& formal_symbol) {
    static auto is_normal = [](const RatioOrUnity& r) {
      return r.is_unity() || r.as_ratio()[1] <= r.as_ratio()[3];
    };
    auto ratios = formal_symbol.ratios();
    // Idea: choose a marker independently from position, so that the choice
    //   wouldn't be affected by Lyndon.
    // Note: if ratios.size() is odd, this would work as well:
    //   const int normal_ratios = absl::c_count_if(ratios, is_normal);
    //   if (normal_ratios <= ratios.size() / 2) { ... }
    const auto marker_it = absl::c_max_element(
      ratios,
      [](const RatioOrUnity& r1, const RatioOrUnity& r2) {
        if (r2.is_unity()) {
          return false;
        }
        if (r1.is_unity()) {
          return true;
        }
        return sorted(r1.as_ratio().indices()) < sorted(r2.as_ratio().indices());
      }
    );
    CHECK(marker_it != ratios.end());
    if (is_normal(*marker_it)) {
      for (auto& r : ratios) {
        if (!r.is_unity()) {
          r = Ratio::inverse(r.as_ratio());
        }
      }
      return neg_one_pow(ratios.size()) * LiraExpr::single(LiraParamOnes(ratios));
    } else {
      return LiraExpr::single(formal_symbol);
    }
  });
}

LiraExpr fully_normalize_ratios(const LiraExpr& expr) {
  return expr.mapped_expanding([&](const LiraParamOnes& formal_symbol) -> LiraExpr {
    int sign = 1;
    auto new_ratios = mapped(formal_symbol.ratios(), [&](const RatioOrUnity& r) -> RatioOrUnity {
      if (r.is_unity()) {
        return r;
      }
      auto indices = r.as_ratio().indices();
      sign *= sort_with_sign(indices);
      return Ratio(indices);
    });
    return sign * LiraExpr::single(LiraParamOnes(new_ratios));
  });
}

DeltaCoExpr lira_expr_comultiply(const LiraExpr& expr) {
  return expr.mapped_expanding([](const LiraParamOnes& formal_symbol) {
    return coproduct_vec<DeltaCoExpr>(
      mapped(formal_symbol.ratios(), [](const auto& ratio_or_unity) {
        CHECK(!ratio_or_unity.is_unity());
        return QLiVec(2, ratio_or_unity.as_ratio().indices());
      })
    );
  });
}

std::variant<const SplittingTree::Node*, std::array<int, 2>> find_central_node(
    const std::vector<int>& points,
    const SplittingTree::Node* node) {
  CHECK_EQ(points.size(), 4);
  CHECK_GE(set_intersection_size(points, node->points), 3);
  for (const auto& child : node->children) {
    const auto points_in_child = set_intersection(points, child->points);
    if (points_in_child.size() >= 3) {
      return find_central_node(points, child.get());
    } else if (points_in_child.size() == 2) {
      return to_array<2>(points_in_child);
    }
  }
  // Neither child has more than one point => this is the central point.
  return node;
}

RatioSubstitutionResult ratio_substitute(
    const RatioOrUnity& ratio,
    const SplittingTree& tree) {
  if (ratio.is_unity()) {
    return ratio;
  }
  const auto& old_points = ratio.as_ratio().indices();
  const auto central_node_or_bad_pair =
    find_central_node(to_vector(sorted(old_points)), tree.root());
  return std::visit(overloaded{
    [&](const SplittingTree::Node* central_node) -> RatioSubstitutionResult {
      CHECK_GE(central_node->valency(), 4);
      auto new_points = mapped_array(old_points, [&](int p) {
        return central_node->metavar_for_point(p);
      });
      return RatioOrUnity(Ratio(new_points));
    },
    [&](std::array<int, 2> bad_pair) -> RatioSubstitutionResult {
      const int dist = std::abs(
        element_index(old_points, bad_pair[0]) -
        element_index(old_points, bad_pair[1])
      );
      CHECK(1 <= dist && dist <= 3) << dist;
      if (dist == 2) {
        return RatioOrUnity::unity();
      } else {
        return ZeroOrInf{};
      }
    },
  }, central_node_or_bad_pair);
}



LiraExpr theta_expr_to_lira_expr_without_products(const ThetaExpr& expr) {
  return expr.mapped_expanding([&](const ThetaPack& term) {
    return std::visit(overloaded{
      [&](const std::vector<Theta>& term_product) -> LiraExpr {
        FATAL("Unexpected std::vector<Theta> when converting to LiraExpr");
      },
      [&](const LiraParam& formal_symbol) {
        if (absl::c_all_of(formal_symbol.weights(), [](int w) { return w == 1; })) {
          CHECK_EQ(formal_symbol.foreweight(), formal_symbol.weights().size());
          LiraParamOnes new_formal_symbol(mapped(formal_symbol.ratios(), to_cross_ratio_or_unity));
          return LiraExpr::single(new_formal_symbol);
        } else {
          // ignore all products
          return LiraExpr{};
        }
      },
    }, term);
  });
}

LiraExpr lira_expr_substitute(
    const LiraExpr& expr,
    const SplittingTree& tree) {
  return expr.mapped_expanding([&](const LiraParamOnes& formal_symbol) {
    std::vector<RatioOrUnity> new_ratios;
    for (const RatioOrUnity& ratio: formal_symbol.ratios()) {
      auto ratio_subst = ratio_substitute(ratio, tree);
      if (std::holds_alternative<RatioOrUnity>(ratio_subst)) {
        new_ratios.push_back(std::get<RatioOrUnity>(ratio_subst));
      } else {
        return LiraExpr{};
      }
    }
    return LiraExpr::single(LiraParamOnes(std::move(new_ratios)));
  });
}


Snowpal& Snowpal::add_ball(std::vector<int> points) {
  CHECK(!points.empty());
  absl::c_sort(points);
  CHECK_EQ(points.size(), num_distinct_elements_unsorted(points)) << dump_to_string(points);
  auto* node = splitting_tree_.node_for_points(points);
  node->split(points, splitting_tree_);
  expr_ = lira_expr_substitute(orig_expr_, splitting_tree_);
  expr_ = without_unities(expr_);
  expr_ = fully_normalize_ratios(expr_);
  expr_ = keep_distinct_ratios(expr_);
  // expr_ = keep_independent_ratios(expr_);
  // expr_ = normalize_inverse(expr_);
  // expr_ = to_lyndon_basis_2(expr_);
  expr_ = to_lyndon_basis_3(expr_, LyndonMode::soft);
  return *this;
}

std::ostream& to_ostream(std::ostream& os, const LiraExpr& expr, const SplittingTree& splitting_tree) {
  ShortFormRatioStorage short_forms{splitting_tree.generate_short_form_ratios()};
  expr.foreach([&](const LiraParamOnes& param, int) {
    for (const auto& r : param.ratios()) {
      if (!r.is_unity()) {
        short_forms.record_usage_stats(r.as_ratio());
      }
    }
  });
  short_forms.update_normal_forms();
  to_ostream(os, expr, std::less<>{}, &short_forms);
  const auto nbr_indices_per_vertex = splitting_tree.dump_nbr_indices();
  if (nbr_indices_per_vertex.size() >= 2) {
    os << "^^^\n";
    for (const auto& [node_index, nbr_indices] : nbr_indices_per_vertex) {
      std::vector<std::string> index_strs;
      for (int i : range(nbr_indices.size())) {
        const std::vector<int>& indices = nbr_indices[i];
        index_strs.push_back(absl::StrCat(
          metavar_to_string_by_name(make_metavar(node_index, i+1)),
          "=",
          fmt::braces(str_join(indices, ","))
        ));
      }
      os << " " << str_join(index_strs, " ") << "\n";
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Snowpal& snowpal) {
  return to_ostream(os, snowpal.expr(), snowpal.splitting_tree());
}
