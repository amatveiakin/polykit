#pragma once

#include <future>

#include "delta.h"
#include "expr_matrix_builder.h"
#include "linalg.h"
#include "ptr_util.h"
#include "x.h"


using PolylogSpace = std::vector<std::shared_ptr<const DeltaExpr>>;
using PolylogCoSpace = std::vector<std::pair<std::shared_ptr<const DeltaExpr>, std::shared_ptr<const DeltaExpr>>>;

PolylogSpace CB1(const XArgs& xargs);
PolylogSpace CB2(const XArgs& xargs);
PolylogSpace CB3(const XArgs& xargs);
PolylogSpace CB4(const XArgs& xargs);
PolylogSpace CB5(const XArgs& xargs);

PolylogSpace CL4(const XArgs& xargs);
// TODO: Keep only one version
PolylogSpace CL5(const XArgs& xargs);
PolylogSpace CL5Alt(const XArgs& xargs);

PolylogSpace L(int weight, const XArgs& xargs);
PolylogSpace L3(const XArgs& xargs);
PolylogSpace L4(const XArgs& xargs);

PolylogCoSpace polylog_space_2(const XArgs& args);
PolylogCoSpace polylog_space_3(const XArgs& args);
PolylogCoSpace polylog_space_4(const XArgs& args);
PolylogCoSpace polylog_space_5(const XArgs& args);
PolylogCoSpace polylog_space_6(const XArgs& args);
PolylogCoSpace polylog_space_6_alt(const XArgs& args);
PolylogCoSpace polylog_space_6_via_l(const XArgs& args);

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


template<typename SpaceT, typename PrepareF, typename MatrixBuilderT>
void add_polylog_space_to_matrix_builder(const SpaceT& space, const PrepareF& prepare, MatrixBuilderT& matrix_builder) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  static_assert(std::is_same_v<MatrixBuilderT, ExprMatrixBuilder<ExprT>>);
  std::vector<std::future<ExprT>> results;
  for (const auto& s : space) {
    results.push_back(std::async([prepare, s]() {
      return prepare(s);
    }));
  }
  for (auto& result : results) {
    matrix_builder.add_expr(result.get());
  }
}

template<typename SpaceT, typename PrepareF>
Matrix compute_polylog_space_matrix(const SpaceT& space, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  ExprMatrixBuilder<ExprT> matrix_builder;
  add_polylog_space_to_matrix_builder(space, prepare, matrix_builder);
  return matrix_builder.make_matrix();
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

template<typename SpaceT, typename PrepareF>
bool polylog_space_equals(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;

  ExprMatrixBuilder<ExprT> matrix_builder_a;
  add_polylog_space_to_matrix_builder(a, prepare, matrix_builder_a);
  const int a_rank = matrix_rank(matrix_builder_a.make_matrix());
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_a);
  const int united_rank = matrix_rank(matrix_builder_a.make_matrix());

  ExprMatrixBuilder<ExprT> matrix_builder_b;
  add_polylog_space_to_matrix_builder(b, prepare, matrix_builder_b);
  const int b_rank = matrix_rank(matrix_builder_b.make_matrix());

  CHECK_LE(a_rank, united_rank);
  CHECK_LE(b_rank, united_rank);
  return all_equal(absl::MakeConstSpan({a_rank, b_rank, united_rank}));
}
