// DEPRECATED[word-to-pvector]

#pragma once

#include <array>


template <class T>
inline void hash_combine(size_t& hash, const T& new_hash) noexcept {
  hash ^= new_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
}

template<size_t size>
size_t hash_array(const std::array<unsigned char, size>& arr) noexcept {
  static_assert(size % sizeof(size_t) == 0);
  constexpr int kWordStorageSizeInHashT = size / sizeof(size_t);
  std::array<size_t, kWordStorageSizeInHashT> as_hash_t;
  std::memcpy(as_hash_t.data(), arr.data(), size);
  size_t ret = 0;
  for (const size_t v : as_hash_t) {
    hash_combine(ret, std::hash<size_t>()(v));
  }
  return ret;
}
