#include "theta.h"

#include "algebra.h"


ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<std::vector<CrossRatio>>& ratios) {
  ThetaExpr ret;
  expr.foreach([&](const EpsilonPack& term, int coeff) {
    ret += coeff *
      std::visit(overloaded{
        [&](const std::vector<Epsilon>& term_product) {
          return tensor_product(absl::MakeConstSpan(mapped(
            absl::MakeConstSpan(term_product),
            [&](const Epsilon& e) -> ThetaExpr {
              return std::visit(overloaded{
                [&](const EpsilonVariable& v) {
                  const auto& v_ratios = ratios.at(v.idx() - 1);
                  ThetaExpr term;
                  for (const CrossRatio& ratio : v_ratios) {
                    term += TRatio(ratio);
                  }
                  return term;
                },
                [&](const EpsilonComplement& v) {
                  std::vector<CrossRatio> v_ratios;
                  for (int idx = 0; idx < kMaxComplementVariables; ++idx) {
                    if (v.indices()[idx]) {
                      append_vector(v_ratios, ratios.at(idx - 1));
                    }
                  }
                  return TComplement(std::move(v_ratios));
                },
              }, e);
            }
          )));
        },
        [&](const LiParam& formal_symbol) {
          std::vector<std::vector<CrossRatio>> symbol_ratios;
          for (const std::vector<int>& point_arg: formal_symbol.points()) {
            symbol_ratios.push_back({});
            for (const int p : point_arg) {
              append_vector(symbol_ratios.back(), ratios.at(p - 1));
            }
          }
          return TFormalSymbolPositive(LiraParam(formal_symbol.weights(), symbol_ratios));
        },
      }, term);
  });
  return ret;
}
