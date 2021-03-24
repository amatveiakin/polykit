#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/ratio.h"


namespace py = pybind11;

PYBIND11_MODULE(ratio, m) {
  constexpr auto kNormalization = CrossRatioNormalization::rotation_only;

  py::class_<CrossRatio>(m, "CrossRatio")
    .def(py::init<>())
    .def(py::init<std::array<int, kCrossRatioElements>>())
    .def_static("one_minus", &CrossRatio::one_minus)
    .def_static("inverse", &CrossRatio::inverse)
    .def("__getitem__", &CrossRatio::at)
    .def("indices", &CrossRatio::indices)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def(py::self <  py::self)
    .def(py::self <= py::self)
    .def(py::self >  py::self)
    .def(py::self >= py::self)
    .def("__hash__", absl::Hash<CrossRatio>())
    .def("__str__", py::overload_cast<const CrossRatio&>(&to_string<kNormalization>))
    .def("__repr__", py::overload_cast<const CrossRatio&>(&to_string<kNormalization>))
  ;

  py::class_<CompoundRatio>(m, "CompoundRatio")
    .def(py::init<>())
    .def(py::init<CrossRatio>())
    .def_static("one_minus", &CompoundRatio::one_minus)
    .def("is_unity", &CompoundRatio::is_unity)
    .def("loops", &CompoundRatio::loops)
    .def(py::self * py::self)
    .def(py::self *= py::self)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def(py::self <  py::self)
    .def(py::self <= py::self)
    .def(py::self >  py::self)
    .def(py::self >= py::self)
    .def("__hash__", absl::Hash<CompoundRatio>())
    .def("__str__", [](const CompoundRatio& ratio) { return to_string(ratio); })
    .def("__repr__", [](const CompoundRatio& ratio) { return to_string(ratio); })
  ;

  py::implicitly_convertible<CrossRatio, CompoundRatio>();
}
