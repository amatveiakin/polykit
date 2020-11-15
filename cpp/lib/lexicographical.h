#pragma once

#include <vector>

#include "circular_view.h"


// Optimization potential: Use an O(N) algorithm:
//   https://en.wikipedia.org/wiki/Lexicographically_minimal_string_rotation
// (Note: can use Lyndon factorization for this!)
template<typename T>
std::vector<T> lexicographically_minimal_rotation(std::vector<T> v) {
  // Note: could've used std::lexicographical_compare if iterators
  // provided by CircularView reported correct traits.
  static auto lexicographical_less = [](CircularView<T> a, CircularView<T> b) {
    CHECK_EQ(a.size(), b.size());
    for (int i = 0; i < a.size(); ++i) {
      if (a[i] != b[i]) {
        return a[i] < b[i];
      }
    }
    return false;
  };

  if (v.empty()) {
    return v;
  }

  CircularView<T> span{v};
  CircularView<T> min = span;
  for (int i = 1; i < v.size(); ++i) {
    const auto span_rotated = span.rotated(i);
    if (lexicographical_less(span_rotated, min)) {
      min = span_rotated;
    }
  }
  absl::c_rotate(v, v.begin() + min.start());
  return v;
}
