#pragma once

#include "delta.h"
#include "polylog_lira_param.h"
#include "theta.h"


#define HAS_LIRA_EXPR 1

// Represents a particular case of LiraParam, namely
//   n_Li_1_1..._1(x_1, ..., x_n),  where each x_i each a single cross-ratio
class LiraParamOnes {
public:
  LiraParamOnes() {}
  explicit LiraParamOnes(std::vector<CrossRatioNOrUnity> ratios) : ratios_(std::move(ratios)) {
    CHECK_GT(ratios_.size(), 0);
  }

  int foreweight() const { return ratios_.size(); }
  std::vector<int> weights() const { return std::vector<int>(ratios_.size(), 1); }
  const std::vector<CrossRatioNOrUnity>& ratios() const { return ratios_; }

  int depth() const { return ratios().size(); }
  int total_weight() const { return ratios_.size() * 2; }
  int sign() const { return neg_one_pow(depth()); }

  auto operator<=>(const LiraParamOnes&) const = default;

  template <typename H>
  friend H AbslHashValue(H h, const LiraParamOnes& param) {
    return H::combine(std::move(h), param.ratios_);
  }

private:
  std::vector<CrossRatioNOrUnity> ratios_;
};


struct LiraExprParam : SimpleLinearParam<LiraParamOnes> {
  using VectorT = std::vector<CrossRatioNOrUnity>;
  static VectorT key_to_vector(const StorageT& key) { return key.ratios(); }
  static StorageT vector_to_key(const VectorT& vec) { return LiraParamOnes(vec); }
  LYNDON_COMPARE_DEFAULT

  static std::string object_to_string(const LiraParamOnes& param) {
    return fmt::function(
      lira_param_function_name(param.foreweight(), param.weights()),
      mapped_to_string(param.ratios()),
      HSpacing::sparse
    );
  }
};

using LiraExpr = Linear<LiraExprParam>;


inline CrossRatioN to_cross_ratio(const CompoundRatio& compound_ratio) {
  CHECK_EQ(compound_ratio.loops().size(), 1) << "Only cross ratios are supported";
  return CrossRatioN(to_array<4>(compound_ratio.loops().front()));
}

inline CrossRatioNOrUnity to_cross_ratio_or_unity(const CompoundRatio& compound_ratio) {
  return compound_ratio.is_unity()
    ? CrossRatioNOrUnity::unity()
    : to_cross_ratio(compound_ratio);
}

int num_distinct_ratio_variables(const std::vector<CrossRatioNOrUnity>& ratios);
int num_ratio_points(const std::vector<CrossRatioN>& ratios);
bool are_ratios_independent(const std::vector<CrossRatioN>& ratios);


LiraExpr without_unities(const LiraExpr& expr);
LiraExpr keep_distinct_ratios(const LiraExpr& expr);
LiraExpr keep_independent_ratios(const LiraExpr& expr);

// Applies rule:
//   {x_1, ..., x_n} = (-1)^n * {1/x_1, ..., 1/x_n}
LiraExpr normalize_inverse(const LiraExpr& expr);

// Sorts each cross ratio points ascendingly, adjusting the sign accordingly.
LiraExpr fully_normalize_ratios(const LiraExpr& expr);

LiraExpr theta_expr_to_lira_expr_without_products(const ThetaExpr& expr);

DeltaICoExpr lira_expr_comultiply(const LiraExpr& expr);
