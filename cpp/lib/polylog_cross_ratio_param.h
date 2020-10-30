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
constexpr int kRatioMaxIndex = kCompressionMaxValue;

// It's not impossible in general to have a CompoundRatio of the form
// (a - d) / (c - d), but in practice we have at least a cross ratio.
constexpr int kMinCompoundRatioComponents = 2;

class CompoundRatio;

// Represents a cross ratio [a,b,c,d] = ([a,b] * [c,d]) / ([b,c] * [d,a])
class CrossRatio {
public:
  CrossRatio() {
    indices_.fill(0);
  }
  CrossRatio(std::array<int, kCrossRatioElements> indices)
      : indices_(std::move(indices)) {
    CHECK(is_valid()) << list_to_string(indices_);
  }
  CrossRatio(absl::Span<const int> indices) {
    CHECK_EQ(indices.size(), kCrossRatioElements);
    absl::c_copy(indices, indices_.begin());
    CHECK(is_valid()) << list_to_string(indices_);
  }

  static CrossRatio one_minus(const CrossRatio& other) {
    const auto& v = other.indices_;
    return CrossRatio(std::array{v[0], v[2], v[1], v[3]});
  }

  bool is_valid() const {
    return absl::c_all_of(indices_, [](int idx) {
      return 1 <= idx && idx <= kRatioMaxIndex;
    });
  }

  const std::array<int, kCrossRatioElements>& indices() const { return indices_; }

  bool operator==(const CrossRatio& other) const { return indices_ == other.indices_; }
  bool operator< (const CrossRatio& other) const { return indices_ <  other.indices_; }

private:
  std::array<int, kCrossRatioElements> indices_;
};

inline CrossRatio CR(int a, int b, int c, int d) {
  return CrossRatio(std::array{a, b, c, d});
}

inline std::string to_string(const CrossRatio& ratio) {
  return absl::StrCat("[", str_join(ratio.indices(), ","), "]");
}


class CompoundRatio {
public:
  CompoundRatio() {}
  CompoundRatio(std::vector<Delta> numerator, std::vector<Delta> denominator)
    : numerator_(std::move(numerator)), denominator_(std::move(denominator)) {
    normalize();
  }

  static CompoundRatio from_cross_ratio(const CrossRatio ratio) {
    const auto& v = ratio.indices();
    static_assert(kCrossRatioElements == 4);
    return CompoundRatio(
      {Delta(v[0], v[1]), Delta(v[2], v[3])},
      {Delta(v[1], v[2]), Delta(v[3], v[0])}
    );
  }
  static CompoundRatio from_cross_ratio_product(const std::vector<CrossRatio>& ratios) {
    CompoundRatio ret;
    for (const auto& r : ratios) {
      ret.add(r);
    }
    return ret;
  }

  template<typename T>
  static CompoundRatio from_serialized(absl::Span<const T> data) {
    CHECK(data.size() % 4 == 0);
    const int n = data.size() / 2;
    std::vector<Delta> numerator, denominator;
    for (int i = 0; i < n; i += 2) {
      numerator.push_back(Delta(data[i], data[i+1]));
    }
    for (int i = n; i < 2*n; i += 2) {
      denominator.push_back(Delta(data[i], data[i+1]));
    }
    return CompoundRatio(std::move(numerator), std::move(denominator));
  }
  std::vector<int> serialized() const {
    std::vector<int> ret;
    // No need to mark segments split because enumerator and denominator are
    // always the same length.
    for (const Delta& d : numerator_) {
      ret.push_back(d.a());
      ret.push_back(d.b());
    }
    for (const Delta& d : denominator_) {
      ret.push_back(d.a());
      ret.push_back(d.b());
    }
    return ret;
  }

  void add(CompoundRatio ratio) {
    append_vector(numerator_, std::move(ratio.numerator_));
    append_vector(denominator_, std::move(ratio.denominator_));
    normalize();
  }
  void add(const CrossRatio& ratio) {
    add(CompoundRatio::from_cross_ratio(ratio));
  }

  const std::vector<Delta>& numerator()   const { return numerator_; }
  const std::vector<Delta>& denominator() const { return denominator_; }

  void normalize();

  std::optional<CompoundRatio> one_minus() const;

  void check() const {
    CHECK_EQ(numerator_.size(), denominator_.size());
    CHECK_GE(numerator_.size(), kMinCompoundRatioComponents);
  }

  auto as_tie() const { return std::tie(numerator_, denominator_); }

  bool operator==(const CompoundRatio& other) const { return as_tie() == other.as_tie(); }
  bool operator< (const CompoundRatio& other) const { return as_tie() <  other.as_tie(); }

private:
  // TODO: Store as non-overlaping loops instead (e.g. [1,2,3,4,5,6][7,8,9,10])
  std::vector<Delta> numerator_;
  std::vector<Delta> denominator_;
};

inline std::string to_string(const CompoundRatio& ratio) {
  std::string ret;
  for (const Delta& d : ratio.numerator()) {
    if (!ret.empty()) {
      ret += fmt::dot();
    }
    ret += absl::StrCat("[", d.a(), ",", d.b(), "]");
  }
  for (const Delta& d : ratio.denominator()) {
    ret += absl::StrCat("/[", d.a(), ",", d.b(), "]");
  }
  return ret;
}


class LiraParam {
public:
  LiraParam() {}
  LiraParam(std::vector<int> weights, std::vector<CompoundRatio> ratios)
      : weights_(std::move(weights)), ratios_(std::move(ratios)) {
    CHECK_EQ(weights_.size(), ratios_.size());
  }

  const std::vector<int>& weights() const { return weights_; }
  const std::vector<CompoundRatio>& ratios() const { return ratios_; }

  int depth() const { return ratios().size(); }
  int total_weight() const { return absl::c_accumulate(weights_, 0); }
  int sign() const { return neg_one_pow(depth()); }

  auto as_tie() const { return std::tie(weights_, ratios_); }

  bool operator==(const LiraParam& other) const { return as_tie() == other.as_tie(); }
  bool operator< (const LiraParam& other) const { return as_tie() <  other.as_tie(); }

private:
  std::vector<int> weights_;
  std::vector<CompoundRatio> ratios_;
};

Word lira_param_to_key(const LiraParam& params);
LiraParam key_to_lira_param(const Word& word);
std::string to_string(const LiraParam& params);
