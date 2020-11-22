#include "projection.h"


WordExpr project_on(int axis, const DeltaExpr& expr) {
  return expr.mapped_expanding([&](const std::vector<Delta>& deltas) {
    Word word;
    for (const Delta& d : deltas) {
      CHECK(!d.is_nil());
      if (d.a() == axis) {
        word.push_back(d.b());
      } else if (d.b() == axis) {
        word.push_back(d.a());
      } else {
        return WordExpr{};
      }
    }
    return WordExpr::single(word);
  });
}
