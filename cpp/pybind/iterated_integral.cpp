#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/iterated_integral.h"


namespace py = pybind11;

PYBIND11_MODULE(iterated_integral, m) {
  m.def("IVec", &IVec);
  m.def("CorrVec", &CorrVec);
}
