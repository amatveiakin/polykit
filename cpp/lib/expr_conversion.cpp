#include "expr_conversion.h"


GammaExpr delta_expr_to_gamma_expr(const DeltaExpr& expr) {
  return expr.mapped<GammaExpr>([](const std::vector<Delta>& term) {
    return mapped(term, [](const Delta& d) {
      return Gamma({d.a().as_simple_var(), d.b().as_simple_var()});
    });
  });
}

DeltaExpr gamma_expr_to_delta_expr(const GammaExpr& expr) {
  return expr.mapped<DeltaExpr>([&](const std::vector<Gamma>& term) {
    return mapped(term, [](const Gamma& g) {
      const auto vars = g.index_vector();
      CHECK_EQ(vars.size(), 2);
      return Delta(vars[0], vars[1]);
    });
  });
}
