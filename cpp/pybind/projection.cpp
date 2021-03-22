#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/lazy/lazy_arithmetic.h"
#include "lib/lazy/lazy_projection.h"
#include "py_util.h"


namespace py = pybind11;

using PyProjectionExpr = PyLinear<LazyProjectionExpr>;

PyProjectionExpr eval_lazy_projection(const LazyProjectionExpr& expr) {
  return eval_lazy_expr(expr, [](const std::vector<int>& term) {
    return py::tuple(py::cast(term));
  });
}


PYBIND11_MODULE(projection, m) {
  py_register_lazy_expr<LazyProjectionExpr>(m, "LazyProjectionExpr");
  py_register_py_expr<PyProjectionExpr>(m, "PyProjectionExpr");
  m.def("eval_lazy_projection", &eval_lazy_projection, "Evaluates a LazyProjectionExpr");

  m.def("project_on", &project_on_lazy, "Projects a LazyDeltaExpr onto an axis");
}
