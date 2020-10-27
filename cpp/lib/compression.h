#pragma once

#include <vector>

#include "absl/types/span.h"


constexpr int kCompressionSentinel = 0;


class Compressor {
public:
  // `data` must not contain kCompressionSentinel.
  void add_segment(absl::Span<const int> data);
  std::vector<unsigned char> result() &&;

private:
  std::vector<int> uncompressed_;
};


class Decompressor {
public:
  Decompressor(absl::Span<const unsigned char> compressed);

  bool done() const;
  std::vector<int> next_segment();

private:
  std::vector<int> decompressed_;
  absl::Span<const int> remainder_;
};
