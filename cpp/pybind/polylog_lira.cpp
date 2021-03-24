#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/polylog_lira.h"


namespace py = pybind11;


PYBIND11_MODULE(polylog_lira, m) {
  // limitation: monsters are not supported
  m.def("Lira", [](int foreweight, const std::vector<int>& weights, const std::vector<CompoundRatio>& ratios) {
    return theta_expr_to_delta_expr(LiraVec(foreweight, weights, ratios));
  });
}
