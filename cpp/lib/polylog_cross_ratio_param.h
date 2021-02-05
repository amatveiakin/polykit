#pragma once

#include <vector>

#include "check.h"
#include "compression.h"
#include "ratio.h"
#include "string.h"
#include "util.h"


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

using LiraParamCompressed = CompressedBlob<LiraParam>;

LiraParamCompressed lira_param_to_key(const LiraParam& param);
LiraParam key_to_lira_param(const LiraParamCompressed& key);
std::string lira_param_function_name(int foreweight, const std::vector<int>& weights);
std::string lira_param_function_name(const LiraParam& param);
std::string to_string(const LiraParam& params);
