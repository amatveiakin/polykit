#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

#include "cpp/lib/polylog_space.h"


namespace py = pybind11;

void pybind_polylog_space(py::module_& m) {
  m.def("polylog_space_matrix", &polylog_space_matrix);
  m.def("polylog_space_matrix_6_via_l", &polylog_space_matrix_6_via_l);
}
