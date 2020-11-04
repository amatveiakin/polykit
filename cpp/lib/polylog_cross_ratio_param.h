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
  return fmt::brackets(str_join(ratio.indices(), ","));
}


class CompoundRatio {
public:
  CompoundRatio() {}
  CompoundRatio(std::vector<std::vector<int>> loops)
    : loops_(std::move(loops)) {
    normalize();
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
    CHECK_EQ(size_vec.size(), 1);
    const int size = size_vec.front();
    for (int i = 0; i < size; ++i) {
      loops.push_back(decompressor.next_segment());
    }
    return CompoundRatio(std::move(loops));
  }
  void compress(Compressor& compressor) const {
    compressor.add_segment({int(loops_.size())});
    for (const auto& l : loops_) {
      compressor.add_segment(l);
    }
  }

  void add(const CompoundRatio& ratio) {
    append_vector(loops_, ratio.loops_);
    normalize();
  }
  void add(const CrossRatio& ratio) {
    add(CompoundRatio::from_cross_ratio(ratio));
  }

  std::vector<std::vector<int>> loops() const { return loops_; };

  void normalize();

  std::optional<CompoundRatio> one_minus() const;

  void check() const {
    CHECK(!loops_.empty());
    CHECK(loops_.front().size() % 2 == 0);
    CHECK_GE(loops_.front().size(), kMinCompoundRatioComponents);
  }

  bool operator==(const CompoundRatio& other) const { return loops_ == other.loops_; }
  bool operator< (const CompoundRatio& other) const { return loops_ <  other.loops_; }

private:
  std::vector<std::vector<int>> loops_;
};

inline std::string to_string(const CompoundRatio& ratio) {
  return str_join(
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

private:
  int foreweight_ = 0;
  std::vector<int> weights_;
  std::vector<CompoundRatio> ratios_;
};

Word lira_param_to_key(const LiraParam& params);
LiraParam key_to_lira_param(const Word& word);
std::string lira_param_function_name(const LiraParam& param);
std::string to_string(const LiraParam& params);
