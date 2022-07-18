#include "polylog_type_d_space.h"

#include "itertools.h"
#include "polylog_gr_space.h"


static constexpr int kNumPoints = 6;

static KappaExpr kappa_y_to_x(const KappaExpr& expr) {
  return expr.mapped([&](const auto& term) {
    return mapped(term, [&](const Kappa& k) {
      return std::visit(overloaded {
        [&](const KappaX&) -> Kappa { return KappaY{}; },
        [&](const KappaY&) -> Kappa { return KappaX{}; },
        [&](const Gamma& g) -> Kappa { return g; }
      }, k);
    });
  });
}

static KappaExpr cycle_indices(const KappaExpr& expr, int shift) {
  return expr.mapped([&](const auto& term) {
    return mapped(term, [&](const Kappa& k) {
      return std::visit(overloaded {
        [&](const KappaX&) -> Kappa { return KappaX{}; },
        [&](const KappaY&) -> Kappa { return KappaY{}; },
        [&](const Gamma& g) -> Kappa {
          return Gamma(mapped(g.index_vector(), [&](const int idx) {
            return pos_mod(idx + shift - 1, kNumPoints) + 1;
          }));
        },
      }, k);
    });
  });
}


TypeD_Space typeD_free_lie_coalgebra(int weight) {
  const auto coords = concat(
    mapped(combinations({1,2,3,4,5,6}, 3), [](const auto& points) {
      return Kappa(Gamma(points));
    }),
    std::vector{ Kappa(KappaX{}), Kappa(KappaY{}) }
  );
  return mapped(get_lyndon_words(coords, weight), [](const auto& word) {
    return KappaExpr::single(word);
  });
}

TypeD_Space typeD_Fx() {
  return concat(
    mapped(combinations(seq_incl(1, kNumPoints), 3), [](const auto& points) {
      return KappaExpr::single({Kappa(Gamma(points))});
    }),
    {K_X(), K_Y()}
  );
}

std::vector<std::array<KappaExpr, 3>> typeD_B2_generators() {
  const std::vector<std::vector<KappaExpr>> b2_generators_y_cyclable = {
    {
      K(1,3,4) + K_Y(),
      K(1,3,6) + K(1,4,5) + K(2,3,4),
      K(1,4,6) + K(3,4,5) + K(1,2,3),
    },
    {
      K(3,4,6) + K_Y(),
      K(1,4,6) + K(2,3,6) + K(3,4,5),
      K(1,3,6) + K(2,3,4) + K(4,5,6),
    },
    {
      K(2,3,6) + K(1,4,5),
      K_Y(),
      K(1,2,3) + K(4,5,6),
    },
  };
  const std::vector<std::vector<KappaExpr>> b2_generators_y_fixed = {
    {
      K(1,3,5) + K_Y(),
      K(1,2,3) + K(3,4,5) + K(1,5,6),
      K(1,3,6) + K(2,3,5) + K(1,4,5),
    },
    {
      K(2,4,6) + K_Y(),
      K(4,5,6) + K(2,3,4) + K(1,2,6),
      K(1,4,6) + K(2,4,5) + K(2,3,6),
    },
  };
  for (const auto& expr : flatten(b2_generators_y_fixed)) {
    CHECK(expr == cycle_indices(expr, 2));
  }

  const auto b2_generators_x_cyclable = mapped_nested<2>(b2_generators_y_cyclable, kappa_y_to_x);
  const auto b2_generators_x_fixed = mapped_nested<2>(b2_generators_y_fixed, kappa_y_to_x);

  const auto b2_generators = concat(
    mapped_nested<2>(b2_generators_y_cyclable, [](const auto& expr) { return cycle_indices(expr, 0); }),
    mapped_nested<2>(b2_generators_y_cyclable, [](const auto& expr) { return cycle_indices(expr, 2); }),
    mapped_nested<2>(b2_generators_y_cyclable, [](const auto& expr) { return cycle_indices(expr, 4); }),
    mapped_nested<2>(b2_generators_y_fixed, [](const auto& expr) { return cycle_indices(expr, 0); }),
    mapped_nested<2>(b2_generators_x_cyclable, [](const auto& expr) { return cycle_indices(expr, 1); }),
    mapped_nested<2>(b2_generators_x_cyclable, [](const auto& expr) { return cycle_indices(expr, 3); }),
    mapped_nested<2>(b2_generators_x_cyclable, [](const auto& expr) { return cycle_indices(expr, 5); }),
    mapped_nested<2>(b2_generators_x_fixed, [](const auto& expr) { return cycle_indices(expr, 1); })
  );
  return mapped(b2_generators, DISAMBIGUATE(to_array<3>));
}

TypeD_Space typeD_B2() {
  return concat(
    mapped(GrL2(3, seq_incl(1, kNumPoints)), gamma_expr_to_kappa_expr),
    mapped(typeD_B2_generators(), [](const auto& gen) {
      // (a-c) * (b-c) == a*b + b*c + c*a
      return NLog(2, gen);
    })
  );
}
