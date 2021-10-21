#include "gamma.h"

#include "set_util.h"


std::string to_string(const Gamma& g) {
  return fmt::parens(str_join(g.index_vector(), ","));
}

// Checks that points in `g1` and `g2` are weakly separated, e.g. that there is no such
// points {xa, ya} in (A = g1 \ g2) and points {xb, yb} in (B = g2 \ g1) that segment x1-y1
// intersects segment x2-y2 if all drawn as vertices of a polygon.
//
// Algoritm. Go around the polygon. Ignore vertices that don't belong to either A or B.
// Count the number of times that we "change color", e.g. see a point from A after we've
// seen a point from B or vice versa. If A and B are non-intersecting, we'll see at most
// 3 color changes, if not - at least 4.
//
// Optimization potential: O(1) algorithm via bit arithmetic: std::countl_zero, std::countr_zero, etc.
bool are_weakly_separated(const Gamma& g1, const Gamma& g2) {
  const auto a = bitset_difference(g1.index_bitset(), g2.index_bitset());
  const auto b = bitset_difference(g2.index_bitset(), g1.index_bitset());
  if (a.none() || b.none()) {
    return true;
  }
  int last_color = 0;  // a: 1,  b: 2
  int num_color_changes = 0;
  static_assert(a.size() == b.size());
  for (const int p : range(a.size())) {
    const int color = a[p] ? 1 : (b[p] ? 2 : 0);
    if (color != 0) {
      if (last_color != color) {
        ++num_color_changes;
        last_color = color;
      }
    }
  }
  CHECK_LE(2, num_color_changes);
  return num_color_changes <= 3;
}

// Optimization potential: consider whether this can be done in O(N) time;
bool is_weakly_separated(const GammaExpr::ObjectT& term) {
  for (int i : range(term.size())) {
    for (int j : range(i)) {
      if (!are_weakly_separated(term[i], term[j])) {
        return false;
      }
    }
  }
  return true;
}
bool is_weakly_separated(const GammaNCoExpr::ObjectT& term) {
  return is_weakly_separated(flatten(term));
}

bool is_totally_weakly_separated(const GammaExpr& expr) {
  return !expr.contains([](const auto& term) { return !is_weakly_separated(term); });
}
bool is_totally_weakly_separated(const GammaNCoExpr& expr) {
  return !expr.contains([](const auto& term) { return !is_weakly_separated(term); });
}

GammaExpr keep_non_weakly_separated(const GammaExpr& expr) {
  return expr.filtered([](const auto& term) { return !is_weakly_separated(term); });
}
GammaNCoExpr keep_non_weakly_separated(const GammaNCoExpr& expr) {
  return expr.filtered([](const auto& term) { return !is_weakly_separated(term); });
}

bool passes_normalize_remove_consecutive(const GammaExpr::ObjectT& term, int dimension, int num_points) {
  if (dimension > 0 && std::gcd(dimension, num_points) == 1) {
    // TODO: Move out to normalize_remove_consecutive OR otherwise optimize.
    absl::flat_hash_set<Gamma> discard;
    for (const int start : range(0, num_points)) {
      discard.insert(Gamma(mapped(range(start, start + dimension), [&](const int idx) {
        return idx % num_points + 1;
      })));
    }
    return absl::c_all_of(term, [&](const Gamma& g) {
      return !discard.contains(g);
    });
  } else {
    return absl::c_all_of(term, [](const Gamma& g) {
      // Optimization potential: check this on bitset level.
      const auto& v = g.index_vector();
      const auto it = absl::c_adjacent_find(v, [](int a, int b) {
        return b != a + 1;
      });
      return it != v.end();
    });
  }
}

GammaExpr normalize_remove_consecutive(const GammaExpr& expr, int dimension, int num_points) {
  return expr.filtered([&](const auto& term) {
    return passes_normalize_remove_consecutive(term, dimension, num_points);
  });
}

// TODO: Consider whether this overload should exist.
GammaExpr normalize_remove_consecutive(const GammaExpr& expr) {
  return normalize_remove_consecutive(expr, 0, 0);
}

GammaExpr delta_expr_to_gamma_expr(const DeltaExpr& expr) {
  return expr.mapped<GammaExpr>([](const std::vector<Delta>& term) {
    return mapped(term, [](const Delta& d) {
      return Gamma({d.a().as_simple_var(), d.b().as_simple_var()});
    });
  });
}

DeltaExpr gamma_expr_to_delta_expr(const GammaExpr& expr) {
  return expr.mapped<DeltaExpr>([&](const std::vector<Gamma>& term) {
    return mapped(term, [](const Gamma& g) {
      const auto vars = g.index_vector();
      CHECK_EQ(vars.size(), 2);
      return Delta(vars[0], vars[1]);
    });
  });
}

GammaExpr pullback(const GammaExpr& expr, const std::vector<int>& bonus_points) {
  if (bonus_points.empty()) {
    return expr;
  }
  const auto bonus_bitset = vector_to_bitset<Gamma::BitsetT>(bonus_points, Gamma::kBitsetOffset);
  return expr.mapped([&](const auto& term) {
    return mapped(term, [&](const Gamma& g) {
      CHECK((g.index_bitset() & bonus_bitset).none())
        << to_string(g) << " vs " << dump_to_string(bonus_points);
      return Gamma(g.index_bitset() | bonus_bitset);
    });
  }).annotations_map([&](const std::string& annotation) {
    // TODO: Find a proper pullback notation
    return fmt::function(
      fmt::opname("pb"),
      {annotation, str_join(bonus_points, ",")},
      HSpacing::sparse
    );
  });
}

GammaExpr pullback(const DeltaExpr& expr, const std::vector<int>& bonus_points) {
  return pullback(delta_expr_to_gamma_expr(expr), bonus_points);
}

GammaExpr plucker_dual(const GammaExpr& expr, const std::vector<int>& point_universe) {
  const auto universe_bitset = vector_to_bitset<Gamma::BitsetT>(point_universe, Gamma::kBitsetOffset);
  return expr.mapped([&](const auto& term) {
    return mapped(term, [&](const Gamma& g) {
      CHECK(bitset_contains(universe_bitset, g.index_bitset()));
      return Gamma(bitset_difference(universe_bitset, g.index_bitset()));
    });
  }).annotations_map([](const std::string& annotation) {
    return fmt::set_complement() + annotation;
  });
}

GammaExpr plucker_dual(const DeltaExpr& expr, const std::vector<int>& point_universe) {
  return plucker_dual(delta_expr_to_gamma_expr(expr), point_universe);
}

GammaACoExpr expand_into_glued_pairs(const GammaExpr& expr) {
  using CoExprT = GammaACoExpr;
  return expr.mapped_expanding([](const auto& term) {
    CoExprT expanded_expr;
    for (const int i : range(term.size() - 1)) {
      std::vector<GammaExpr> expanded_term;
      for (const int j : range(term.size() - 1)) {
        if (j < i) {
          expanded_term.push_back(GammaExpr::single({term[j]}));
        } else if (j == i) {
          expanded_term.push_back(GammaExpr::single({term[j], term[j+1]}));
        } else {
          expanded_term.push_back(GammaExpr::single({term[j+1]}));
        }
      }
      expanded_expr += abstract_coproduct_vec<CoExprT>(expanded_term);
    }
    return expanded_expr;
  });
}
