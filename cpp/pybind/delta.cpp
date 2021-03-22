#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_arithmetic.h"
#include "lib/lazy/lazy_delta.h"
#include "py_util.h"


namespace py = pybind11;

using PyDeltaExpr = PyLinear<LazyDeltaExpr>;

PyDeltaExpr eval_lazy_delta(const LazyDeltaExpr& expr) {
  return eval_lazy_expr(expr, [](const std::vector<Delta>& term) {
    return py::tuple(py::cast(term));
  });
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

  py_register_lazy_expr<LazyDeltaExpr>(m, "LazyDeltaExpr");

  py::class_<PyDeltaExpr>(m, "PyDeltaExpr")
    .def_readonly("data", &PyDeltaExpr::data)
  ;

  m.def("eval_lazy_delta", &eval_lazy_delta, "Evaluates a LazyDeltaExpr");
}