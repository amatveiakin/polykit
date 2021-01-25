#include "cotheta.h"


ThetaCoExpr epsilon_coexpr_to_theta_coexpr(
    const EpsilonCoExpr& expr,
    const std::vector<std::vector<CrossRatio>>& ratios) {
  return expr.mapped_expanding([&](const std::vector<EpsilonPack>& term) {
    CHECK_EQ(term.size(), kThetaCoExprComponents);
    const std::vector<ThetaExpr> multipliers =
      mapped(term, [&](const EpsilonPack& pack) {
        return epsilon_expr_to_theta_expr(EpsilonExpr::single(pack), ratios);
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
  }).without_annotations();
}
