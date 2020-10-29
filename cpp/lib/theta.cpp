#include "theta.h"

#include "algebra.h"


ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<std::vector<CrossRatio>>& cross_ratios) {
  const std::vector<CompoundRatio>& compound_ratios =
    mapped(cross_ratios, CompoundRatio::from_cross_ratio_product);
  ThetaExpr ret;
  expr.foreach([&](const EpsilonPack& term, int coeff) {
    if (epsilon_pack_is_unity(term)) {
      ret += coeff * TUnity();
      return;
    }
    ret += coeff *
      std::visit(overloaded{
        [&](const std::vector<Epsilon>& term_product) {
          return tensor_product(absl::MakeConstSpan(mapped(
            absl::MakeConstSpan(term_product),
            [&](const Epsilon& e) -> ThetaExpr {
              return std::visit(overloaded{
                [&](const EpsilonVariable& v) {
                  return TRatio(compound_ratios.at(v.idx() - 1));
                },
                [&](const EpsilonComplement& v) {
                  CompoundRatio ratio_prod;
                  for (int idx = 0; idx < kMaxComplementVariables; ++idx) {
                    if (v.indices()[idx]) {
                      ratio_prod.add(compound_ratios.at(idx - 1));
                    }
                  }
                  return TComplement(std::move(ratio_prod));
                },
              }, e);
            }
          )));
        },
        [&](const LiParam& formal_symbol) {
          std::vector<CompoundRatio> symbol_ratios;
          for (const std::vector<int>& point_arg: formal_symbol.points()) {
            symbol_ratios.push_back({});
            for (const int p : point_arg) {
              symbol_ratios.back().add(compound_ratios.at(p - 1));
            }
          }
          return TFormalSymbolPositive(LiraParam(formal_symbol.weights(), symbol_ratios));
        },
      }, term);
  });
  return ret;
}
