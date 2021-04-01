#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/iterated_integral.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_qli.h"


namespace py = pybind11;

void pybind_polylog(py::module_& m) {
  m.def("IVec", &IVec);
  m.def("CorrVec", &CorrVec);

  m.def("QLi", &QLiVec);
  m.def("QLiNeg", &QLiNegVec);
  m.def("QLiSymm", &QLiSymmVec);

  m.def("QLiPr", &QLiVecPr);
  m.def("QLiNegPr", &QLiNegVecPr);
  m.def("QLiSymmPr", &QLiSymmVecPr);

  // limitation: monsters are not supported
  m.def("Lira", [](int foreweight, const std::vector<int>& weights, const std::vector<CompoundRatio>& ratios) {
    return theta_expr_to_delta_expr(LiraVec(foreweight, weights, ratios));
  });
}
