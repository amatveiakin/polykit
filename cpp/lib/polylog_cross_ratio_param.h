#pragma once

#include <array>
#include <vector>

#include "absl/strings/str_cat.h"

#include "check.h"
#include "compression.h"
#include "string.h"
#include "util.h"
#include "word.h"


constexpr int kCrossRatioElements = 4;
constexpr int kCrossRatioElementsCompressed = 2;
constexpr int kCrossRatioMaxIndex = kCompressionMaxValue;

using CrossRatioCompressed = std::array<unsigned char, kCrossRatioElementsCompressed>;

// Represents a cross ratio [a,b,c,d]
class CrossRatio {
public:
  CrossRatio() {}
  CrossRatio(std::array<int, kCrossRatioElements> indices) : indices_(std::move(indices)) {
    CHECK(is_valid()) << list_to_string(indices_);
    normalize();
  }
  CrossRatio(absl::Span<const int> indices) {
    CHECK_EQ(indices.size(), kCrossRatioElements);
    absl::c_copy(indices, indices_.begin());
    CHECK(is_valid()) << list_to_string(indices_);
    normalize();
  }

  static CrossRatio from_compressed(const CrossRatioCompressed& compressed) {
    return CrossRatio(decompress_array(compressed));
  }
  static CrossRatio from_compressed(absl::Span<const unsigned char> compressed) {
    CHECK_EQ(compressed.size(), kCrossRatioElementsCompressed);
    CrossRatioCompressed as_array;
    absl::c_copy(compressed, as_array.begin());
    return from_compressed(as_array);
  }

  static CrossRatio one_minus(const CrossRatio& other) {
    const auto& v = other.indices_;
    return CrossRatio(std::array{v[0], v[2], v[1], v[3]});
  }

  bool is_valid() const {
    return absl::c_all_of(indices_, [](int idx) {
      return 1 <= idx && idx <= kCrossRatioMaxIndex;
    });
  }

  const std::array<int, kCrossRatioElements>& indices() const { return indices_; }
  CrossRatioCompressed compressed() const { return compress_array(indices_); }

  bool operator==(const CrossRatio& other) const { return indices_ == other.indices_; }
  bool operator<(const CrossRatio& other)  const { return indices_ <  other.indices_; }

private:
  void normalize() {
    // TODO: !!!
  }

  std::array<int, kCrossRatioElements> indices_;
};

inline CrossRatio CR(int a, int b, int c, int d) {
  return CrossRatio(std::array{a, b, c, d});
}

inline std::string to_string(const CrossRatio& ratio) {
  return absl::StrCat("[", str_join(ratio.indices(), ","), "]");
}


class LiraParam {
public:
  LiraParam() {}
  LiraParam(std::vector<int> weights, std::vector<std::vector<CrossRatio>> ratios)
      : weights_(std::move(weights)), ratios_(std::move(ratios)) {
    CHECK_EQ(weights_.size(), ratios_.size());
    for (auto& r : ratios_) {
      absl::c_sort(r);
    }
  }

  const std::vector<int>& weights() const { return weights_; }
  const std::vector<std::vector<CrossRatio>>& ratios() const { return ratios_; }

  int sign() const { return neg_one_pow(ratios().size()); }
  int total_weight() const { return absl::c_accumulate(weights_, 0); }

  auto as_tie() const { return std::tie(weights_, ratios_); }

  bool operator==(const LiraParam& other) const { return as_tie() == other.as_tie(); }
  bool operator< (const LiraParam& other) const { return as_tie() <  other.as_tie(); }

private:
  std::vector<int> weights_;
  std::vector<std::vector<CrossRatio>> ratios_;
};

Word lira_param_to_key(const LiraParam& params);
LiraParam key_to_lira_param(const Word& word);
std::string to_string(const LiraParam& params);
