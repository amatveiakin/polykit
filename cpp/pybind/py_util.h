#pragma once

#include "pybind11/operators.h"
#include "pybind11/pybind11.h"


// TODO: Add annotations.
template<typename LazyT>
struct PyLinear {
  pybind11::list data;  // list of pairs: (<object_type>, int)
};

template<typename LazyT, typename F>
auto eval_lazy_expr(const LazyT& expr, const F& obj_to_py) {
  namespace py = pybind11;
  py::list data;
  expr.evaluate().foreach([&](const auto& obj, int coeff) {
    py::tuple obj_coeff = py::cast(std::pair{obj_to_py(obj), coeff});
    data.append(obj_coeff);
  });
  return PyLinear<LazyT>{data};
}

template<typename LazyT>
void py_register_lazy_expr(const pybind11::module_& module, const char* name) {
  namespace py = pybind11;
  py::class_<LazyT>(module, name)
    .def("description", &LazyT::description)
    .def(+py::self)
    .def(-py::self)
    .def(py::self + py::self)
    .def(py::self - py::self)
    .def(py::self * int())
    .def(int() * py::self)
  ;
}

template<typename PyT>
void py_register_py_expr(const pybind11::module_& module, const char* name) {
  namespace py = pybind11;
  py::class_<PyT>(module, name)
    .def_readonly("data", &PyT::data)
  ;
}
