#pragma once

#include "integer_math.h"
#include "itertools.h"


// TODO: Templatize.
template<typename MarkerT>
struct SpaceCharacteristics {
  int weight = 0;
  MarkerT marker = {};
  bool operator==(const SpaceCharacteristics& other) const {
    return weight == other.weight && marker == other.marker;
  }
};

template<typename MarkerT>
std::string to_string(const SpaceCharacteristics<MarkerT>& characteristics) {
  if constexpr (std::is_same_v<MarkerT, std::monostate>) {
    return absl::StrCat("{w=", characteristics.weight, "}");
  } else {
    return absl::StrCat("{w=", characteristics.weight, "; ", to_string(characteristics.marker), "}");
  }
}

// Verifies that each element has the same weight and dimension. Feel free to disable if not required.
template<typename... SpaceTs>
void check_spaces(const SpaceTs&... spaces) {
  // TODO: Fix for spaces of tuples.
  check_space_homogeneity([](const auto& expr) {
    using Marker = std::decay_t<decltype(expr.uniformity_marker())>;
    return SpaceCharacteristics<Marker>{expr.weight(), expr.uniformity_marker()};
  }, spaces...);
}

template<typename F, typename... SpaceTs>
void check_space_homogeneity(const F& func, const SpaceTs&... spaces) {
  const auto spaces_list = {std::cref(spaces)...};
  using Space = typename decltype(spaces_list)::value_type::type;
  using Expr = typename Space::value_type;
  std::optional<std::invoke_result_t<F, Expr>> exemplar;
  const Expr* exemplar_ptr = nullptr;
  for (const auto& space : spaces_list) {
    for (const auto& expr : space.get()) {
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
