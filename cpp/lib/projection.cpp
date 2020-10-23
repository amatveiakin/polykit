#include "projection.h"


WordExpr project_on(int axis, const DeltaExpr& expr) {
  WordExpr ret;
  expr.foreach([&](const std::vector<Delta>& deltas, int coeff) {
    Word word;
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
