#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/polylog_qli.h"


namespace py = pybind11;

PYBIND11_MODULE(polylog_qli, m) {
  m.def("QLi", &QLiVec);
  m.def("QLiNeg", &QLiNegVec);
  m.def("QLiSymm", &QLiSymmVec);

  m.def("QLiPr", &QLiVecPr);
  m.def("QLiNegPr", &QLiNegVecPr);
  m.def("QLiSymmPr", &QLiSymmVecPr);
}
