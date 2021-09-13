#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"

#include "cpp/lib/polylog_space.h"


namespace py = pybind11;

template<typename T>
T ptr_to_lyndon_basis(const std::shared_ptr<T>& ptr) {
  return to_lyndon_basis(*ptr);
}


auto polylog_space_matrix_l_vs_m(int weight, const XArgs& points) {
  return compute_polylog_space_matrices(
    L(weight, points),
    XCoords(weight, points),
    DISAMBIGUATE(ptr_to_lyndon_basis)
  );
}

void pybind_polylog_space(py::module_& m) {
  m.def("polylog_space_matrix", &polylog_space_matrix);
  m.def("polylog_space_matrix_6_via_l", &polylog_space_matrix_6_via_l);
  m.def("polylog_space_matrix_l_vs_m", &polylog_space_matrix_l_vs_m);
}
