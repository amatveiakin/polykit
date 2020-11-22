#pragma once

#include <array>
#include <optional>
#include <vector>

#include "absl/strings/str_cat.h"

#include "check.h"
#include "delta.h"
#include "compression.h"
#include "string.h"
#include "util.h"
#include "word.h"


constexpr int kCrossRatioElements = 4;

// TODO: Try to keep only CrossRatioNormalization::full
enum class CrossRatioNormalization {
  // Performs one kind of normalization: shift by two.
  // Reduces the number of possible permutation from 24 to 12.
  rotation_only,

  // Puts the smallest element in front.
  // Reduces the number of possible permutation from 24 to 6,
  // which is the theoretical minimum
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
    CHECK(is_valid()) << list_to_string(indices_);
    normalize();
  }
  explicit CrossRatioTmpl(absl::Span<const int> indices) {
    CHECK_EQ(indices.size(), kCrossRatioElements);
    absl::c_copy(indices, indices_.begin());
    CHECK(is_valid()) << list_to_string(indices_);
    normalize();
  }

  static CrossRatioTmpl one_minus(const CrossRatioTmpl& other) {
    return CrossRatioTmpl(permute(other.indices_, {0,2,1,3}));
  }
  static CrossRatioTmpl inverse(const CrossRatioTmpl& other) {
    return CrossRatioTmpl(permute(other.indices_, {0,3,2,1}));
  }

  bool is_valid() const {
    return absl::c_all_of(indices_, [](int idx) { return idx >= 1; });
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
  void normalize();

  std::array<int, kCrossRatioElements> indices_;
};

using CrossRatio = CrossRatioTmpl<CrossRatioNormalization::rotation_only>;
using CrossRatioN = CrossRatioTmpl<CrossRatioNormalization::full>;

inline CrossRatio CR(int a, int b, int c, int d) {
  return CrossRatio(std::array{a, b, c, d});
}

inline std::string to_string(const CrossRatio& ratio) {
  return fmt::brackets(str_join(ratio.indices(), ","));
}


// Generally speaking, it's possible to have a CompoundRatio of the form
// (a - d) / (c - d), but in practice we have at least a cross ratio.
constexpr int kMinCompoundRatioComponents = 2;

// TODO: Fix duplicate variables
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
    return CompoundRatio({std::vector<int>{ratio.indices().begin(), ratio.indices().end()}});
  }
  static CompoundRatio from_cross_ratio_product(const std::vector<CrossRatio>& ratios) {
    CompoundRatio ret;
    for (const auto& r : ratios) {
      ret.add(r);
    }
    return ret;
  }

  static CompoundRatio from_compressed(Decompressor& decompressor) {
    std::vector<std::vector<int>> loops;
    const std::vector<int> size_vec = decompressor.next_segment();
    CHECK_EQ(size_vec.size(), kSizeBump);
    const int size = size_vec.front() - 1;
    for (int i = 0; i < size; ++i) {
      loops.push_back(decompressor.next_segment());
    }
    return CompoundRatio(std::move(loops));
  }
  void compress(Compressor& compressor) const {
    // Compressor doesn't support zeroes
    compressor.add_segment({int(loops_.size()) + kSizeBump});
    for (const auto& l : loops_) {
      compressor.add_segment(l);
    }
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

  void normalize();

  std::optional<CompoundRatio> one_minus() const;

  void check() const {
    for (const auto& points : loops_) {
      CHECK(points.size() % 2 == 0);
      CHECK_GE(points.size(), kMinCompoundRatioComponents);
    }
  }

  bool operator==(const CompoundRatio& other) const { return loops_ == other.loops_; }
  bool operator< (const CompoundRatio& other) const { return loops_ <  other.loops_; }

  template <typename H>
  friend H AbslHashValue(H h, const CompoundRatio& ratio) {
    return H::combine(std::move(h), ratio.loops_);
  }

private:
  static constexpr int kSizeBump = 1;

  std::vector<std::vector<int>> loops_;
};

inline std::string to_string(const CompoundRatio& ratio) {
  return ratio.is_unity()
    ? fmt::unity()
    : str_join(
        ratio.loops(),
        "",
        [](const std::vector<int>& loop) {
          return fmt::brackets(str_join(loop, ","));
        }
      );
}


class LiraParam {
public:
  LiraParam() {}
  LiraParam(int foreweight, std::vector<int> weights, std::vector<CompoundRatio> ratios)
      : foreweight_(foreweight), weights_(std::move(weights)), ratios_(std::move(ratios)) {
    CHECK_EQ(weights_.size(), ratios_.size());
    CHECK_GT(weights_.size(), 0);
    CHECK_GE(foreweight_, 1);
    CHECK(absl::c_all_of(weights_, [](int w) { return w >= 1; }));
  }

  int foreweight() const { return foreweight_; }
  const std::vector<int>& weights() const { return weights_; }
  const std::vector<CompoundRatio>& ratios() const { return ratios_; }

  int depth() const { return ratios().size(); }
  int total_weight() const { return foreweight_ + absl::c_accumulate(weights_, 0) - 1; }
  int sign() const { return neg_one_pow(depth()); }

  auto as_tie() const { return std::tie(weights_, ratios_); }

  bool operator==(const LiraParam& other) const { return as_tie() == other.as_tie(); }
  bool operator< (const LiraParam& other) const { return as_tie() <  other.as_tie(); }

  template <typename H>
  friend H AbslHashValue(H h, const LiraParam& param) {
    return H::combine(std::move(h), param.foreweight_, param.weights_, param.ratios_);
  }

private:
  int foreweight_ = 0;
  std::vector<int> weights_;
  std::vector<CompoundRatio> ratios_;
};

Word lira_param_to_key(const LiraParam& params);
LiraParam key_to_lira_param(const Word& word);
std::string lira_param_function_name(const LiraParam& param);
std::string to_string(const LiraParam& params);
