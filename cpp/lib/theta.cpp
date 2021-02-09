#include "theta.h"

#include "algebra.h"



ThetaExpr TRatio(const CompoundRatio& ratio) {
  ThetaExpr ret;
  for (const std::vector<int>& l : ratio.loops()) {
    ret += delta_expr_to_theta_expr(cross_ratio(l));
  }
  return ret;
}

ThetaExpr TComplement(const CompoundRatio& ratio) {
  auto one_minus_ratio = CompoundRatio::one_minus(ratio);
  return one_minus_ratio.has_value()
    ? TRatio(std::move(one_minus_ratio.value()))
    : ThetaExpr::single(std::vector<Theta>{ThetaComplement(std::move(ratio))});
}

ThetaExpr TComplement(std::initializer_list<std::initializer_list<int>> indices) {
  std::vector<CrossRatio> ratios;
  for (auto&& r : indices) {
    ratios.push_back(CrossRatio(r));
  }
  return TComplement(CompoundRatio::from_cross_ratio_product(ratios));
}

ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<CompoundRatio>& compound_ratios) {
  return expr.mapped_expanding([&](const EpsilonPack& term) {
    if (epsilon_pack_is_unity(term)) {
      return TUnity();
    }
    return std::visit(overloaded{
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
                for (int idx : range(kMaxComplementVariables)) {
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
  }).without_annotations();
}

ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<std::vector<CrossRatio>>& cross_ratios) {
  return epsilon_expr_to_theta_expr(
    expr,
    mapped(cross_ratios, CompoundRatio::from_cross_ratio_product)
  );
}

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
        FATAL("Unexpected formal symbol: " + to_string(formal_symbol));
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
      FATAL("Expected a unity or a formal symbol, got " + to_string(term));
    }
  });
}

StringExpr count_functions(const ThetaExpr& expr) {
  return expr
    .termwise_abs()
    .mapped<StringExpr>(
      [&](const std::variant<std::vector<Theta>, LiraParam>& term) {
        return lira_param_function_name(std::get<LiraParam>(term));
      }
    ).without_annotations();
}

static bool is_monster(const ThetaPack& term) {
  return std::visit(overloaded{
    [](const std::vector<Theta>& term_product) {
      return absl::c_any_of(term_product, [](const Theta& t) {
        return !std::holds_alternative<Delta>(t);
      });
    },
    [](const LiraParam& formal_symbol) {
      return true;
    },
  }, term);
}

ThetaExpr theta_expr_without_monsters(const ThetaExpr& expr) {
  return expr.filtered([](const ThetaPack& term) {
    return !is_monster(term);
  });
}
ThetaExpr theta_expr_keep_monsters(const ThetaExpr& expr) {
  return expr.filtered([](const ThetaPack& term) {
    return is_monster(term);
  });
}
ThetaCoExpr theta_coexpr_without_monsters(const ThetaCoExpr& expr) {
  return expr.filtered([](const auto& term) {
    return !absl::c_any_of(term, is_monster);
  });
}
ThetaCoExpr theta_coexpr_keep_monsters(const ThetaCoExpr& expr) {
  return expr.filtered([](const auto& term) {
    return absl::c_any_of(term, is_monster);
  });
}
