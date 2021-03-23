#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/delta.h"
#include "py_util.h"


namespace py = pybind11;

// TODO: Add Python binding for SpanX instead
DeltaExpr substitute_variables_wrapper(const DeltaExpr& expr, const std::vector<X>& new_points) {
  return substitute_variables(expr, new_points);
}

PYBIND11_MODULE(delta, m) {
  py::class_<Delta>(m, "Delta")
    .def(py::init<X, X>())
    .def_property_readonly("a", &Delta::a)
    .def_property_readonly("b", &Delta::b)
    .def("is_nil", &Delta::is_nil)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def(py::self <  py::self)
    .def(py::self <= py::self)
    .def(py::self >  py::self)
    .def(py::self >= py::self)
    .def("__hash__", absl::Hash<Delta>())
    .def("__str__", py::overload_cast<const Delta&>(&to_string))
    .def("__repr__", py::overload_cast<const Delta&>(&to_string))
  ;

  py_register_linear<DeltaExpr>(m, "DeltaExpr");
  py_register_linear<DeltaCoExpr>(m, "DeltaCoExpr");

  // TODO: Test&Fix: pybind probably can't result function overloads across modules
  m.def("substitute_variables", &substitute_variables_wrapper, "Substitutes variable into a DeltaExpr; can substitute Inf");
  m.def("involute", &involute, "Eliminates terms (x5-x6), (x4-x6), (x2-x6) using involution x1<->x4, x2<->x5, x3<->x6");

  m.def("coproduct", &coproduct<DeltaCoExpr, DeltaExpr>);
  m.def("comultiply", &comultiply<DeltaCoExpr, DeltaExpr>);
}
