#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "cpp/lib/iterated_integral.h"
#include "cpp/lib/polylog_lira.h"
#include "cpp/lib/polylog_qli.h"


namespace py = pybind11;

void pybind_polylog(py::module_& m) {
  m.def("IVec", &IVec);
  m.def("CorrVec", &CorrVec);

  m.def("Log", &LogVec);

  m.def("QLi", &QLiVec);
  m.def("QLiNeg", &QLiNegVec);
  m.def("QLiSymm", &QLiSymmVec);

  m.def("QLiPr", &QLiVecPr);
  m.def("QLiNegPr", &QLiNegVecPr);
  m.def("QLiSymmPr", &QLiSymmVecPr);

  m.def("A2", &A2Vec);

  // limitation: monsters are not supported
  m.def("Lira", [](int foreweight, const std::vector<int>& weights, const std::vector<CompoundRatio>& ratios) {
    return theta_expr_to_delta_expr(LiraVec(foreweight, weights, ratios));
  });
}
