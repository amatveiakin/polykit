#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_polylog_qli.h"


namespace py = pybind11;


PYBIND11_MODULE(polylog_qli, m) {
  m.def("QLi", &lazy_qli, "Creates a lazy expression for QLi");
  m.def("QLiNeg", &lazy_qli_neg, "Creates a lazy expression for QLiNeg");
  m.def("QLiSymm", &lazy_qli_symm, "Creates a lazy expression for QLiSymm");
}
