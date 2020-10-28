#include "cotheta.h"


ThetaCoExpr epsilon_coexpr_to_theta_coexpr(
    const EpsilonCoExpr& expr,
    const std::vector<std::vector<CrossRatio>>& ratios) {
  ThetaCoExpr ret;
  expr.foreach([&](const std::vector<EpsilonPack>& term, int coeff) {
    CHECK_EQ(term.size(), kThetaCoExprComponents);
    const std::vector<ThetaExpr> multipliers =
      mapped(term, [&](const EpsilonPack& pack) {
        return epsilon_expr_to_theta_expr(EpsilonExpr::single(pack), ratios);
      });
    static_assert(kThetaCoExprComponents == 2);
    ret += coeff * outer_product<ThetaCoExpr>(
      multipliers[0],
      multipliers[1],
      [](const MultiWord& lhs, const MultiWord& rhs) {
        return std::array{lhs, rhs};
      }
    );
  });
  return ret;
}
