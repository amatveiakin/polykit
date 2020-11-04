#include "delta.h"

#include "absl/container/flat_hash_set.h"

#include "util.h"


DeltaAlphabetMapping delta_alphabet_mapping;


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<X>& new_points) {
  DeltaExpr ret;
  expr.foreach([&](const std::vector<Delta>& term_old, int coeff) {
    std::vector<Delta> term_new;
    for (const Delta& d_old : term_old) {
      Delta d_new(new_points.at(d_old.a() - 1), new_points.at(d_old.b() - 1));
      if (d_new.is_nil()) {
        return;
      }
      term_new.push_back(d_new);
    }
    ret.add_to(term_new, coeff);
  });
  return ret;
}


DeltaExpr sort_term_multiples(const DeltaExpr& expr) {
  return expr.mapped<DeltaExpr>([&](const std::vector<Delta>& term) {
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


DeltaExpr terms_containing_num_variables(const DeltaExpr& expr, int num_variables) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    std::vector<int> elements;
    for (const Delta& d : term) {
      elements.push_back(d.a());
      elements.push_back(d.b());
    }
    absl::c_sort(elements);
    const int unique_elements =
      std::unique(elements.begin(), elements.end()) - elements.begin();
    return unique_elements == num_variables;
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

DeltaExpr keep_connected_graphs(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return graph_is_connected(term);
  });
}
