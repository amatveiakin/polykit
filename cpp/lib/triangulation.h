#pragma once

#include <array>
#include <vector>

#include "absl/types/span.h"

#include "range.h"
#include "util.h"


template<typename T>
std::vector<std::vector<std::pair<T, T>>> get_triangulations(absl::Span<const T> vertices) {
  const int n = vertices.size();
  if (n <= 3) {
    return {{}};
  }
  using Triangulation = std::vector<std::pair<T, T>>;
  std::vector<Triangulation> ret;
  // The foundation is {v[0], v[n-1]}. Now iterate the third vertex:
  for (int v : range(1, n-1)) {
    Triangulation tr_center;
    if (v != 1) {
      tr_center.push_back({vertices[0], vertices[v]});
    }
    if (v != n-2) {
      tr_center.push_back({vertices[v], vertices[n-1]});
    }
    const auto triangulations_left = get_triangulations(vertices.subspan(0, v+1));
    const auto triangulations_right = get_triangulations(vertices.subspan(v));
    for (const auto& tr_left : triangulations_left) {
      for (const auto& tr_right : triangulations_right) {
        ret.push_back(concat<Triangulation>(tr_left, tr_center, tr_right));
      }
    }
  }
  return ret;
}
template<typename T>
std::vector<std::vector<std::pair<T, T>>> get_triangulations(const std::vector<T>& vertices) {
  return get_triangulations(absl::MakeConstSpan(vertices));
}


using TriangulationQuadrangle = std::array<int, 4>;
std::vector<std::vector<TriangulationQuadrangle>> get_triangulation_quadrangle_indices(int num_vertices);

template<typename T>
std::vector<std::vector<std::array<T, 4>>> get_triangulation_quadrangles(const std::vector<T>& vertices) {
  return mapped(get_triangulation_quadrangle_indices(vertices.size()), [&](const auto& quadrangles) {
    return mapped(quadrangles, [&](const auto& quad) {
      return mapped_array(quad, [&](const int idx) {
        return vertices.at(idx);
      });
    });
  });
}
