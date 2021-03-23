#include "lib/lazy/lazy_delta.h"
#include "lib/polylog_qli.h"


// TODO: Rename lazy_... to ..._lazy
LazyDeltaExpr lazy_qli(int weight, std::vector<X> points) {
  return make_lazy("QLi", &QLiVec, weight, points);
}

LazyDeltaExpr lazy_qli_neg(int weight, std::vector<X> points) {
  return make_lazy("QLiNeg", &QLiNegVec, weight, points);
}

LazyDeltaExpr lazy_qli_symm(int weight, std::vector<X> points) {
  return make_lazy("QLiSymm", &QLiSymmVec, weight, points);
}
