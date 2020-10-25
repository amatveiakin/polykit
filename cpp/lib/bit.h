#pragma once

#include <cassert>


inline unsigned int get_bit(unsigned int x, int n) {
  return (x >> n) & 1;
}

inline unsigned int set_bit(unsigned int x, int n, unsigned int value) {
  assert(value == 0 || value == 1);
  return (x & ~(1 << n)) | (value << n);
}
