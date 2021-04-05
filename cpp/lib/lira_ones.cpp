#include "lira_ones.h"

#include "polylog_qli.h"


int num_distinct_ratio_variables(const std::vector<CrossRatioNOrUnity>& ratios) {
// TODO: What was meant here? `ratios_sorted` is unused!
  std::vector<std::array<int, 4>> ratios_sorted;
  for (const auto& r : ratios) {
    if (!r.is_unity()) {
      ratios_sorted.push_back(sorted(r.as_ratio().indices()));
    }
  };
  return num_distinct_elements_unsorted(ratios);
}

int num_ratio_points(const std::vector<CrossRatioN>& ratios) {
  absl::flat_hash_set<int> all_points;
  for (const auto& r : ratios) {
    all_points.insert(r.indices().begin(), r.indices().end());
  }
  return all_points.size();
}

// TODO: Check if this is a valid criterion
// Optimiation potential: try to avoid exponential explosion
bool are_ratios_independent(const std::vector<CrossRatioN>& ratios) {
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
  using Key = std::array<CrossRatioNOrUnity, 3>;
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
    return absl::c_none_of(formal_symbol.ratios(), [](const CrossRatioNOrUnity& r) {
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
    std::vector<CrossRatioN> ratios;
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
    static auto is_normal = [](const CrossRatioNOrUnity& r) {
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
      [](const CrossRatioNOrUnity& r1, const CrossRatioNOrUnity& r2) {
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
          r = CrossRatioN::inverse(r.as_ratio());
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
    auto new_ratios = mapped(formal_symbol.ratios(), [&](const CrossRatioNOrUnity& r) -> CrossRatioNOrUnity {
      if (r.is_unity()) {
        return r;
      }
      auto indices = r.as_ratio().indices();
      sign *= sort_with_sign(indices);
      return CrossRatioN(indices);
    });
    return sign * LiraExpr::single(LiraParamOnes(new_ratios));
  });
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

DeltaCoExpr lira_expr_comultiply(const LiraExpr& expr) {
  return expr.mapped_expanding([](const LiraParamOnes& formal_symbol) {
    return coproduct_vec(
      mapped(formal_symbol.ratios(), [](const auto& ratio_or_unity) {
        CHECK(!ratio_or_unity.is_unity());
        return QLiVec(2, ratio_or_unity.as_ratio().indices());
      })
    );
  });
}
