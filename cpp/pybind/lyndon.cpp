#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

#include "lib/lazy/lazy_delta.h"
#include "lib/lazy/lazy_lyndon.h"


namespace py = pybind11;


PYBIND11_MODULE(lyndon, m) {
  const char kLazyDescription[] = "Converts an expression to Lyndon basis";
  m.def("to_lyndon_basis", &to_lyndon_basis_lazy<DeltaExpr>, kLazyDescription);
}
