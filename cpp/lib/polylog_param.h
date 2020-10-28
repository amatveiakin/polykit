#pragma once

#include "absl/algorithm/container.h"

#include "word.h"


class LiParam {
public:
  LiParam() {}
  LiParam(std::vector<int> weights, std::vector<std::vector<int>> points)
      : weights_(std::move(weights)), points_(std::move(points)) {
    CHECK_EQ(weights_.size(), points_.size());
    for (auto& p : points_) {
      absl::c_sort(p);
    }
  }

  const std::vector<int>& weights() const { return weights_; }
  const std::vector<std::vector<int>>& points() const { return points_; }

  int sign() const { return neg_one_pow(points().size()); }
  int total_weight() const { return absl::c_accumulate(weights_, 0); }

  auto as_tie() const { return std::tie(weights_, points_); }

  bool operator==(const LiParam& other) const { return as_tie() == other.as_tie(); }
  bool operator< (const LiParam& other) const { return as_tie() <  other.as_tie(); }

private:
  std::vector<int> weights_;
  std::vector<std::vector<int>> points_;
};

Word li_param_to_key(const LiParam& params);
LiParam key_to_li_param(const Word& word);
std::string to_string(const LiParam& params);
