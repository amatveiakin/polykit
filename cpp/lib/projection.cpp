#include "projection.h"


IntWordExpr project_on(int axis, const DeltaExpr& expr) {
  IntWordExpr ret;
  expr.foreach([&](const std::vector<Delta>& deltas, int coeff) {
    IntWord word;
    for (const Delta& d : deltas) {
      CHECK(!d.is_nil());
      if (d.a() == axis) {
        word.push_back(d.b());
      } else if (d.b() == axis) {
        word.push_back(d.a());
      } else {
        return;
      }
    }
    ret.add_to_key(word, coeff);
  });
  return ret;
}
