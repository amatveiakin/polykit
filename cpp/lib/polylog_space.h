#pragma once

#include "delta.h"
#include "expr_matrix_builder.h"
#include "gamma.h"
#include "linalg.h"
#include "parallel_util.h"
#include "profiler.h"
#include "x.h"


using PolylogSpace = std::vector<DeltaExpr>;
using PolylogNCoSpace = std::vector<DeltaNCoExpr>;
using PolylogSpacePair = std::vector<std::pair<DeltaExpr, DeltaExpr>>;
using GrPolylogSpace = std::vector<GammaExpr>;
using GrPolylogNCoSpace = std::vector<GammaNCoExpr>;

std::string dump_to_string_impl(const PolylogSpace& space);
std::string dump_to_string_impl(const PolylogNCoSpace& space);
std::string dump_to_string_impl(const GrPolylogSpace& space);
std::string dump_to_string_impl(const GrPolylogNCoSpace& space);

PolylogSpace QL(int weight, const XArgs& xargs);

PolylogSpace CB1(const XArgs& xargs);
PolylogSpace CB2(const XArgs& xargs);
PolylogSpace CB3(const XArgs& xargs);
PolylogSpace CB4(const XArgs& xargs);
PolylogSpace CB5(const XArgs& xargs);

PolylogSpace CL4(const XArgs& xargs);
// TODO: Keep only one version
PolylogSpace CL5(const XArgs& xargs);
PolylogSpace CL5Alt(const XArgs& xargs);

PolylogSpace H(int weight, const XArgs& xargs);

PolylogSpace LInf(int weight, const XArgs& xargs);
PolylogSpace L(int weight, const XArgs& xargs);

PolylogSpace XCoords(int weight, const XArgs& args);
PolylogSpace ACoords(int weight, const XArgs& args);
PolylogSpace ACoordsHopf(int weight, const XArgs& args);

// TODO: Give more specific names to these spaces and functions for their ranks.
PolylogSpacePair polylog_space_2(const XArgs& args);
PolylogSpacePair polylog_space_3(const XArgs& args);
PolylogSpacePair polylog_space_4(const XArgs& args);
PolylogSpacePair polylog_space_5(const XArgs& args);
PolylogSpacePair polylog_space_6(const XArgs& args);
PolylogSpacePair polylog_space_6_alt(const XArgs& args);
PolylogSpacePair polylog_space_6_via_l(const XArgs& args);

inline auto polylog_space(int weight) {
  switch (weight) {
    case 3: return &polylog_space_3;
    case 4: return &polylog_space_4;
    case 5: return &polylog_space_5;
    case 6: return &polylog_space_6;
  }
  FATAL(absl::StrCat("Unsupported weight in polylog_space_by_weight: ", weight));
}

Matrix polylog_space_matrix(int weight, const XArgs& points, bool apply_comult);
Matrix polylog_space_matrix_6_via_l(const XArgs& points, bool apply_comult);

PolylogNCoSpace polylog_space_ql_wedge_ql(int weight, const XArgs& xargs);

GrPolylogSpace GrLBasic(int weight, const XArgs& xargs);  // dimension = 3
GrPolylogSpace GrL1(int dimension, const XArgs& xargs);
GrPolylogSpace GrL2(int dimension, const XArgs& xargs);
GrPolylogSpace GrL3(int dimension, const XArgs& xargs);


template<typename SpaceT, typename PrepareF, typename MatrixBuilderT>
void add_polylog_space_to_matrix_builder(const SpaceT& space, const PrepareF& prepare, MatrixBuilderT& matrix_builder) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  static_assert(std::is_same_v<MatrixBuilderT, ExprMatrixBuilder<ExprT>>);
  const auto space_prepared = mapped_parallel(space, [prepare](const auto& s) {
    return prepare(s);
  });
  for (const auto& expr : space_prepared) {
    matrix_builder.add_expr(expr);
  }
}

template<typename SpaceT, typename PrepareF>
Matrix compute_polylog_space_matrix(const SpaceT& space, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  ExprMatrixBuilder<ExprT> matrix_builder;
  add_polylog_space_to_matrix_builder(space, prepare, matrix_builder);
  return matrix_builder.make_matrix();
}

template<typename SpaceT, typename PrepareF>
int compute_polylog_space_dim(const SpaceT& space, const PrepareF& prepare) {
  return matrix_rank(compute_polylog_space_matrix(space, prepare));
}

// TODO: Version when `needle` is a single function
template<typename SpaceT, typename PrepareF>
bool polylog_space_contains(const SpaceT& haystack, const SpaceT& needle, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  ExprMatrixBuilder<ExprT> matrix_builder;
  add_polylog_space_to_matrix_builder(haystack, prepare, matrix_builder);
  const int haystack_rank = matrix_rank(matrix_builder.make_matrix());
  add_polylog_space_to_matrix_builder(needle, prepare, matrix_builder);
  const int united_rank = matrix_rank(matrix_builder.make_matrix());
  CHECK_LE(haystack_rank, united_rank);
  return united_rank == haystack_rank;
}


struct PolylogSpaceDimensions {
  int a;
  int b;
  int united;
};

template<typename SpaceT, typename PrepareF>
PolylogSpaceDimensions compute_polylog_space_dimensions(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  Profiler profiler(false);

  ExprMatrixBuilder<ExprT> matrix_builder_a;
  add_polylog_space_to_matrix_builder(a, prepare, matrix_builder_a);
  profiler.finish("A space");
  const int a_rank = matrix_rank(matrix_builder_a.make_matrix());
  profiler.finish("A dim");
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_a);
  profiler.finish("united space");
  const int united_rank = matrix_rank(matrix_builder_a.make_matrix());
  profiler.finish("united dim");

  ExprMatrixBuilder<ExprT> matrix_builder_b;
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_b);
  profiler.finish("B space");
  const int b_rank = matrix_rank(matrix_builder_b.make_matrix());
  profiler.finish("B dim");

  CHECK_LE(a_rank, united_rank);
  CHECK_LE(b_rank, united_rank);
  return PolylogSpaceDimensions{a_rank, b_rank, united_rank};
}

template<typename SpaceT, typename PrepareF>
bool polylog_space_equals(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto dim = compute_polylog_space_dimensions(a, b, prepare);
  return all_equal(absl::MakeConstSpan({dim.a, dim.b, dim.united}));
}

template<typename SpaceT, typename PrepareF>
int polylog_space_intersection_dimension(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto dim = compute_polylog_space_dimensions(a, b, prepare);
  return dim.a + dim.b - dim.united;
}

template<typename SpaceT, typename PrepareF>
std::string polylog_spaces_intersection_describe(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  const auto dim = compute_polylog_space_dimensions(a, b, prepare);
  const int intersection = dim.a + dim.b - dim.united;
  // TODO: Use `fmt` for special characters.
  return absl::StrCat("(", dim.a, ", ", dim.b, "), ∩ = ", intersection);
}

template<typename SpaceT, typename PrepareF, typename MapF>
std::string polylog_space_kernel_describe(const SpaceT& space, const PrepareF& prepare, const MapF& map) {
  Profiler profiler(false);
  const auto whole_space = compute_polylog_space_matrix(space, prepare);
  profiler.finish("whole space");
  const int whole_dim = matrix_rank(whole_space);
  profiler.finish("whole dim");
  const auto image_space = compute_polylog_space_matrix(space, map);
  profiler.finish("image space");
  const int image_dim = matrix_rank(image_space);
  profiler.finish("image dim");
  const int kernel_dim = whole_dim - image_dim;
  return absl::StrCat(whole_dim, " - ", image_dim, " = ", kernel_dim);
}

inline std::string polylog_space_ncomultiply_kernel_describe(const PolylogNCoSpace& space) {
  return polylog_space_kernel_describe(space, DISAMBIGUATE(identity_function), DISAMBIGUATE(ncomultiply));
}


// For pybind11. Returns matrices: (a, b, united)
template<typename SpaceT, typename PrepareF>
std::tuple<Matrix, Matrix, Matrix> compute_polylog_space_matrices(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;

  ExprMatrixBuilder<ExprT> matrix_builder_a;
  add_polylog_space_to_matrix_builder(a, prepare, matrix_builder_a);
  Matrix mat_a = matrix_builder_a.make_matrix();
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_a);
  Matrix mat_united = matrix_builder_a.make_matrix();

  ExprMatrixBuilder<ExprT> matrix_builder_b;
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_b);
  Matrix mat_b = matrix_builder_b.make_matrix();

  return {std::move(mat_a), std::move(mat_b), std::move(mat_united)};
}
