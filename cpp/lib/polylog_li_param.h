// `LiParam` contains the full set of parameters required to compute a Li function
// and, by extension, describes a formal symbol.

#pragma once

#include "absl/algorithm/container.h"

#include "compression.h"
#include "util.h"


class LiParam {
public:
  LiParam() {}
  LiParam(int foreweight, std::vector<int> weights, std::vector<std::vector<int>> points)
      : foreweight_(foreweight), weights_(std::move(weights)), points_(std::move(points)) {
    CHECK_EQ(weights_.size(), points_.size());
    CHECK_GT(weights_.size(), 0);
    CHECK_GE(foreweight_, 0);
    CHECK(absl::c_all_of(weights_, [](int w) { return w >= 1; }));
    for (auto& p : points_) {
      absl::c_sort(p);
    }
  }

  int foreweight() const { return foreweight_; }
  const std::vector<int>& weights() const { return weights_; }
  const std::vector<std::vector<int>>& points() const { return points_; }

  int depth() const { return points().size(); }
  int total_weight() const { return foreweight_ + sum(weights_); }
  int sign() const { return neg_one_pow(depth()); }

  auto as_tie() const { return std::tie(weights_, points_); }

  bool operator==(const LiParam& other) const { return as_tie() == other.as_tie(); }
  bool operator< (const LiParam& other) const { return as_tie() <  other.as_tie(); }

  template <typename H>
  friend H AbslHashValue(H h, const LiParam& param) {
    return H::combine(std::move(h), param.foreweight_, param.weights_, param.points_);
  }

private:
  int foreweight_ = 0;
  std::vector<int> weights_;
  std::vector<std::vector<int>> points_;
};

using LiParamCompressed = CompressedBlob<LiParam>;

LiParamCompressed li_param_to_key(const LiParam& param);
LiParam key_to_li_param(const LiParamCompressed& key);
std::string to_string(const LiParam& params);
