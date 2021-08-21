#include  "epsilon.h"

#include  "algebra.h"


EpsilonExpr substitute_variables(
    const EpsilonExpr& expr,
    const std::vector<std::vector<int>>& new_products) {
  return expr.mapped_expanding([&](const EpsilonPack& term) {
    return std::visit(overloaded{
      [&](const std::vector<Epsilon>& term_product) {
        return tensor_product(absl::MakeConstSpan(mapped(
          term_product,
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
              [&](const EpsilonComplement& v) {
                std::bitset<kMaxComplementVariables> term_indices;
                for (int idx : range(kMaxComplementVariables)) {
                  if (v.indices()[idx]) {
                    const auto& new_indices = new_products.at(idx - 1);
                    for (const int new_idx : new_indices) {
                      CHECK_LT(new_idx, kMaxComplementVariables);
                      CHECK(term_indices[new_idx] == 0) << "Epsilon doesn't support duplicate indices.";
                      term_indices[new_idx] = 1;
                    }
                  }
                }
                return EComplementIndexSet(term_indices);
              },
            }, e);
          }
        )));
      },
      [&](const LiParam& formal_symbol) {
        std::vector<std::vector<int>> new_points;
        for (const std::vector<int>& point_arg: formal_symbol.points()) {
          new_points.push_back({});
          for (const int p : point_arg) {
            append_vector(new_points.back(), new_products.at(p - 1));
          }
        }
        return EFormalSymbolPositive(LiParam(
          formal_symbol.foreweight(), formal_symbol.weights(), std::move(new_points)));
      },
    }, term);
  }).without_annotations();
}

static bool is_monster(const EpsilonPack& term) {
  return std::visit(overloaded{
    [&](const std::vector<Epsilon>& product) {
      return absl::c_any_of(
        product,
        [](const Epsilon& e) {
          return std::visit(overloaded{
            [](const EpsilonVariable&) { return false; },
            [](const EpsilonComplement& v) { return v.indices().count() != 1; },
          }, e);
        }
      );
    },
    [](const LiParam& /*formal_symbol*/) {
      return false;
    },
  }, term);
}

EpsilonExpr without_monsters(const EpsilonExpr& expr) {
  return expr.filtered([](const EpsilonPack& term) {
    return !is_monster(term);
  });
}
EpsilonExpr keep_monsters(const EpsilonExpr& expr) {
  return expr.filtered([](const EpsilonPack& term) {
    return is_monster(term);
  });
}
EpsilonCoExpr without_monsters(const EpsilonCoExpr& expr) {
  return expr.filtered([](const auto& term) {
    return !absl::c_any_of(term, is_monster);
  });
}
EpsilonCoExpr keep_monsters(const EpsilonCoExpr& expr) {
  return expr.filtered([](const auto& term) {
    return absl::c_any_of(term, is_monster);
  });
}
