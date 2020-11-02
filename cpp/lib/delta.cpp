#include "delta.h"

#include "util.h"


DeltaAlphabetMapping delta_alphabet_mapping;


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<X>& new_points) {
  DeltaExpr ret;
  expr.foreach([&](const std::vector<Delta>& term_old, int coeff) {
    std::vector<Delta> term_new;
    for (const Delta& d_old : term_old) {
      Delta d_new(new_points.at(d_old.a() - 1), new_points.at(d_old.b() - 1));
      if (d_new.is_nil()) {
        return;
      }
      term_new.push_back(d_new);
    }
    ret.add_to(term_new, coeff);
  });
  return ret;
}
