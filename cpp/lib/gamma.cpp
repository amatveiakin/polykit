#include "gamma.h"

#include "set_util.h"


std::string to_string(const Gamma& g) {
  return fmt::parens(str_join(g.index_vector(), ","));
}

std::string to_string(const GammaUniformityMarker& marker) {
  return absl::StrCat("d=", marker.dimension);
}

Gamma monom_substitute_variables(Gamma g, const std::vector<int>& new_points) {
  // Optimization potential: do things on bitset level.
  return Gamma(mapped(g.index_vector(), [&](const int idx) {
    return new_points.at(idx - 1);
  }));
}

GammaExpr substitute_variables(const GammaExpr& expr, const std::vector<int>& new_points) {
  return expr.mapped_expanding([&](const GammaExpr::ObjectT& term_old) -> GammaExpr {
    std::vector<Gamma> term_new;
    for (const Gamma& g_old : term_old) {
      const Gamma g_new = monom_substitute_variables(g_old, new_points);
      if (g_new.is_nil()) {
        return {};
      }
      term_new.push_back(g_new);
    }
    return GammaExpr::single(term_new);
  }).without_annotations();
}

// TODO: Auto-generate substitute_variables functions for all co-exprs.
GammaNCoExpr substitute_variables(const GammaNCoExpr& expr, const std::vector<int>& new_points) {
  return expr.mapped_expanding([&](const GammaNCoExpr::ObjectT& term_old) -> GammaNCoExpr {
    std::vector<std::vector<Gamma>> term_new;
    for (const auto& copart_old : term_old) {
      std::vector<Gamma> copart_new;
      for (const Gamma& g_old : copart_old) {
        const Gamma g_new = monom_substitute_variables(g_old, new_points);
        if (g_new.is_nil()) {
          return {};
        }
        copart_new.push_back(g_new);
      }
      term_new.push_back(copart_new);
    }
    return GammaNCoExpr::single(term_new);
  }).without_annotations();
}

GammaExpr project_on(int axis, const GammaExpr& expr) {
  return expr.mapped_expanding([&](const GammaExpr::ObjectT& term_old) -> GammaExpr {
    std::vector<Gamma> term_new;
    for (const Gamma& g : term_old) {
      auto bitset = g.index_bitset();
      if (!bitset.test(axis - Gamma::kBitsetOffset)) {
        return {};
      }
      bitset.reset(axis - Gamma::kBitsetOffset);
      term_new.push_back(Gamma(bitset));
    }
    return GammaExpr::single(term_new);
  });
}

// Checks that points in `g1` and `g2` are weakly separated, i.e. that there are no such
// points {xa, ya} in (A = g1 \ g2) and points {xb, yb} in (B = g2 \ g1) that segment x1--y1
// intersects segment x2--y2 if all drawn as vertices of a polygon.
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

bool passes_normalize_remove_consecutive(const GammaExpr::ObjectT& term, int dimension, int num_points) {
  // TODO: Do we even need to pass the dimension, given that it can be deduced?
  if (dimension != 0) {
    CHECK_EQ(GammaExpr::Param::object_to_dimension(term), dimension);
  }
  if (dimension > 0 && std::gcd(dimension, num_points) == 1) {
    // Optimization potential: Move out to normalize_remove_consecutive OR otherwise optimize.
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
      // Optimization potential: check this on bitset level OR use discard set as above.
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

template<size_t Nesting, typename LinearT>
LinearT pullback_impl(const LinearT& expr, const std::vector<int>& bonus_points) {
  if (bonus_points.empty()) {
    return expr;
  }
  const auto bonus_bitset_or = vector_to_bitset_or<Gamma::BitsetT>(bonus_points, Gamma::kBitsetOffset);
  if (!bonus_bitset_or.has_value()) {
    return LinearT{};
  }
  const auto& bonus_bitset = bonus_bitset_or.value();
  return expr.mapped_expanding([&](const auto& term) {
    bool is_zero = false;
    const auto new_term = mapped_nested<Nesting>(term, [&](const Gamma& g) {
      if ((g.index_bitset() & bonus_bitset).any()) {
        is_zero = true;
      }
      return Gamma(g.index_bitset() | bonus_bitset);
    });
    return is_zero ? LinearT{} : LinearT::single(new_term);
  }).annotations_map([&](const std::string& annotation) {
    // TODO: Find a proper pullback notation
    return fmt::function(
      fmt::opname("pb"),
      {annotation, str_join(sorted(bonus_points), ",")},
      HSpacing::sparse
    );
  });
}

GammaExpr pullback(const DeltaExpr& expr, const std::vector<int>& bonus_points) {
  return pullback(delta_expr_to_gamma_expr(expr), bonus_points);
}

GammaExpr pullback(const GammaExpr& expr, const std::vector<int>& bonus_points) {
  return pullback_impl<1>(expr, bonus_points);
}

GammaNCoExpr pullback(const GammaNCoExpr& expr, const std::vector<int>& bonus_points) {
  return pullback_impl<2>(expr, bonus_points);
}

GammaExpr plucker_dual(const GammaExpr& expr, const std::vector<int>& point_universe) {
  const auto universe_bitset_or = vector_to_bitset_or<Gamma::BitsetT>(point_universe, Gamma::kBitsetOffset);
  if (!universe_bitset_or.has_value()) {
    return GammaExpr{};
  }
  const auto& universe_bitset = universe_bitset_or.value();
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

GammaExpr symmetrize_double(const GammaExpr& expr, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  const int sign = neg_one_pow(num_points);
  return expr + sign * substitute_variables(expr, rotated_vector(points, 1));
}

GammaExpr symmetrize_loop(const GammaExpr& expr, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  GammaExpr ret;
  for (const int i : range(num_points)) {
    const int sign = num_points % 2 == 1 ? 1 : neg_one_pow(i);
    ret += sign * substitute_variables(expr, rotated_vector(points, i));
  }
  return ret;
}

std::vector<int> common_vars(const GammaExpr::ObjectT& term) {
  auto indices = term[0].index_bitset();
  for (const Gamma& g : term) {
    indices &= g.index_bitset();
  }
  return bitset_to_vector(indices, Gamma::kBitsetOffset);
}

std::vector<int> all_vars(const GammaExpr::ObjectT& term) {
  auto indices = term[0].index_bitset();
  for (const Gamma& g : term) {
    indices |= g.index_bitset();
  }
  return bitset_to_vector(indices, Gamma::kBitsetOffset);
}
