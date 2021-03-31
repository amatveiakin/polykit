#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/polylog_qli.h"


namespace py = pybind11;

PYBIND11_MODULE(polylog_qli, m) {
  // TODO: Add Python binding for SpanX instead
  m.def("QLi", [](int weight, const std::vector<X>& points) { return QLiVec(weight, points); });
  m.def("QLiNeg", [](int weight, const std::vector<X>& points) { return QLiNegVec(weight, points); });
  m.def("QLiSymm", [](int weight, const std::vector<X>& points) { return QLiSymmVec(weight, points); });

  m.def("QLiPr", [](int weight, const std::vector<X>& points, DeltaProjector projector) { return QLiVecPr(weight, points, projector); });
  m.def("QLiNegPr", [](int weight, const std::vector<X>& points, DeltaProjector projector) { return QLiNegVecPr(weight, points, projector); });
  m.def("QLiSymmPr", [](int weight, const std::vector<X>& points, DeltaProjector projector) { return QLiSymmVecPr(weight, points, projector); });
}
