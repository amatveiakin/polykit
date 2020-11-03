#include "theta.h"

#include "algebra.h"


ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<CompoundRatio>& compound_ratios) {
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
          return TFormalSymbol(LiraParam(
            formal_symbol.foreweight(), formal_symbol.weights(), symbol_ratios));
        },
      }, term);
  });
  return ret;
}

ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<std::vector<CrossRatio>>& cross_ratios) {
  return epsilon_expr_to_theta_expr(
    expr,
    mapped(cross_ratios, CompoundRatio::from_cross_ratio_product));
}

ThetaExpr delta_expr_to_theta_expr(const DeltaExpr& expr) {
  return expr.mapped<ThetaExpr>([](const std::vector<Delta>& term) -> ThetaPack {
    return mapped(term, [](const Delta& d) -> Theta {
      return d;
    });
  });
}

DeltaExpr theta_expr_to_delta_expr(const ThetaExpr& expr) {
  return expr.mapped<DeltaExpr>([&](const ThetaPack& term) -> std::vector<Delta> {
    return std::visit(overloaded{
      [](const std::vector<Theta>& term_product) -> std::vector<Delta> {
        CHECK_GE(term_product.size(), 1);
        return mapped(term_product, [](const Theta& t) {
          CHECK(std::holds_alternative<Delta>(t)) << to_string(t);
          return std::get<Delta>(t);
        });
      },
      [](const LiraParam& formal_symbol) -> std::vector<Delta> {
        FAIL("Unexpected formal symbol: " + to_string(formal_symbol));
      },
    }, term);
  });
}

ThetaExpr theta_expr_keep_monsters(const ThetaExpr& expr) {
  return expr.filtered([](const ThetaPack& term) {
    return std::visit(overloaded{
      [](const std::vector<Theta>& term_product) -> bool {
        return absl::c_any_of(term_product, [](const Theta& t) {
          return !std::holds_alternative<Delta>(t);
        });
      },
      [](const LiraParam& formal_symbol) -> bool {
        FAIL("Unexpected formal symbol: " + to_string(formal_symbol));
      },
    }, term);
  });
}

ThetaExpr update_foreweight(
    const ThetaExpr& expr,
    int new_foreweight) {
  return expr.without_annotations().mapped<ThetaExpr>([&](const ThetaPack& term) -> ThetaPack {
    if (theta_pack_is_unity(term)) {
      return term;
    } else if (std::holds_alternative<LiraParam>(term)) {
      const LiraParam& param = std::get<LiraParam>(term);
      CHECK_EQ(param.foreweight(), 1);
      return LiraParam(new_foreweight, param.weights(), param.ratios());
    } else {
      FAIL("Expected a unity or a formal symbol, got " + to_string(term));
    }
  });
}

StringExpr count_functions(const ThetaExpr& expr) {
  StringExpr ret;
  expr.foreach([&](const std::variant<std::vector<Theta>, LiraParam>& term, int coeff) {
    ret.add_to(lira_param_function_name(std::get<LiraParam>(term)), std::abs(coeff));
  });
  return ret;
}
