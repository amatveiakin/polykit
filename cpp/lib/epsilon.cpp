#include  "epsilon.h"

#include  "algebra.h"


EpsilonExpr epsilon_expr_substitute(
    const EpsilonExpr& expr,
    const std::vector<std::vector<int>>& new_products) {
  EpsilonExpr ret;
  expr.foreach([&](const std::vector<Epsilon>& term, int coeff) {
    ret += coeff * tensor_product(absl::MakeConstSpan(mapped(
      absl::MakeConstSpan(term),
      [&](const Epsilon& e) -> EpsilonExpr {
        return std::visit(overloaded{
          [&](const EpsilonVariable& v) {
            const auto& new_indices = new_products.at(v.idx() - 1);
            EpsilonExpr term;
            for (const int new_idx : new_indices) {
              term += EVar(new_idx);
            }
            return term;
          },
          [&](const EpsilonMonster& v) {
            std::bitset<kMaxMonsterVariables> term_indices;
            for (int idx = 0; idx < kMaxMonsterVariables; ++idx) {
              if (v.indices()[idx]) {
                const auto& new_indices = new_products.at(idx - 1);
                for (const int new_idx : new_indices) {
                  CHECK_LT(new_idx, kMaxMonsterVariables);
                  CHECK(term_indices[new_idx] == 0) << "Epsilon doesn't support duplicate indices.";
                  term_indices[new_idx] = 1;
                }
              }
            }
            return EMonsterIndexSet(term_indices);
          },
        }, e);
      })));
  });
  return ret;
}

EpsilonExpr epsilon_expr_without_monsters(const EpsilonExpr& expr) {
  return expr.filtered([](const std::vector<Epsilon>& term) {
    return absl::c_all_of(
      term,
      [](const Epsilon& e) {
        return std::visit(overloaded{
          [](const EpsilonVariable&) { return true; },
          [](const EpsilonMonster& v) { return v.indices().count() == 1; },
        }, e);
      }
    );
  });
}
