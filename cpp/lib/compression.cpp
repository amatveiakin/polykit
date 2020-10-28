// Optimization potential: use varint encoding instead.

#include "compression.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "check.h"
#include "string.h"
#include "util.h"


void Compressor::add_segment(absl::Span<const int> data) {
  CHECK(!data.empty());
  CHECK(absl::c_all_of(data, compression_value_ok)) << list_to_string(data);
  if (!uncompressed_.empty()) {
    uncompressed_.push_back(kCompressionSentinel);
  }
  append_vector(uncompressed_, data);
}

std::vector<unsigned char> Compressor::result() && {
  std::vector<unsigned char> ret;
  for (int i = 0; i < uncompressed_.size(); i += kCompressionValuesPerByte) {
    const int a = uncompressed_[i];
    const int b = (i+1 < uncompressed_.size()) ? uncompressed_[i+1] : kCompressionSentinel;
    ret.push_back(compress_pair_no_check(a, b));
  }
  return ret;
}

Decompressor::Decompressor(absl::Span<const unsigned char> compressed) {
  CHECK(!compressed.empty());
  for (unsigned char ch : compressed) {
    int a, b;
    decompress_pair(ch, a, b);
    decompressed_.push_back(a);
    decompressed_.push_back(b);
  }
  while (!decompressed_.empty() && decompressed_.back() == kCompressionSentinel) {
    decompressed_.pop_back();
  }
  CHECK(!decompressed_.empty());
  remainder_ = absl::MakeConstSpan(decompressed_);
}

bool Decompressor::done() const {
  return remainder_.empty();
}

std::vector<int> Decompressor::next_segment() {
  CHECK(!done());
  const auto it = absl::c_find(remainder_, kCompressionSentinel);
  auto ret = std::vector(remainder_.begin(), it);
  remainder_ = remainder_.subspan(
      (it == remainder_.end() ? it : it + 1) -
      remainder_.begin());
  return ret;
}
