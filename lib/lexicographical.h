#pragma once

#include <vector>

#include "absl/algorithm/container.h"

#include "range.h"


// Optimization potential: Use an O(N) algorithm:
//   https://en.wikipedia.org/wiki/Lexicographically_minimal_string_rotation
// (Note: can use Lyndon factorization for this!)
template<typename T>
std::vector<T> lexicographically_minimal_rotation(std::vector<T> v) {
  std::vector<T> min = v;
  for (EACH : range(1, v.size())) {
    absl::c_rotate(v, v.begin() + 1);
    if (v < min) {
      min = v;
    }
  }
  return min;
}
