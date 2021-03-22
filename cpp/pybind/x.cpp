#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

#include "lib/x.h"


namespace py = pybind11;


PYBIND11_MODULE(x, m) {
  py::class_<X>(m, "X")
    .def(py::init<int>())
    .def(py::init<X>())
    .def_property_readonly("var", &X::var)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def(py::self <  py::self)
    .def(py::self <= py::self)
    .def(py::self >  py::self)
    .def(py::self >= py::self)
    .def("__str__", py::overload_cast<const X&>(&to_string))
    .def("__repr__", py::overload_cast<const X&>(&to_string))
  ;

  py::implicitly_convertible<int, X>();

  m.attr("Inf") = Inf;
}