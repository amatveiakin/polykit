#include "delta.h"

#include "util.h"


DeltaAlphabetMapping delta_alphabet_mapping;


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<int>& new_points) {
  DeltaExpr ret;
  expr.main().foreach([&](const std::vector<Delta>& monom, int coeff) {
    ret += coeff * DeltaExpr::single(mapped(monom, [&](const Delta& d) -> Delta {
      return Delta(new_points.at(d.a() - 1), new_points.at(d.b() - 1));
    }));
  });
  return ret;
}
