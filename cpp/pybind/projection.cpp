#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/projection.h"
#include "py_util.h"


namespace py = pybind11;

// TODO: Move out of `projection` module
void print_sorted_by_num_distinct_variables_overloaded(std::ostream& os, const std::variant<DeltaExpr, ProjectionExpr>& expr) {
  return std::visit([&](const auto& concrete_expr) {
    print_sorted_by_num_distinct_variables(os, concrete_expr);
  }, expr);
}


PYBIND11_MODULE(projection, m) {
  py_register_linear<ProjectionExpr>(m, "ProjectionExpr");

  m.def("project_on", &project_on, "Projects a DeltaExpr onto an axis");
  m.def("involute_projected", &involute_projected);

  m.def("sorted_by_num_distinct_variables", [](const std::variant<DeltaExpr, ProjectionExpr>& expr) {
    std::stringstream ss;
    print_sorted_by_num_distinct_variables_overloaded(ss, expr);
    return ss.str();
  });
}
