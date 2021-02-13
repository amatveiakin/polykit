#include "polylog_lira.h"

#include "polylog_li.h"


ThetaExpr LiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios) {
  std::vector<std::vector<int>> points;
  for (int i : range_incl(1, ratios.size())) {
    points.push_back({i});
  }
  return substitute_ratios(
    LiVec(foreweight, weights, points),
    ratios
  );
}

ThetaExpr LiraVec(const LiraParam& param) {
  return LiraVec(param.foreweight(), param.weights(), param.ratios());
}

ThetaCoExpr CoLiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios) {
  std::vector<std::vector<int>> points;
  for (int i : range_incl(1, ratios.size())) {
    points.push_back({i});
  }
  return substitute_ratios(
    CoLiVec(foreweight, weights, points),
    ratios
  );
}

ThetaCoExpr CoLiraVec(const LiraParam& param) {
  return CoLiraVec(param.foreweight(), param.weights(), param.ratios());
}


ThetaExpr eval_formal_symbols(const ThetaExpr& expr) {
  return expr.mapped_expanding([&](const ThetaPack& term) {
    if (is_unity(term)) {
      return TUnity();
    }
    return std::visit(overloaded{
        [&](const std::vector<Theta>& term_product) {
          return ThetaExpr::single(term);
        },
        [&](const LiraParam& as_cross_ratios) {
          return LiraVec(as_cross_ratios);
        },
      }, term);
  });
}

ThetaCoExpr eval_formal_symbols(const ThetaCoExpr& expr) {
  return expr.mapped_expanding([&](const std::array<ThetaPack, kThetaCoExprComponents>& term) {
    const std::array multipliers =
      mapped_array(term, [&](const ThetaPack& pack) {
        return eval_formal_symbols(ThetaExpr::single(pack));
      });
    static_assert(kThetaCoExprComponents == 2);
    return outer_product<ThetaCoExpr>(
      multipliers[0],
      multipliers[1],
      [](const ThetaExpr::StorageT& lhs, const ThetaExpr::StorageT& rhs) {
        return std::array{lhs, rhs};
      },
      AnnOperator(fmt::coprod_hopf())
    );
  });
}
