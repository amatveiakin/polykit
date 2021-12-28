#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "cpp/lib/x.h"


namespace py = pybind11;

void pybind_x(py::module_& m) {
  py::enum_<XForm>(m, "XForm")
    .value("var", XForm::var)
    .value("neg_var", XForm::neg_var)
    .value("sq_var", XForm::sq_var)
    .value("zero", XForm::zero)
    .value("infinity", XForm::infinity)
    .value("undefined", XForm::undefined)
  ;

  py::class_<X>(m, "X")
    .def(py::init<>())
    .def(py::init<XForm, int>())
    .def(py::init<int>())
    .def_property_readonly("form", &X::form)
    .def_property_readonly("idx", &X::idx)
    .def("is", &X::is)
    .def("is_constant", &X::is_constant)
    .def("negated", &X::negated)
    .def(+py::self)
    .def(-py::self)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def(py::self <  py::self)
    .def(py::self <= py::self)
    .def(py::self >  py::self)
    .def(py::self >= py::self)
    .def("__hash__", absl::Hash<X>())
    .def("__str__", py::overload_cast<X>(&to_string))
    .def("__repr__", py::overload_cast<X>(&to_string))
  ;

  m.attr("Inf") = Inf;
  m.attr("Zero") = Zero;

  m.attr("x1") = x1;
  m.attr("x2") = x2;
  m.attr("x3") = x3;
  m.attr("x4") = x4;
  m.attr("x5") = x5;
  m.attr("x6") = x6;
  m.attr("x7") = x7;
  m.attr("x8") = x8;
  m.attr("x9") = x9;
  m.attr("x10") = x10;
  m.attr("x11") = x11;
  m.attr("x12") = x12;
  m.attr("x13") = x13;
  m.attr("x14") = x14;
  m.attr("x15") = x15;
  m.attr("x16") = x16;

  m.attr("x1s") = x1s;
  m.attr("x2s") = x2s;
  m.attr("x3s") = x3s;
  m.attr("x4s") = x4s;
  m.attr("x5s") = x5s;
  m.attr("x6s") = x6s;
  m.attr("x7s") = x7s;
  m.attr("x8s") = x8s;
  m.attr("x9s") = x9s;
  m.attr("x10s") = x10s;
  m.attr("x11s") = x11s;
  m.attr("x12s") = x12s;
  m.attr("x13s") = x13s;
  m.attr("x14s") = x14s;
  m.attr("x15s") = x15s;
  m.attr("x16s") = x16s;

  py::class_<XArgs>(m, "XArgs")
    .def(py::init<std::vector<int>>())
    .def(py::init<std::vector<X>>())
  ;

  py::implicitly_convertible<int, X>();

  py::implicitly_convertible<std::vector<int>, XArgs>();
  py::implicitly_convertible<std::vector<X>, XArgs>();
}
