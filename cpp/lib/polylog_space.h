// Optimization potential: convert to Lyndon basis earlier to avoid doing this multiple times.

#pragma once

#include "delta.h"
#include "expr_matrix_builder.h"
#include "gamma.h"
#include "integer_math.h"
#include "itertools.h"
#include "linalg.h"
#include "parallel_util.h"
#include "profiler.h"
#include "x.h"


using PolylogSpace = std::vector<DeltaExpr>;
using PolylogNCoSpace = std::vector<DeltaNCoExpr>;
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
PolylogSpace CL5(const XArgs& xargs);

PolylogSpace CB(int weight, const XArgs& xargs);
PolylogSpace CL(int weight, const XArgs& xargs);

PolylogSpace old_CL4_via_A2(const XArgs& xargs);

PolylogSpace H(int weight, const XArgs& xargs);

PolylogSpace LInf(int weight, const XArgs& xargs);
PolylogSpace L(int weight, const XArgs& xargs);
inline PolylogSpace L1(const XArgs& xargs) { return L(1, xargs); }
inline PolylogSpace L2(const XArgs& xargs) { return L(2, xargs); }
inline PolylogSpace L3(const XArgs& xargs) { return L(3, xargs); }
inline PolylogSpace L4(const XArgs& xargs) { return L(4, xargs); }
inline PolylogSpace L5(const XArgs& xargs) { return L(5, xargs); }
inline PolylogSpace L6(const XArgs& xargs) { return L(6, xargs); }

PolylogSpace XCoords(int weight, const XArgs& args);
PolylogSpace ACoords(int weight, const XArgs& args);
PolylogSpace ACoordsHopf(int weight, const XArgs& args);

PolylogNCoSpace co_CL_3(const XArgs& args);
PolylogNCoSpace co_CL_4(const XArgs& args);
PolylogNCoSpace co_CL_5(const XArgs& args);
PolylogNCoSpace co_CL_6(const XArgs& args);
PolylogNCoSpace co_CL_6_via_l(const XArgs& args);

PolylogNCoSpace QL_wedge_QL(int weight, const XArgs& xargs);

GrPolylogSpace GrLBasic(int weight, const XArgs& xargs);  // dimension = 3
GrPolylogSpace GrL1(int dimension, const XArgs& xargs);
GrPolylogSpace GrL2(int dimension, const XArgs& xargs);
GrPolylogSpace GrL3(int dimension, const XArgs& xargs);

GrPolylogSpace GrL(int weight, int dimension, const XArgs& xargs);

// Computes a co-space of a given structure from spaces provided by `get_space`.
// E.g. for weight == 5, num_coparts == 3 returns:
//   + get_space(3) * lambda^2 get_space(1)
//   + lambda^2 get_space(2) * get_space(1)
template<typename SpaceF>
auto co_space(int weight, int num_coparts, const SpaceF& get_space) {
  CHECK_LE(num_coparts, weight);
  const auto weights_per_summand = get_partitions(weight, num_coparts);
  const int max_atom_weight = max_value(flatten(weights_per_summand));
  const auto atom_spaces = mapped(range_incl(1, max_atom_weight), get_space);
  std::vector<NCoExprForExpr_t<typename decltype(atom_spaces)::value_type::value_type>> ret;
  for (const auto& summand_weights : weights_per_summand) {
    auto summand_components = cartesian_combinations(
      mapped(group_equal(summand_weights), [&](const auto& equal_weight_group) {
        return std::pair{
          atom_spaces.at(equal_weight_group.front() - 1),
          static_cast<int>(equal_weight_group.size())
        };
      })
    );
    append_vector(ret, mapped(summand_components, DISAMBIGUATE(ncoproduct_vec)));
  }
  return ret;
}

// Note: applies normalize_remove_consecutive (hence not allowing arbitrary input points).
PolylogNCoSpace simple_co_L(int weight, int num_coparts, int num_points);
GrPolylogNCoSpace simple_co_GrL(int weight, int num_coparts, int dimension, int num_points);


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


template<typename SpaceT>
SpaceT normalize_space_remove_consecutive(const SpaceT& space) {
  return mapped(space, DISAMBIGUATE(normalize_remove_consecutive));
}

template<typename SpaceT, typename PrepareF, typename MatrixBuilderT>
void add_space_to_matrix_builder(const SpaceT& space, const PrepareF& prepare, MatrixBuilderT& matrix_builder) {
  const auto space_prepared = mapped_parallel(space, [prepare](const auto& s) {
    return prepare(s);
  });
  for (const auto& expr : space_prepared) {
    matrix_builder.add_expr(expr);
  }
}

template<typename SpaceT, typename PrepareF>
Matrix space_matrix(const SpaceT& space, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
  GetExprMatrixBuilder_t<ExprT> matrix_builder;
  add_space_to_matrix_builder(space, prepare, matrix_builder);
  return matrix_builder.make_matrix();
}

template<typename SpaceT, typename PrepareF>
int space_rank(const SpaceT& space, const PrepareF& prepare) {
  return matrix_rank(space_matrix(space, prepare));
}

// TODO: Version when `needle` is a single function
template<typename SpaceT, typename PrepareF>
bool space_contains(const SpaceT& haystack, const SpaceT& needle, const PrepareF& prepare) {
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
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
  using ExprT = std::invoke_result_t<PrepareF, typename SpaceT::value_type>;
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
  return space_mapping_ranks(space, DISAMBIGUATE(identity_function), DISAMBIGUATE(ncomultiply));
}
