#include "triangulation.h"

#include "absl/container/flat_hash_map.h"


void add_quadrangles(
    const absl::flat_hash_map<int, std::vector<int>>& edges,
    std::vector<int>& current,
    std::vector<TriangulationQuadrangle>& ret
) {
  CHECK_LE(current.size(), 4);
  if (current.size() == 4) {
    // Check if closing edge exists. Notice the vertex order: small vertex goes first.
    if (contains_naive(value_or(edges, current.front()), current.back())) {
      ret.push_back(to_array<4>(current));
    }
    return;
  }
  for (const auto& e : value_or(edges, current.back())) {
    current.push_back(e);
    add_quadrangles(edges, current, ret);
    current.pop_back();
  }
}

std::vector<std::vector<TriangulationQuadrangle>> get_triangulation_quadrangle_indices(int num_vertices) {
  const std::vector<int> vertices = to_vector(range(num_vertices));
  const auto triangulations = get_triangulations(absl::MakeConstSpan(vertices));
  return mapped(triangulations, [&](const auto& triangulation) {
    // TODO: Use a vector instead of absl::flat_hash_map.
    absl::flat_hash_map<int, std::vector<int>> edges;
    const auto add_edge = [&](int v1, int v2) {
      CHECK_NE(v1, v2);
      // Add only edges only from a vertex with a lower index to a vertex with a higher index.
      // Rationale: for each quadrangle there is exactly one way to traverse it so that vertex
      // numbers are increasing. We are going to look for such ways only.
      sort_two(v1, v2);
      edges[v1].push_back(v2);
    };
    for (const int i : range(num_vertices)) {
      add_edge(vertices[i], vertices[(i+1) % num_vertices]);
    }
    for (const auto [v1, v2] : triangulation) {
      add_edge(v1, v2);
    }
    std::vector<TriangulationQuadrangle> ret;
    for (const int v : range(num_vertices)) {
      std::vector<int> current{v};
      add_quadrangles(edges, current, ret);
    }
    return ret;
  });
}
