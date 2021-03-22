#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_delta.h"
#include "lib/polylog_qli.h"


LazyDeltaExpr lazy_qli(int weight, std::vector<X> points) {
  return make_lazy("QLi", &QLiVec, weight, points);
}

namespace py = pybind11;

PYBIND11_MODULE(polylog_qli, m) {
  m.def("QLi", &lazy_qli, "Creates a lazy expression for QLi");
}
