#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/delta.h"
#include "py_util.h"


namespace py = pybind11;

PYBIND11_MODULE(delta, m) {
  py::class_<Delta>(m, "Delta")
    .def(py::init<X, X>())
    .def_property_readonly("a", &Delta::a)
    .def_property_readonly("b", &Delta::b)
    .def("is_nil", &Delta::is_nil)
    .def("contains", &Delta::contains)
    .def("other_point", &Delta::other_point)
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

  m.def("substitute_variables", &substitute_variables, "Substitutes variable into a DeltaExpr; can substitute Inf");
  m.def("involute", &involute, "Eliminates terms (x5-x6), (x4-x6), (x2-x6) using involution x1<->x4, x2<->x5, x3<->x6");

  m.def("terms_with_num_distinct_variables", &terms_with_num_distinct_variables);
  m.def("terms_with_min_distinct_variables", &terms_with_min_distinct_variables);
  m.def("terms_containing_only_variables", &terms_containing_only_variables);
  m.def("terms_without_variables", &terms_without_variables);

  m.def("coproduct", &coproduct<DeltaCoExpr, DeltaExpr>);
  m.def("comultiply", &comultiply<DeltaCoExpr, DeltaExpr>);
}
