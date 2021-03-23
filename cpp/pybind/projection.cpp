#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/projection.h"
#include "py_util.h"


namespace py = pybind11;

PYBIND11_MODULE(projection, m) {
  py_register_linear<ProjectionExpr>(m, "ProjectionExpr");

  m.def("project_on", &project_on, "Projects a DeltaExpr onto an axis");
  m.def("involute_projected", &involute_projected);
}
