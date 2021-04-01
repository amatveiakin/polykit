#pragma once

#include <sstream>

#include "pybind11/functional.h"
#include "pybind11/operators.h"
#include "pybind11/pybind11.h"


template<typename LinearT>
class LinearIterableRange {
public:
  LinearIterableRange(const LinearT& expr)
    : current_(expr.begin()), end_(expr.end()) {}

  LinearIterableRange iter() const {
    return *this;
  }
  auto next() {
    if (current_ == end_) {
      throw pybind11::stop_iteration();
    }
    auto ret = *current_;
    ++current_;
    return ret;
  }

private:
  using const_iterator = typename LinearT::const_iterator;
  const_iterator current_;
  const_iterator end_;
};


template<typename LinearT>
void py_register_linear(const pybind11::module_& module, const char* name) {
  namespace py = pybind11;

  using Iterable = LinearIterableRange<LinearT>;
  std::string iterator_class_name = absl::StrCat(name, "Iterator");
  py::class_<Iterable>(module, iterator_class_name.c_str())
    .def("__iter__", &Iterable::iter)
    .def("__next__", &Iterable::next)
  ;

  // TODO: Allow to customize Python ObjectT and use tuples instead of lists
  //   (need to update all high-order functions and `single`)
  using ObjectT = typename LinearT::ObjectT;
  py::class_<LinearT>(module, name)
    .def(py::init<>())
    .def_static("single", &LinearT::single)
    .def("is_zero", &LinearT::is_zero)
    .def("is_blank", &LinearT::is_blank)
    .def("size", &LinearT::size)
    .def("l1_norm", &LinearT::l1_norm)
    .def("__iter__", [](const LinearT& self) { return Iterable(self); })
    .def("__getitem__", &LinearT::operator[])
    .def("add_to", &LinearT::add_to)
    .def("element", &LinearT::element)
    .def("pop", &LinearT::pop)
    // TODO: Expose iterable interface instead
    .def("foreach", [](const LinearT& self, const std::function<void(ObjectT, int)>& func) {
      self.foreach(func);
    })
    // limitation: `mapped` can return only linear of the same type
    .def("mapped", [](const LinearT& self, const std::function<ObjectT(ObjectT)>& func) {
      return self.mapped(func);
    })
    // limitation: `mapped_expanding` can return only linear of the same type
    .def("mapped_expanding", [](const LinearT& self, const std::function<LinearT(ObjectT)>& func) {
      return self.mapped_expanding(func);
    })
    .def("filtered", [](const LinearT& self, const std::function<bool(ObjectT)>& func) {
      return self.filtered(func);
    })
    .def("termwise_abs", &LinearT::termwise_abs)
    .def("annotate", &LinearT::annotate)
    .def("without_annotations", &LinearT::without_annotations)
    .def(+py::self)
    .def(-py::self)
    .def(py::self + py::self)
    .def(py::self - py::self)
    .def(py::self * int())
    .def(int() * py::self)
    .def("dived_int", &LinearT::dived_int)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
    .def(py::self += py::self)
    .def(py::self -= py::self)
#pragma clang diagnostic pop
    .def(py::self *= int())
    .def("div_int", &LinearT::div_int)
    .def(py::self == py::self)
    .def(py::self != py::self)
    .def("__str__", [](const LinearT& expr) {
      // Python `print` adds a trailing newline automatically.
      // TODO: Debug why `set_new_line_after_expression` has no effect.
      ScopedFormatting sf(FormattingConfig().set_new_line_after_expression(false));
      std::stringstream ss;
      ss << expr;
      return ss.str();
    })
  ;
}
