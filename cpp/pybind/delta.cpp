#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_arithmetic.h"
#include "lib/lazy/lazy_delta.h"


namespace py = pybind11;

// TODO: Add annotations.
struct PyDeltaExpr {
  using ObjectT = std::vector<Delta>;
  py::list data;  // list of pairs: (ObjectT, int)
};

PyDeltaExpr eval_lazy_delta(const LazyDeltaExpr& expr) {
  py::list data;
  expr.evaluate().main().foreach([&](const auto& key, int coeff) {
    py::tuple obj = py::cast(key);
    py::tuple obj_coeff = py::cast(std::pair{obj, coeff});
    data.append(obj_coeff);
  });
  return PyDeltaExpr{data};
}


PYBIND11_MODULE(delta, m) {
  py::class_<Delta>(m, "Delta")
    // .def(py::init<X, X>())  // TODO:
    .def(py::init<int, int>())
    // TODO: Make `a` and `b` read-only attributes (def_property + py::cpp_function(&MyClass::getData))
    .def("a", &Delta::a)
    .def("b", &Delta::b)
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

  py::class_<LazyDeltaExpr>(m, "LazyDeltaExpr")
    .def("description", &LazyDeltaExpr::description)
    .def(+py::self)
    .def(-py::self)
    .def(py::self + py::self)
    .def(py::self - py::self)
    .def(py::self * int())
    .def(int() * py::self)
  ;

  py::class_<PyDeltaExpr>(m, "PyDeltaExpr")
    .def_readonly("data", &PyDeltaExpr::data)
  ;

  m.def("eval_lazy_delta", &eval_lazy_delta, "Evaluates a LazyDeltaExpr");
}
