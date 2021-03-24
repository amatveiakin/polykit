#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/iterated_integral.h"


namespace py = pybind11;

PYBIND11_MODULE(iterated_integral, m) {
  // TODO: Add Python binding for SpanX instead
  m.def("IVec", [](const std::vector<X>& points) { return IVec(points); });
  m.def("CorrVec", [](const std::vector<X>& points) { return CorrVec(points); });
}
