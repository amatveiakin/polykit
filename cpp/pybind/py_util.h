#pragma once

#include <sstream>

#include "pybind11/operators.h"
#include "pybind11/pybind11.h"


template<typename LinearT>
void py_register_linear(const pybind11::module_& module, const char* name) {
  namespace py = pybind11;
  // TODO: Expose more functions
  py::class_<LinearT>(module, name)
    .def(+py::self)
    .def(-py::self)
    .def(py::self + py::self)
    .def(py::self - py::self)
    .def(py::self * int())
    .def(int() * py::self)
    .def("is_zero", &LinearT::is_zero)
    .def("size", &LinearT::size)
    .def("l1_norm", &LinearT::l1_norm)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def("__str__", [](const LinearT& expr) { std::stringstream ss; ss << expr; return ss.str(); })
  ;
}
