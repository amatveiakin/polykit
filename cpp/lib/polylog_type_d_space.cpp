#include "polylog_type_d_space.h"

#include "itertools.h"


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
