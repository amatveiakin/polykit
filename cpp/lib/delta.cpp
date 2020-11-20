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
  return num_distinct_elements(elements);
}


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<X>& new_points) {
  constexpr int kMaxChar = std::numeric_limits<unsigned char>::max();
  constexpr int kNoReplacement = kMaxChar;
  constexpr int kNil = kMaxChar - 1;
  std::array<unsigned char, kMaxChar> replacements;
  replacements.fill(kNoReplacement);
  const int num_src_vars = new_points.size();
  for (int a = 1; a <= num_src_vars; ++a) {
    for (int b = a+1; b <= num_src_vars; ++b) {
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
  expr.foreach_key([&](const Word& term_old, int coeff) {
    Word term_new;
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


DeltaExpr terms_with_num_distinct_variables(const DeltaExpr& expr, int num_variables) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return num_distinct_variables(term) == num_variables;
  });
}

DeltaExpr terms_containing_only_variables(const DeltaExpr& expr, const std::vector<int>& indices) {
  absl::flat_hash_set<int> indices_set(indices.begin(), indices.end());
  return expr.filtered([&](const std::vector<Delta>& term) {
    return absl::c_all_of(term, [&](const Delta& d) {
      return indices_set.count(d.a()) > 0 && indices_set.count(d.b()) > 0;
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
  if (expr.zero()) {
    os << expr;
    return;
  }
  std::map<int, DeltaExpr> num_vars_to_expr;
  expr.foreach([&](const auto& term, int coeff) {
    num_vars_to_expr[num_distinct_variables(term)].add_to(term, coeff);
  });
  bool first = true;
  for (const auto& [num_vars, expr] : num_vars_to_expr) {
    if (!first) {
      os << "---\n";
    }
    first = false;
    os << num_vars << " vars " << expr;
  }
  // TODO: Factor out function for printing annotation that would encapsulate:
  // separator; checking `expression_include_annotations`; removing (or setting
  // custom) line limit, etc.
  if (*current_formatting_config().expression_include_annotations) {
    os << "~~~\n" << expr.annotations();
  }
}
