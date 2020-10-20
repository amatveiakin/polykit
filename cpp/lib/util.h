#pragma once

#include <cassert>


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
inline void append_vector(std::vector<T>& dst, const std::vector<T>& src) {
  const size_t old_size = dst.size();
  dst.resize(old_size + src.size());
  std::move(src.begin(), src.end(), dst.begin() + old_size);
}
