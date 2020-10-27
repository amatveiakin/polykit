#pragma once

#include "absl/algorithm/container.h"

#include "word.h"


class LiParam {
public:
  LiParam() {}
  // Neither weights nor points can contain the kCompressionSentinel.
  LiParam(std::vector<int> weights, std::vector<std::vector<int>> points)
      : weights_(std::move(weights)), points_(std::move(points)) {
    CHECK_EQ(weights.size(), points.size());
    for (auto& p : points_) {
      absl::c_sort(p);
    }
  }

  const std::vector<int>& weights() const { return weights_; }
  const std::vector<std::vector<int>>& points() const { return points_; }

  int total_weight() const { return absl::c_accumulate(weights_, 0); }

private:
  std::vector<int> weights_;
  std::vector<std::vector<int>> points_;
};

Word li_param_to_key(const LiParam& params);
LiParam key_to_li_param(const Word& word);
std::string to_string(const LiParam& params);

// For sorting when printing
inline bool operator<(const LiParam& lhs, const LiParam& rhs) {
  return std::tie(lhs.weights(), lhs.points()) < std::tie(rhs.weights(), rhs.points());
}
