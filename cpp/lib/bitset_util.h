#pragma once

#include <bitset>
#include <vector>

#include "check.h"


template<size_t N>
std::bitset<N> bitset_difference(const std::bitset<N>& a, const std::bitset<N>& b) {
  return a & ~b;
}

template<size_t DstN, size_t SrcN>
std::bitset<DstN> convert_bitset(const std::bitset<SrcN>& src) {
  if constexpr (SrcN <= sizeof(unsigned long) * CHAR_BIT) {
    return std::bitset<DstN>(src.to_ulong());
  } else {
    static_assert(SrcN <= sizeof(unsigned long long) * CHAR_BIT, "Bitset too large");
    return std::bitset<DstN>(src.to_ullong());
  }
}

template<size_t N>
std::bitset<N> vector_to_bitset(const std::vector<int>& vector, int offset = 0) {
  std::bitset<N> bitset;
  for (int idx : vector) {
    idx -= offset;
    CHECK_LE(0, idx);
    CHECK_LT(idx, N);
    bitset[idx] = true;
  }
  return bitset;
}

template<size_t N>
std::vector<int> bitset_to_vector(std::bitset<N> bitset, int offset = 0) {
  int current_index = offset;
  std::vector<int> vector;
  while (bitset.any()) {
    if (bitset[0]) {
      vector.push_back(current_index);
    }
    bitset >>= 1;
    ++current_index;
  }
  return vector;
}
