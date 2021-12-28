// TODO(C++20): Check what can be replaced or implemented via std bit manipulation:
//   https://en.cppreference.com/w/cpp/numeric#Bit_manipulation_.28since_C.2B.2B20.29

#pragma once

#include <bitset>

#include "check.h"


inline unsigned int get_bit(unsigned int x, int n) {
  return (x >> n) & 1;
}

inline unsigned int set_bit(unsigned int x, int n, unsigned int value) {
  ASSERT(value == 0 || value == 1);
  return (x & ~(1 << n)) | (value << n);
}

// The smallest power of 2 greater than or equal to x
inline unsigned int round_up_to_power_of_two(unsigned int x) {
  CHECK_GT(x, 0);
  --x;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x+1;
}

// Requires: x == 2^k; returns k.
// From https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
inline int which_power_of_two(unsigned int x) {
  static const int MultiplyDeBruijnBitPosition2[32] = {
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
  };
  const int ret = MultiplyDeBruijnBitPosition2[(uint32_t)(x * 0x077CB531U) >> 27];
  CHECK_EQ(x, 1 << ret);
  return ret;
}
