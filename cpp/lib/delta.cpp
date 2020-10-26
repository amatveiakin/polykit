#include "delta.h"

#include "util.h"


DeltaAlphabetMapping delta_alphabet_mapping;


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<int>& new_points) {
  return expr.mapped<DeltaExpr>([&](const std::vector<Delta>& term) {
    return mapped(term,
      [&](const Delta& d) {
        return Delta(new_points.at(d.a() - 1), new_points.at(d.b() - 1));
      });
  });
}
