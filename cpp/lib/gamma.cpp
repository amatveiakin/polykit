#include "gamma.h"


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
