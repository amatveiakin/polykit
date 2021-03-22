#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_arithmetic.h"
#include "lib/lazy/lazy_delta.h"
#include "py_util.h"


namespace py = pybind11;

using PyDeltaExpr = PyLinear<LazyDeltaExpr>;
using PyDeltaCoExpr = PyLinear<LazyDeltaCoExpr>;

PyDeltaExpr eval_lazy_delta(const LazyDeltaExpr& expr) {
  return eval_lazy_expr(expr, [](const std::vector<Delta>& term) {
    return py::tuple(py::cast(term));
  });
}

PyDeltaCoExpr eval_lazy_codelta(const LazyDeltaCoExpr& expr) {
  return eval_lazy_expr(expr, [](const std::vector<std::vector<Delta>>& term) {
    return py::tuple(py::cast(mapped(term, [](const auto& subterm) {
      return py::tuple(py::cast(subterm));
    })));
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
  py_register_py_expr<PyDeltaExpr>(m, "PyDeltaExpr");
  m.def("eval_lazy_delta", &eval_lazy_delta, "Evaluates a LazyDeltaExpr");

  py_register_lazy_expr<LazyDeltaCoExpr>(m, "LazyDeltaCoExpr");
  py_register_py_expr<PyDeltaCoExpr>(m, "PyDeltaCoExpr");
  m.def("eval_lazy_codelta", &eval_lazy_codelta, "Evaluates a LazyDeltaCoExpr");

  // TODO: Test&Fix: pybind probably can't result function overloads across modules
  m.def("substitute_variables", &substitute_variables_lazy, "Substitutes variable into a LazyDeltaExpr; can substitute Inf");
  m.def("involute", &involute, "Eliminates terms (x5-x6), (x4-x6), (x2-x6) using involution x1<->x4, x2<->x5, x3<->x6");

  m.def("coproduct", &coproduct_lazy);
  m.def("comultiply", &comultiply_lazy);
}
