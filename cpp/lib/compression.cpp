#include "compression.h"

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "check.h"
#include "string.h"
#include "util.h"


inline bool value_ok(int v) {
  return kCompressionMinValue <= v && v <= kCompressionMaxValue;
};
inline bool value_efficient(int v) {
  return kCompressionMinEfficientValue <= v && v <= kCompressionMaxEfficientValue;
};
inline bool value_compressible(int v) {
  return 0 <= v && v <= internal::kCompressionLowerValueMask;
};

inline unsigned char combine_pair(int a, int b) {
  CHECK(value_compressible(a));
  CHECK(value_compressible(b));
  return (a << internal::kCompressionShift) + b;
}
inline void split_pair(unsigned char compressed, int& a, int& b) {
  a = compressed >> internal::kCompressionShift;
  b = compressed & internal::kCompressionLowerValueMask;
}


void Compressor::push_value(int v) {
  CHECK(value_ok(v)) << v;
  if (value_efficient(v)) {
    uncompressed_.push_back(v);
  } else {
    int a, b;
    split_pair(v, a, b);
    uncompressed_.push_back(internal::kCompressionOverflowFlag);
    uncompressed_.push_back(a);
    uncompressed_.push_back(b);
  }
}

void Compressor::push_segment(absl::Span<const int> data) {
  push_value(data.size());
  for (const int v : data) {
    push_value(v);
  }
}

internal::CompressedBlobData Compressor::result_impl() {
  internal::CompressedBlobData ret;
  for (int i = 0; i < uncompressed_.size(); i += internal::kCompressionValuesPerByte) {
    static_assert(internal::kCompressionValuesPerByte == 2);
    const int a = uncompressed_[i];
    // Use kCompressionOverflowFlag as a filler. We know that it must always be followed
    // by two more value, hence if they are missing this is unambigously input end.
    const int b = (i+1 < uncompressed_.size())
      ? uncompressed_[i+1]
      : internal::kCompressionOverflowFlag;
    ret.push_back(combine_pair(a, b));
  }
  return ret;
}

Decompressor::Decompressor(const internal::CompressedBlobData& compressed) {
  for (unsigned char ch : compressed) {
    int a, b;
    split_pair(ch, a, b);
    decompressed_.push_back(a);
    decompressed_.push_back(b);
  }
  if (!decompressed_.empty() && decompressed_.back() == internal::kCompressionOverflowFlag) {
    decompressed_.pop_back();
  }
  remainder_ = absl::MakeConstSpan(decompressed_);
}

bool Decompressor::done() const {
  return remainder_.empty();
}

int Decompressor::pop_value() {
  CHECK(!done());
  if (remainder_[0] == internal::kCompressionOverflowFlag) {
    CHECK(remainder_.size() >= 3);
    int v = static_cast<char>(combine_pair(remainder_[1], remainder_[2]));
    remainder_ = remainder_.subspan(3);
    return v;
  } else {
    int v = remainder_[0];
    remainder_ = remainder_.subspan(1);
    return v;
  }
}

std::vector<int> Decompressor::pop_segment() {
  int size = pop_value();
  std::vector<int> ret;
  for (EACH : range(size)) {
    ret.push_back(pop_value());
  }
  return ret;
}
