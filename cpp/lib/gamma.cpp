#include "gamma.h"


template<size_t N>
std::bitset<N> bitset_difference(const std::bitset<N>& a, const std::bitset<N>& b) {
  return a & ~b;
}


std::string to_string(const Gamma& g) {
  return fmt::parens(str_join(g.index_vector(), ","));
}

// TODO: Describe how it works !!!
// TODO: Test !!!
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

GammaExpr keep_weakly_separated(const GammaExpr& expr) {
  return expr.filtered(&is_weakly_separated);
}
