#pragma once

#include "expr_matrix_builder.h"
#include "linalg.h"
#include "parallel_util.h"
#include "profiler.h"
#include "vector_space.h"


class SpaceVennRanks {
public:
  SpaceVennRanks(int a, int b, int united) : a_(a), b_(b), united_(united) {}

  int a() const { return a_; }
  int b() const { return b_; }
  int united() const { return united_; }
  int intersected() const { return a_ + b_ - united_; }

  bool are_equal() const { return united_ == a_ && united_ == b_; }

private:
  int a_ = 0;
  int b_ = 0;
  int united_ = 0;
};

class SpaceMappingRanks {
public:
  SpaceMappingRanks(int space, int image) : space_(space), image_(image) {}

  int space() const { return space_; }
  int image() const { return image_; }
  int kernel() const { return space_ - image_; }

private:
  int space_ = 0;
  int image_ = 0;
};

std::string to_string(const SpaceVennRanks& ranks);
std::string to_string(const SpaceMappingRanks& ranks);


template<typename SpaceT, typename PrepareF, typename MatrixBuilderT>
void add_space_to_matrix_builder(const SpaceT& space, const PrepareF& prepare, MatrixBuilderT& matrix_builder) {
  check_spaces(space);
  const auto space_prepared = mapped_parallel(space, [prepare](const auto& s) {
    return prepare(s);
  });
  for (const auto& expr : space_prepared) {
    matrix_builder.add_expr(expr);
  }
}

template<typename SpaceT, typename PrepareF>
Matrix space_matrix(const SpaceT& space, const PrepareF& prepare) {
  using ExprT = std::decay_t<std::invoke_result_t<PrepareF, typename SpaceT::value_type>>;
  GetExprMatrixBuilder_t<ExprT> matrix_builder;
  add_space_to_matrix_builder(space, prepare, matrix_builder);
  return matrix_builder.make_matrix();
}

template<typename SpaceT, typename PrepareF>
int space_rank(const SpaceT& space, const PrepareF& prepare) {
  return matrix_rank(space_matrix(space, prepare));
}

template<typename SpaceT, typename PrepareF>
bool space_contains(const SpaceT& haystack, const SpaceT& needle, const PrepareF& prepare) {
  check_spaces(haystack, needle);
  using ExprT = std::decay_t<std::invoke_result_t<PrepareF, typename SpaceT::value_type>>;
  GetExprMatrixBuilder_t<ExprT> matrix_builder;
  add_space_to_matrix_builder(haystack, prepare, matrix_builder);
  const int haystack_rank = matrix_rank(matrix_builder.make_matrix());
  add_space_to_matrix_builder(needle, prepare, matrix_builder);
  const int united_rank = matrix_rank(matrix_builder.make_matrix());
  CHECK_LE(haystack_rank, united_rank);
  return united_rank == haystack_rank;
}

template<typename SpaceT, typename PrepareF>
SpaceVennRanks space_venn_ranks(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  check_spaces(a, b);
  using ExprT = std::decay_t<std::invoke_result_t<PrepareF, typename SpaceT::value_type>>;
  Profiler profiler(false);

  GetExprMatrixBuilder_t<ExprT> matrix_builder_a;
  add_space_to_matrix_builder(a, prepare, matrix_builder_a);
  profiler.finish("A space");
  const int a_rank = matrix_rank(matrix_builder_a.make_matrix());
  profiler.finish("A rank");
  add_space_to_matrix_builder(b, prepare, matrix_builder_a);
  profiler.finish("united space");
  const int united_rank = matrix_rank(matrix_builder_a.make_matrix());
  profiler.finish("united rank");

  GetExprMatrixBuilder_t<ExprT> matrix_builder_b;
  add_space_to_matrix_builder(b, prepare, matrix_builder_b);
  profiler.finish("B space");
  const int b_rank = matrix_rank(matrix_builder_b.make_matrix());
  profiler.finish("B rank");

  CHECK_LE(a_rank, united_rank);
  CHECK_LE(b_rank, united_rank);
  return SpaceVennRanks{a_rank, b_rank, united_rank};
}

template<typename SpaceT, typename PrepareF>
bool space_equal(const SpaceT& a, const SpaceT& b, const PrepareF& prepare) {
  return space_venn_ranks(a, b, prepare).are_equal();
}

template<typename SpaceT, typename PrepareF, typename MapF>
SpaceMappingRanks space_mapping_ranks(const SpaceT& raw_space, const PrepareF& prepare, const MapF& map) {
  Profiler profiler(false);
  const auto space = space_matrix(raw_space, prepare);
  profiler.finish("space");
  const int space_rank = matrix_rank(space);
  profiler.finish("space rank");
  const auto image = space_matrix(raw_space, map);
  profiler.finish("image");
  const int image_rank = matrix_rank(image);
  profiler.finish("image rank");
  return SpaceMappingRanks{space_rank, image_rank};
}

template<typename SpaceT>
SpaceMappingRanks space_ncomultiply_mapping_ranks(const SpaceT& space) {
  return space_mapping_ranks(space, identity_function, DISAMBIGUATE(ncomultiply));
}
