#pragma once

#include "absl/algorithm/container.h"

#include "range.h"
#include "util.h"


// Functionally equivalent to
//   sorted(container, cmp::projected(projector));
// but computes `projector` once per element.
template<typename C, typename F>
C sorted_by_projection(const C& container, const F& projector) {
  auto projected = mapped_with_index(container, [&](int idx, const auto& element) {
    return std::pair{projector(element), idx};
  });
  absl::c_sort(projected);
  return mapped(projected, [&](const auto& p) {
    return container[p.second];
  });
}


// Optimization potential: O(N*log(N)) sort for large N.
template<typename Container, typename Compare>
[[nodiscard]] int sort_with_sign(Container& v, const Compare& comp) {
  int sign = 1;
  for (EACH : range(v.size())) {
    for (int i : range(v.size() - 1)) {
      if (comp(v[i+1], v[i])) {
        std::swap(v[i], v[i+1]);
        sign *= -1;
      }
    }
  }
  return sign;
}

template<typename Container>
[[nodiscard]] int sort_with_sign(Container& v) {
  return sort_with_sign(v, std::less<>());
}

template<typename Container>
int permutation_sign(Container c) {
  return sort_with_sign(c);
}
