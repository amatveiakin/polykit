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
using GrPolylogACoSpace = std::vector<GammaACoExpr>;

std::string dump_to_string_impl(const PolylogSpace& space);
std::string dump_to_string_impl(const PolylogNCoSpace& space);
std::string dump_to_string_impl(const GrPolylogSpace& space);
std::string dump_to_string_impl(const GrPolylogNCoSpace& space);
std::string dump_to_string_impl(const GrPolylogACoSpace& space);

PolylogSpace CB_naive_via_QLi_fours(int weight, const XArgs& xargs);
PolylogSpace CB(int weight, const XArgs& xargs);
inline PolylogSpace CB1(const XArgs& xargs) { return CB(1, xargs); }
inline PolylogSpace CB2(const XArgs& xargs) { return CB(2, xargs); }
inline PolylogSpace CB3(const XArgs& xargs) { return CB(3, xargs); }
inline PolylogSpace CB4(const XArgs& xargs) { return CB(4, xargs); }
inline PolylogSpace CB5(const XArgs& xargs) { return CB(5, xargs); }
inline PolylogSpace CB6(const XArgs& xargs) { return CB(6, xargs); }
inline PolylogSpace CB7(const XArgs& xargs) { return CB(7, xargs); }
inline PolylogSpace CB8(const XArgs& xargs) { return CB(8, xargs); }

PolylogSpace CL(int weight, const XArgs& xargs);
inline PolylogSpace CL1(const XArgs& xargs) { return CL(1, xargs); }
inline PolylogSpace CL2(const XArgs& xargs) { return CL(2, xargs); }
inline PolylogSpace CL3(const XArgs& xargs) { return CL(3, xargs); }
inline PolylogSpace CL4(const XArgs& xargs) { return CL(4, xargs); }
inline PolylogSpace CL5(const XArgs& xargs) { return CL(5, xargs); }
inline PolylogSpace CL6(const XArgs& xargs) { return CL(6, xargs); }
inline PolylogSpace CL7(const XArgs& xargs) { return CL(7, xargs); }
inline PolylogSpace CL8(const XArgs& xargs) { return CL(8, xargs); }
PolylogSpace old_CL4_via_A2(const XArgs& xargs);

PolylogSpace H(int weight, const XArgs& xargs);

PolylogSpace Fx(const XArgs& xargs);

PolylogSpace LInf(int weight, const XArgs& xargs);
PolylogSpace L(int weight, const XArgs& xargs);
inline PolylogSpace L1(const XArgs& xargs) { return L(1, xargs); }
inline PolylogSpace L2(const XArgs& xargs) { return L(2, xargs); }
inline PolylogSpace L3(const XArgs& xargs) { return L(3, xargs); }
inline PolylogSpace L4(const XArgs& xargs) { return L(4, xargs); }
inline PolylogSpace L5(const XArgs& xargs) { return L(5, xargs); }
inline PolylogSpace L6(const XArgs& xargs) { return L(6, xargs); }
inline PolylogSpace L7(const XArgs& xargs) { return L(7, xargs); }
inline PolylogSpace L8(const XArgs& xargs) { return L(8, xargs); }

PolylogSpace XCoords(int weight, const XArgs& args);
PolylogSpace ACoords(int weight, const XArgs& args);
PolylogSpace ACoordsHopf(int weight, const XArgs& args);

GrPolylogSpace GrFx(int dimension, const std::vector<int>& args);

GrPolylogSpace GrL_core(
  int weight, int dimension, const std::vector<int>& args,
  bool include_one_minus_cross_ratio, int num_fixed_points);
GrPolylogSpace GrL1(int dimension, const std::vector<int>& args);
GrPolylogSpace GrL2(int dimension, const std::vector<int>& args);
GrPolylogSpace GrL3(int dimension, const std::vector<int>& args);
GrPolylogSpace GrL(int weight, int dimension, const std::vector<int>& args);

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
  using ExprT = typename decltype(atom_spaces)::value_type::value_type;
  std::vector<NCoExprForExpr_t<ExprT>> ret;
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

PolylogNCoSpace co_CL(int weight, int num_coparts, const XArgs& xargs);


template<typename SpaceT, typename F>
void check_space_is_homogeneous(const SpaceT& space, const F& func) {
  std::optional<std::invoke_result_t<F, typename SpaceT::value_type>> exemplar;
  const typename SpaceT::value_type* exemplar_ptr = nullptr;
  for (const auto& expr : space) {
    // TODO: Benchmark if checking each term of an expression is too slow.
    if (!expr.is_zero()) {
      const auto new_value = func(expr);
      if (exemplar.has_value()) {
        CHECK(exemplar == new_value)
          << "  space not homogeneous:\n"
          << dump_to_string(exemplar.value()) << " vs " << dump_to_string(new_value) << "\n"
          << "  for\n"
          << annotations_one_liner(exemplar_ptr->annotations())
          << " vs " << annotations_one_liner(expr.annotations());
      } else {
        exemplar = new_value;
        exemplar_ptr = &expr;
      }
    }
  }
}

struct SpaceCharacteristics {
  int weight = 0;
  int dimension = 0;
  bool operator==(const SpaceCharacteristics& other) const {
    return weight == other.weight && dimension == other.dimension;
  }
};

std::string to_string(const SpaceCharacteristics& characteristics);

// Verifies that each element has the same weight and dimension. Feel free to disable if not required.
template<typename SpaceT>
void check_space(const SpaceT& space) {
  check_space_is_homogeneous(space, [](const auto& expr) {
    return SpaceCharacteristics{expr.weight(), expr.dimension()};
  });
}

template<typename SpaceT>
void check_space_weight_eq(const SpaceT& space, int weight) {
  for (const auto& expr : space) {
    if (!expr.is_zero()) {
      CHECK_EQ(expr.weight(), weight);
    }
  }
}

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
  check_space(space);
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
