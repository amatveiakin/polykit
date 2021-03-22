#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_delta.h"
#include "lib/polylog_qli.h"


namespace py = pybind11;

LazyDeltaExpr lazy_qli(int weight, std::vector<X> points) {
  return make_lazy("QLi", &QLiVec, weight, points);
}

LazyDeltaExpr lazy_qli_neg(int weight, std::vector<X> points) {
  return make_lazy("QLiNeg", &QLiNegVec, weight, points);
}

LazyDeltaExpr lazy_qli_symm(int weight, std::vector<X> points) {
  return make_lazy("QLiSymm", &QLiSymmVec, weight, points);
}


PYBIND11_MODULE(polylog_qli, m) {
  m.def("QLi", &lazy_qli, "Creates a lazy expression for QLi");
  m.def("QLiNeg", &lazy_qli_neg, "Creates a lazy expression for QLiNeg");
  m.def("QLiSymm", &lazy_qli_symm, "Creates a lazy expression for QLiSymm");
}
