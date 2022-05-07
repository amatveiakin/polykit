#pragma once

#include "integer_math.h"
#include "itertools.h"


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


template<typename SpaceT>
SpaceT normalize_space_remove_consecutive(const SpaceT& space, int dimension, int num_points) {
  return mapped(space, [&](const auto& expr) {
    return normalize_remove_consecutive(expr, dimension, num_points);
  });
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
