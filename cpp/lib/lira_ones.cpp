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
      [&](const std::vector<Theta>& /*term_product*/) -> LiraExpr {
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

DeltaICoExpr lira_expr_comultiply(const LiraExpr& expr) {
  return expr.mapped_expanding([](const LiraParamOnes& formal_symbol) {
    return icoproduct_vec(
      mapped(formal_symbol.ratios(), [](const auto& ratio_or_unity) {
        CHECK(!ratio_or_unity.is_unity());
        return QLiVec(2, ratio_or_unity.as_ratio().indices());
      })
    );
  });
}
