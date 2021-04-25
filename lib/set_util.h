#pragma once

#include <vector>

#include "absl/algorithm/container.h"


// Optimization potential: sort only if necessary to avoid copying.

template<typename T>
std::vector<T> set_intersection(const std::vector<T>& a, const std::vector<T>& b) {
  std::vector<T> ret;
  absl::c_set_intersection(sorted(a), sorted(b), std::back_inserter(ret));
  return ret;
}

template<typename T>
int set_intersection_size(const std::vector<T>& a, const std::vector<T>& b) {
  // Optimization potential: Use a fake iterators that counts instead of back_inserter.
  return set_intersection(a, b).size();
}

template<typename T>
std::vector<T> set_intersection(const std::vector<std::vector<T>>& sets) {
  CHECK(!sets.empty());
  std::vector<int> ret = sets.front();
  absl::c_sort(ret);
  for (auto one_set : absl::MakeConstSpan(sets).subspan(1)) {
    absl::c_sort(one_set);
    std::vector<int> new_ret;
    absl::c_set_intersection(ret, one_set, std::back_inserter(new_ret));
    ret = std::move(new_ret);
  }
  return ret;
}

template<typename T>
int set_intersection_size(const std::vector<std::vector<T>>& sets) {
  return set_intersection(sets).size();
}

template<typename T>
std::vector<T> set_union(const std::vector<T>& a, const std::vector<T>& b) {
  std::vector<T> ret;
  absl::c_set_union(sorted(a), sorted(b), std::back_inserter(ret));
  return ret;
}

template<typename T>
std::vector<T> set_union(const std::vector<std::vector<T>>& sets) {
  return keep_unique(sorted(flatten(sets)));
}

template<typename T>
std::vector<T> set_difference(const std::vector<T>& a, const std::vector<T>& b) {
  std::vector<T> ret;
  absl::c_set_difference(sorted(a), sorted(b), std::back_inserter(ret));
  return ret;
}
