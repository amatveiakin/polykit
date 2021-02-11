#pragma once

#include <array>
#include <optional>
#include <vector>

#include "check.h"
#include "compression.h"
#include "string.h"
#include "util.h"


constexpr int kCrossRatioElements = 4;

// TODO: Try to keep only CrossRatioNormalization::full
enum class CrossRatioNormalization {
  // Performs one kind of normalization: shift by two.
  // Reduces the number of possible permutation from 24 to 12.
  rotation_only,

  // Puts the smallest element in front.
  // Reduces the number of possible permutation from 24 to 6.
  full,
};

// Represents a cross ratio [a,b,c,d] = ([a,b] * [c,d]) / ([b,c] * [d,a])
//
// Note: if this is going to be compressed (e.g. put into LiraParam),
//   indices should not exceed kCompressionMaxValue.
//
template<CrossRatioNormalization Normalization>
class CrossRatioTmpl {
public:
  CrossRatioTmpl() {
    indices_.fill(0);
  }
  explicit CrossRatioTmpl(std::array<int, kCrossRatioElements> indices)
      : indices_(std::move(indices)) {
    check();
    normalize();
  }
  explicit CrossRatioTmpl(absl::Span<const int> indices) {
    CHECK_EQ(indices.size(), kCrossRatioElements);
    absl::c_copy(indices, indices_.begin());
    check();
    normalize();
  }

  static CrossRatioTmpl one_minus(const CrossRatioTmpl& ratio) {
    return CrossRatioTmpl(permute(ratio.indices_, {0,2,1,3}));
  }
  static CrossRatioTmpl inverse(const CrossRatioTmpl& ratio) {
    return CrossRatioTmpl(permute(ratio.indices_, {0,3,2,1}));
  }

  int operator[](int idx) const { return indices_[idx]; }
  int at        (int idx) const { return indices_.at(idx); }

  const std::array<int, kCrossRatioElements>& indices() const { return indices_; }

  bool operator==(const CrossRatioTmpl& other) const { return indices_ == other.indices_; }
  bool operator!=(const CrossRatioTmpl& other) const { return indices_ != other.indices_; }
  bool operator< (const CrossRatioTmpl& other) const { return indices_ <  other.indices_; }
  bool operator<=(const CrossRatioTmpl& other) const { return indices_ <= other.indices_; }
  bool operator> (const CrossRatioTmpl& other) const { return indices_ >  other.indices_; }
  bool operator>=(const CrossRatioTmpl& other) const { return indices_ >= other.indices_; }

  template <typename H>
  friend H AbslHashValue(H h, const CrossRatioTmpl& ratio) {
    return H::combine(std::move(h), ratio.indices_);
  }

private:
  void check() const {
    CHECK(absl::c_all_of(indices_, [](int idx) { return idx >= 1; })) << dump_to_string(indices_);
  }
  void normalize();

  std::array<int, kCrossRatioElements> indices_;
};

using CrossRatio = CrossRatioTmpl<CrossRatioNormalization::rotation_only>;
using CrossRatioN = CrossRatioTmpl<CrossRatioNormalization::full>;

inline CrossRatio CR(int a, int b, int c, int d) {
  return CrossRatio(std::array{a, b, c, d});
}

template<CrossRatioNormalization Normalization>
std::string to_string(const CrossRatioTmpl<Normalization>& ratio) {
  return fmt::brackets(str_join(ratio.indices(), ","));
}

std::string dependent_cross_ratio_formula(
    CrossRatioN initial, const std::string& notation, CrossRatioN value);


template<CrossRatioNormalization Normalization>
class CrossRatioOrUnityTmpl {
public:
  using CrossRatioT = CrossRatioTmpl<Normalization>;

  CrossRatioOrUnityTmpl() {}
  CrossRatioOrUnityTmpl(CrossRatioT r) : data_(std::move(r)) {}

  static CrossRatioOrUnityTmpl unity() { return CrossRatioOrUnityTmpl(); }

  bool is_unity() const { return !data_.has_value(); }
  const CrossRatioT& as_ratio() const { return data_.value(); }

  bool operator==(const CrossRatioOrUnityTmpl& other) const { return data_ == other.data_; }
  bool operator!=(const CrossRatioOrUnityTmpl& other) const { return data_ != other.data_; }
  bool operator< (const CrossRatioOrUnityTmpl& other) const { return data_ <  other.data_; }
  bool operator<=(const CrossRatioOrUnityTmpl& other) const { return data_ <= other.data_; }
  bool operator> (const CrossRatioOrUnityTmpl& other) const { return data_ >  other.data_; }
  bool operator>=(const CrossRatioOrUnityTmpl& other) const { return data_ >= other.data_; }

  template <typename H>
  friend H AbslHashValue(H h, const CrossRatioOrUnityTmpl& r) {
    return H::combine(std::move(h), r.data_);
  }

private:
  std::optional<CrossRatioT> data_;
};

template<CrossRatioNormalization Normalization>
std::string to_string(const CrossRatioOrUnityTmpl<Normalization>& r) {
  return r.is_unity() ? fmt::unity() : to_string(r.as_ratio());
}

using CrossRatioOrUnity = CrossRatioOrUnityTmpl<CrossRatioNormalization::rotation_only>;
using CrossRatioNOrUnity = CrossRatioOrUnityTmpl<CrossRatioNormalization::full>;


// Generally speaking, it's possible to have a CompoundRatio of the form
// (a - d) / (c - d), but in practice we have at least a cross ratio.
constexpr int kMinCompoundRatioComponents = 2;

class CompoundRatio {
public:
  CompoundRatio() {}
  CompoundRatio(std::vector<std::vector<int>> loops)
    : loops_(std::move(loops)) {
    normalize();
  }

  static CompoundRatio unity() {
    return CompoundRatio();
  }

  static CompoundRatio from_cross_ratio(const CrossRatio& ratio) {
    return CompoundRatio({to_vector(ratio.indices())});
  }
  static CompoundRatio from_cross_ratio(const CrossRatioOrUnity& r) {
    return r.is_unity()
      ? CompoundRatio::unity()
      : CompoundRatio::from_cross_ratio(r.as_ratio());
  }
  static CompoundRatio from_cross_ratio_product(const std::vector<CrossRatio>& ratios) {
    CompoundRatio ret;
    for (const auto& r : ratios) {
      ret.add(r);
    }
    return ret;
  }

  bool is_unity() const { return loops_.empty(); };

  void add(const CompoundRatio& ratio) {
    append_vector(loops_, ratio.loops_);
    normalize();
  }
  void add(const CrossRatio& ratio) {
    add(CompoundRatio::from_cross_ratio(ratio));
  }

  const std::vector<std::vector<int>>& loops() const { return loops_; };

  static std::optional<CompoundRatio> one_minus(const CompoundRatio& ratio);

  bool operator==(const CompoundRatio& other) const { return loops_ == other.loops_; }
  bool operator< (const CompoundRatio& other) const { return loops_ <  other.loops_; }

  template <typename H>
  friend H AbslHashValue(H h, const CompoundRatio& ratio) {
    return H::combine(std::move(h), ratio.loops_);
  }

private:
  void check() const;
  void normalize();

  std::vector<std::vector<int>> loops_;
};

using CompoundRatioCompressed = CompressedBlob<CompoundRatio>;

void compress_compound_ratio(const CompoundRatio& ratio, Compressor& compressor);
CompoundRatioCompressed compress_compound_ratio(const CompoundRatio& ratio);
CompoundRatio uncompress_compound_ratio(Decompressor& decompressor);
CompoundRatio uncompress_compound_ratio(const CompoundRatioCompressed& data);

std::string to_string(const CompoundRatio& ratio);
