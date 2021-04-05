#include "pybind11/pybind11.h"


namespace py = pybind11;

void pybind_expressions(py::module_&);
void pybind_format(py::module_&);
void pybind_loops(py::module_&);
void pybind_polylog(py::module_&);
void pybind_ratio(py::module_&);
void pybind_x(py::module_&);

PYBIND11_MODULE(pylib, m) {
  pybind_expressions(m);
  pybind_format(m);
  pybind_loops(m);
  pybind_polylog(m);
  pybind_ratio(m);
  pybind_x(m);
}
