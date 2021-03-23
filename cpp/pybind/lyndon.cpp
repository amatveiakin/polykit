#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

#include "lib/delta.h"
#include "lib/lyndon.h"
#include "lib/projection.h"


namespace py = pybind11;

PYBIND11_MODULE(lyndon, m) {
  const char kLyndonDescription[] = "Converts an expression to Lyndon basis";
  m.def("to_lyndon_basis", &to_lyndon_basis<DeltaExpr>, kLyndonDescription);
  m.def("to_lyndon_basis", &to_lyndon_basis<ProjectionExpr>, kLyndonDescription);
}
