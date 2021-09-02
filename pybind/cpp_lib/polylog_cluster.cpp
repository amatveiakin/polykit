#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

#include "cpp/lib/polylog_cluster.h"


namespace py = pybind11;

void pybind_polylog_cluster(py::module_& m) {
  m.def("cluster_space_matrix", &cluster_space_matrix);
}
