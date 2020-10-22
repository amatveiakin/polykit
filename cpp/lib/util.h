#pragma once

#include <cassert>

#include "absl/algorithm/container.h"


inline int pos_mod(int x, int y) {
  assert(y > 0);
  return (x % y + y) % y;
}

inline int neg_one_pow(int power) {
  assert(power >= 0);
  return 1 - power % 2 * 2;
}

inline int div_int(int a, int b) {
  assert(a % b == 0);
  return a / b;
}

inline int factorial(int n) {
  assert(n >= 0);
  int ret = 1;
  for (int i = 1; i <= n; ++i) {
    ret *= i;
  }
  return ret;
}


template<typename T>
std::vector<T> slice(const std::vector<T>& v, int from, int to = -1) {
  if (to == -1) {
    to = v.size();
  }
  assert(0 <= from && from <= v.size());
  assert(0 <= to && to <= v.size());
  return std::vector<T>(v.begin() + from, v.begin() + to);
}

template<typename T>
std::vector<T> concat(std::vector<T> a, std::vector<T> b) {
  a.insert(a.end(), std::move_iterator(b.begin()), std::move_iterator(b.end()));
  return a;
}

template<typename T>
void rotate_vector(std::vector<T>& v, int n) {
  n = pos_mod(n, v.size());
  absl::c_rotate(v, v.begin() + n);
}

template<typename T>
void append_vector(std::vector<T>& dst, const std::vector<T>& src) {
  const size_t old_size = dst.size();
  dst.resize(old_size + src.size());
  absl::c_move(src, dst.begin() + old_size);
}
