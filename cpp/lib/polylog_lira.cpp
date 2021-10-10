#include "polylog_lira.h"

#include "polylog_li.h"


ThetaExpr LiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios) {
  return LiraVec(LiraParam(foreweight, weights, ratios));
}

ThetaExpr LiraVec(const LiraParam& param) {
  std::vector<std::vector<int>> points;
  for (int i : range_incl(1, param.ratios().size())) {
    points.push_back({i});
  }
  return substitute_ratios(
    LiVec(param.foreweight(), param.weights(), points),
    param.ratios()
  ).annotate(to_string(param));
}

ThetaICoExpr CoLiraVec(
    int foreweight,
    const std::vector<int>& weights,
    const std::vector<CompoundRatio>& ratios) {
  return CoLiraVec(LiraParam(foreweight, weights, ratios));
}

ThetaICoExpr CoLiraVec(const LiraParam& param) {
  std::vector<std::vector<int>> points;
  for (int i : range_incl(1, param.ratios().size())) {
    points.push_back({i});
  }
  return substitute_ratios(
    CoLiVec(param.foreweight(), param.weights(), points),
    param.ratios()
  ).annotate(fmt::comult() + to_string(param));;
}


ThetaExpr eval_formal_symbols(const ThetaExpr& expr) {
  return expr.mapped_expanding([&](const ThetaPack& term) {
    if (is_unity(term)) {
      return TUnity();
    }
    return std::visit(overloaded{
        [&](const std::vector<Theta>& /*term_product*/) {
          return ThetaExpr::single(term);
        },
        [&](const LiraParam& as_cross_ratios) {
          return LiraVec(as_cross_ratios);
        },
      }, term);
  });
}

ThetaICoExpr eval_formal_symbols(const ThetaICoExpr& expr) {
  return expr.mapped_expanding([&](const std::array<ThetaPack, kThetaCoExprParts>& term) {
    const std::array multipliers =
      mapped_array(term, [&](const ThetaPack& pack) {
        return eval_formal_symbols(ThetaExpr::single(pack));
      });
    static_assert(kThetaCoExprParts == 2);
    return outer_product<ThetaICoExpr>(
      multipliers[0],
      multipliers[1],
      [](const ThetaExpr::StorageT& lhs, const ThetaExpr::StorageT& rhs) {
        return std::array{lhs, rhs};
      },
      AnnOperator(fmt::coprod_hopf())
    );
  });
}
