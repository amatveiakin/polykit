// Simple packing algorithm for integers within signed char range, [-128, 127].
// Efficiently packs small non-negative integers (compresses two ints into one byte).
// The exact bound of "small" is a subject to change (15 at the moment).

#pragma once

#include <array>
#include <vector>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "packed.h"


namespace internal {
static_assert(CHAR_BIT % 2 == 0);
constexpr int kCompressionValuesPerByte = 2;  // Note: some code implicitly assumes it's 2
constexpr int kCompressionShift = CHAR_BIT / kCompressionValuesPerByte;
constexpr int kCompressionLowerValueMask = (1 << kCompressionShift) - 1;
constexpr int kCompressionOverflowFlag = kCompressionLowerValueMask - 1;
}  // namespace internal

constexpr int kCompressionMinValue = std::numeric_limits<char>::min();
constexpr int kCompressionMaxValue = std::numeric_limits<char>::max();
constexpr int kCompressionMinEfficientValue = 0;
constexpr int kCompressionMaxEfficientValue = internal::kCompressionOverflowFlag - 1;


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
struct IsCompressedBlob : std::false_type {};
template<typename T>
struct IsCompressedBlob<CompressedBlob<T>> : std::true_type {};
}  // namespace internal


class Compressor {
public:
  void push_value(int value);
  void push_segment(absl::Span<const int> data);

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

  // Be careful with `done`! A class that relies on it cannot be nested within larger
  // class compression blob.
  // TODO: In order to improve safety, add `push_blob`/`pop_blob` instead of passing a
  // Compressor/Decompressor to member classes.
  bool done() const;

  int pop_value();
  std::vector<int> pop_segment();

private:
  Decompressor(const internal::CompressedBlobData& compressed);

  std::vector<int> decompressed_;
  absl::Span<const int> remainder_;
};
