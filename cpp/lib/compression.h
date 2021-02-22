// Simple packing algorithm for integer sequences where each value is in [0, 16).
// Stores two numbers per byte.

#pragma once

#include <array>
#include <vector>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "packed.h"


constexpr int kCompressionSentinel = 0;
constexpr int kCompressionValuesPerByte = 2;  // Note: some code implicitly assumes it's 2
constexpr int kCompressionShift = CHAR_BIT / kCompressionValuesPerByte;
constexpr int kCompressionMaxValue = (1 << kCompressionShift) - 1;
constexpr int kCompressionLowerValueMask = (1 << kCompressionShift) - 1;


namespace internal {
using CompressedBlobData = PVector<unsigned char, 10>;
}  // namespace internal

template<typename Tag>
class CompressedBlob {
public:
  CompressedBlob(internal::CompressedBlobData data) : data_(std::move(data)) {}
  const internal::CompressedBlobData& data() const { return data_; };

  bool operator==(const CompressedBlob& other) const { return data_ == other.data_; }
  bool operator!=(const CompressedBlob& other) const { return data_ != other.data_; }
  bool operator< (const CompressedBlob& other) const { return data_ <  other.data_; }
  bool operator<=(const CompressedBlob& other) const { return data_ <= other.data_; }
  bool operator> (const CompressedBlob& other) const { return data_ >  other.data_; }
  bool operator>=(const CompressedBlob& other) const { return data_ >= other.data_; }
  template <typename H>
  friend H AbslHashValue(H h, const CompressedBlob& blob) {
    return H::combine(std::move(h), blob.data_);
  }

private:
  internal::CompressedBlobData data_;
};

namespace internal {
template<typename T>
struct IsCompressedBlob { static constexpr bool value = false; };
template<typename T>
struct IsCompressedBlob<CompressedBlob<T>> { static constexpr bool value = true; };
}  // namespace internal


class Compressor {
public:
  // `data` must not contain kCompressionSentinel.
  void add_segment(absl::Span<const int> data);

  template<typename CompressedBlobT>
  CompressedBlobT result() && {
    static_assert(internal::IsCompressedBlob<CompressedBlobT>::value);
    return CompressedBlobT(result_impl());
  }

private:
  internal::CompressedBlobData result_impl();

  std::vector<int> uncompressed_;
};

class Decompressor {
public:
  template<typename Tag>
  Decompressor(const CompressedBlob<Tag>& compressed_blob) : Decompressor(compressed_blob.data()) {}

  bool done() const;
  std::vector<int> next_segment();

private:
  Decompressor(const internal::CompressedBlobData& compressed);

  std::vector<int> decompressed_;
  absl::Span<const int> remainder_;
};


inline bool compression_value_ok(int ch) {
  return 0 <= ch && ch <= kCompressionMaxValue && ch != kCompressionSentinel;
};

inline unsigned char compress_pair_no_check(int a, int b) {
  return (a << kCompressionShift) + b;
}

inline void decompress_pair(unsigned char compressed, int& a, int& b) {
  a = compressed >> kCompressionShift;
  b = compressed & kCompressionLowerValueMask;
}

template<int N>
std::array<unsigned char, N/2> compress_array(const std::array<int, N>& data) {
  static_assert(N % 2 == 0);
  CHECK(absl::c_all_of(data, compression_value_ok)) << dump_to_string(data);
  std::array<unsigned char, N/2> ret;
  int dst = 0;
  for (int i = 0; i < data.size(); i += kCompressionValuesPerByte) {
    ret[dst] = compress_pair_no_check(data[i], data[i+1]);
    ++dst;
  }
  return ret;
}

template<int N>
std::array<int, N*2> decompress_array(const std::array<unsigned char, N>& compressed) {
  std::array<int, N*2> ret;
  int dst = 0;
  for (unsigned char ch : compressed) {
    decompress_pair(ch, ret[dst], ret[dst+1]);
    dst += kCompressionValuesPerByte;
  }
  return ret;
}
