#include "delta.h"

#include "absl/container/flat_hash_set.h"

#include "util.h"


DeltaAlphabetMapping delta_alphabet_mapping;


static int num_distinct_variables(const std::vector<Delta>& term) {
  std::vector<int> elements;
  for (const Delta& d : term) {
    elements.push_back(d.a());
    elements.push_back(d.b());
  }
  return num_distinct_elements_unsorted(elements);
}


DeltaExpr substitute_variables(const DeltaExpr& expr, SpanX new_points_span) {
  constexpr int kMaxChar = std::numeric_limits<unsigned char>::max();
  constexpr int kNoReplacement = kMaxChar;
  constexpr int kNil = kMaxChar - 1;
  const auto new_points = new_points_span.as_x();
  std::array<unsigned char, kMaxChar> replacements;
  replacements.fill(kNoReplacement);
  const int num_src_vars = new_points.size();
  for (int a : range_incl(1, num_src_vars)) {
    for (int b : range_incl(a+1, num_src_vars)) {
      const Delta before = Delta(a, b);
      const Delta after = Delta(new_points[a-1], new_points[b-1]);
      const unsigned char key_before = delta_alphabet_mapping.to_alphabet(before);
      if (after.is_nil()) {
        replacements.at(key_before) = kNil;
      } else {
        const unsigned char key_after = delta_alphabet_mapping.to_alphabet(after);
        CHECK(key_after != kNoReplacement);
        CHECK(key_after != kNil);
        replacements.at(key_before) = key_after;
      }
    }
  }
  DeltaExpr ret;
  expr.foreach_key([&](const DeltaExpr::StorageT& term_old, int coeff) {
    DeltaExpr::StorageT term_new;
    for (unsigned char ch : term_old) {
      unsigned char ch_new = replacements.at(ch);
      CHECK(ch_new != kNoReplacement);
      if (ch_new == kNil) {
        return;
      }
      term_new.push_back(ch_new);
    }
    ret.add_to_key(term_new, coeff);
  });
  return ret;
}

DeltaExpr involute(const DeltaExpr& expr, const std::vector<int>& points) {
  return expr.mapped_expanding([&](const std::vector<Delta>& term) {
    return tensor_product(absl::MakeConstSpan(
      mapped(term, [&](const Delta& d) -> DeltaExpr {
        const auto [p1, p2, p3, p4, p5, p6] = to_array<6>(points);
        if (d == Delta(p6,p5)) {
          return D(p6,p1) - D(p1,p2) + D(p2,p3) - D(p3,p4) + D(p4,p5);
        } else if (d == Delta(p6,p4)) {
          return D(p4,p2) + D(p3,p1) - D(p1,p5) + D(p6,p1) - D(p1,p2) - D(p3,p4) + D(p4,p5);
        } else if (d == Delta(p6,p2)) {
          return D(p6,p1) - D(p1,p5) + D(p5,p3) - D(p3,p4) + D(p4,p2);
        } else {
          return DeltaExpr::single({d});
        }
      })
    ));
  });
}

DeltaExpr sort_term_multiples(const DeltaExpr& expr) {
  return expr.mapped([&](const std::vector<Delta>& term) {
    return sorted(term);
  });
}

DeltaExpr terms_with_unique_muptiples(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    const auto term_sorted = sorted(term);
    return absl::c_adjacent_find(term_sorted) == term_sorted.end();
  });
}

DeltaExpr terms_with_nonunique_muptiples(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    const auto term_sorted = sorted(term);
    return absl::c_adjacent_find(term_sorted) != term_sorted.end();
  });
}


DeltaExpr terms_with_num_distinct_variables(const DeltaExpr& expr, int num_distinct) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return num_distinct_variables(term) == num_distinct;
  });
}

DeltaExpr terms_with_min_distinct_variables(const DeltaExpr& expr, int min_distinct) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return num_distinct_variables(term) >= min_distinct;
  });
}

DeltaExpr terms_containing_only_variables(const DeltaExpr& expr, const std::vector<int>& indices) {
  absl::flat_hash_set<int> indices_set(indices.begin(), indices.end());
  return expr.filtered([&](const std::vector<Delta>& term) {
    return absl::c_all_of(term, [&](const Delta& d) {
      return indices_set.contains(d.a()) && indices_set.contains(d.b());
    });
  });
}

DeltaExpr terms_without_variables(const DeltaExpr& expr, const std::vector<int>& indices) {
  absl::flat_hash_set<int> indices_set(indices.begin(), indices.end());
  return expr.filtered([&](const std::vector<Delta>& term) {
    return !absl::c_any_of(term, [&](const Delta& d) {
      return indices_set.contains(d.a()) && indices_set.contains(d.b());
    });
  });
}


static void graph_mark_reached(
    int start,
    const std::vector<std::vector<int>>& nbrs,
    std::vector<bool>& reached) {
  if (reached.at(start)) {
    return;
  }
  reached.at(start) = true;
  for (int v : nbrs.at(start)) {
    graph_mark_reached(v, nbrs, reached);
  }
}

static bool graph_is_connected(const std::vector<Delta>& graph) {
  if (graph.empty()) {
    return true;
  }
  int max_vertex = -1;
  for (const Delta& d : graph) {
    max_vertex = std::max({max_vertex, d.a(), d.b()});
  }
  std::vector<std::vector<int>> nbrs(max_vertex+1, std::vector<int>{});
  for (const Delta& d : graph) {
    nbrs[d.a()].push_back(d.b());
    nbrs[d.b()].push_back(d.a());
  }
  std::vector<bool> reached(max_vertex+1, false);
  graph_mark_reached(graph.front().a(), nbrs, reached);
  for (const Delta& d : graph) {
    if (!reached[d.a()] || !reached[d.b()]) {
      return false;
    }
  }
  return true;
}

DeltaExpr terms_with_connected_variable_graph(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return graph_is_connected(term);
  });
}

void print_sorted_by_num_distinct_variables(std::ostream& os, const DeltaExpr& expr) {
  to_ostream_grouped(
    os, expr, std::less<>{},
    num_distinct_variables, std::less<>{},
    [](int num_vars) {
      return absl::StrCat(num_vars, " vars");
    },
    LinearNoContext{}
  );
}
