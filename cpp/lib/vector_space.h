#pragma once

#include "expr_matrix_builder.h"
#include "integer_math.h"
#include "linalg.h"
#include "parallel_util.h"
#include "profiler.h"


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

// TODO: Templatize.
struct SpaceCharacteristics {
  int weight = 0;
  int dimension = 0;
  bool operator==(const SpaceCharacteristics& other) const {
    return weight == other.weight && dimension == other.dimension;
  }
};

std::string to_string(const SpaceCharacteristics& characteristics);

// TODO: Also check that space characteristics match in functions like `space_venn_ranks`.
// Verifies that each element has the same weight and dimension. Feel free to disable if not required.
template<typename SpaceT>
void check_space(const SpaceT& space) {
  return;  // TODO: Fix the check for points in involution and re-enable it!
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
SpaceT normalize_space_remove_consecutive(const SpaceT& space, int dimension, int num_points) {
  return mapped(space, [&](const auto& expr) {
    return normalize_remove_consecutive(expr, dimension, num_points);
  });
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

// TODO: Optimize: don't compute rank on each step.
// Optimization potential: pre-apply for common spaces.
template<typename SpaceT, typename PrepareF>
SpaceT space_basis(const SpaceT& space, const PrepareF& prepare) {
  SpaceT ret;
  int rank = 0;
  for (const auto& expr : mapped(space, prepare)) {
    ret.push_back(expr);
    const int new_rank = space_rank(ret, DISAMBIGUATE(identity_function));
    if (new_rank == rank) {
      ret.pop_back();
    }
    rank = new_rank;
  }
  return ret;
}


template<typename SpaceF, typename CombineF>
auto abstract_co_space(int weight, int num_coparts, const SpaceF& get_space, const CombineF& combine) {
  CHECK_LE(num_coparts, weight);
  const auto weights_per_summand = get_partitions(weight, num_coparts);
  const int max_atom_weight = max_value(flatten(weights_per_summand));
  const auto atom_spaces = mapped(range_incl(1, max_atom_weight), get_space);
  return mapped_expanding(weights_per_summand, [&](const auto& summand_weights) {
    const auto summand_components = cartesian_combinations(
      mapped(group_equal(summand_weights), [&](const auto& equal_weight_group) {
        return std::pair{
          atom_spaces.at(equal_weight_group.front() - 1),
          static_cast<int>(equal_weight_group.size())
        };
      })
    );
    return mapped(summand_components, combine);
  });
}

// Computes a co-space of a given structure from spaces provided by `get_space`.
// E.g. for weight == 5, num_coparts == 3 returns:
//   + get_space(3) * lambda^2 get_space(1)
//   + lambda^2 get_space(2) * get_space(1)
template<typename SpaceF>
auto co_space(int weight, int num_coparts, const SpaceF& get_space) {
  return abstract_co_space(weight, num_coparts, get_space, DISAMBIGUATE(ncoproduct_vec));
}
