#include <future>

#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

#include "cpp/lib/expr_matrix_builder.h"
#include "cpp/lib/polylog_cluster.h"


namespace py = pybind11;

auto cluster_space_by_weight(int weight) {
  switch (weight) {
    case 2: return &cluster_weight2;
    case 3: return &cluster_weight3;
    case 4: return &cluster_weight4;
    case 5: return &cluster_weight5;
    case 6: return &cluster_weight6_alt;
  }
  FATAL(absl::StrCat("Unsupported weight in cluster_space_by_weight: ", weight));
}

Eigen::MatrixXd cluster_polylog_matrix(int weight, const XArgs& points, bool apply_comult) {
  ExprMatrixBuilder<DeltaNCoExpr> matrix_builder;
  std::vector<std::future<DeltaNCoExpr>> results;
  for (const auto& s : cluster_space_by_weight(weight)(points)) {
    results.push_back(std::async([s, apply_comult]() {
      const auto& [s1, s2] = s;
      const auto prod = ncoproduct(*s1, *s2);
      return apply_comult ? ncomultiply(prod) : prod;
    }));
  }
  for (auto& result : results) {
    matrix_builder.add_expr(result.get());
  }
  return matrix_builder.make_matrix<double>();
}

void pybind_polylog_cluster(py::module_& m) {
  m.def("cluster_polylog_matrix", &cluster_polylog_matrix);
}
