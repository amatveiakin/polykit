#pragma once

#include "integer_math.h"
#include "itertools.h"


bool space_homogeneity_check_enabled();

// Creating/destroying this is not thread-safe.
class ScopedDisableSpaceHomogeneityCheck {
public:
  ScopedDisableSpaceHomogeneityCheck();
  ~ScopedDisableSpaceHomogeneityCheck();
private:
  bool old_value = false;
};

template<typename MarkerT>
struct SpaceCharacteristics {
  int weight = 0;
  MarkerT marker = {};
  bool operator==(const SpaceCharacteristics& other) const {
    return weight == other.weight && marker == other.marker;
  }
};

template<typename ExprT>
auto expression_characteristics(const ExprT& expr) {
  using Marker = std::decay_t<decltype(expr.uniformity_marker())>;
  return SpaceCharacteristics<Marker>{expr.weight(), expr.uniformity_marker()};
}

template<typename MarkerT>
std::string to_string(const SpaceCharacteristics<MarkerT>& characteristics) {
  if constexpr (std::is_same_v<MarkerT, std::monostate>) {
    return absl::StrCat("{w=", characteristics.weight, "}");
  } else {
    return absl::StrCat("{w=", characteristics.weight, "; ", to_string(characteristics.marker), "}");
  }
}

template<typename ExprT>
struct GetSpaceElementCharacteristics {
  static auto characteristics(const ExprT& expr) { return expression_characteristics(expr); }
  static bool should_check(const ExprT& expr) { return !expr.is_zero(); }
  static std::string description(const ExprT& expr) { return annotations_one_liner(expr.annotations()); }
};

template<typename... ExprT>
struct GetSpaceElementCharacteristics<std::tuple<ExprT...>> {
  static auto characteristics(const std::tuple<ExprT...>& exprs) {
    return characteristics_impl(exprs, std::make_index_sequence<std::tuple_size_v<std::tuple<ExprT...>>>{});
  }
  static bool should_check(const std::tuple<ExprT...>&) {
    // For simplicity, assume tuple elements are always non-zero
    return true;
  }
  static std::string description(const std::tuple<ExprT...>& exprs) {
    return absl::StrCat("(", str_join(exprs, ", ", [](const auto& expr) {
      return annotations_one_liner(expr.annotations());
    }), ")");
  }
private:
  template<typename std::size_t... Idx>
  static auto characteristics_impl(const std::tuple<ExprT...>& exprs, std::index_sequence<Idx...>) {
    return std::tuple{expression_characteristics(std::get<Idx>(exprs))...};
  }
};

template<typename ExprT>
auto get_space_element_characteristics(const ExprT& expr) {
  return GetSpaceElementCharacteristics<ExprT>::characteristics(expr);
}

template<typename ExprT>
bool should_check_space_element_characteristics(const ExprT& expr) {
  return GetSpaceElementCharacteristics<ExprT>::should_check(expr);
}

template<typename ExprT>
std::string space_element_description(const ExprT& expr) {
  return GetSpaceElementCharacteristics<ExprT>::description(expr);
}

// Verifies that each element has the same weight and dimension.
// Use ScopedDisableSpaceHomogeneityCheck if this check not required.
template<typename... SpaceTs>
void check_spaces(const SpaceTs&... spaces) {
  if (space_homogeneity_check_enabled()) {
    check_space_homogeneity([](const auto& expr) {
      return get_space_element_characteristics(expr);
    }, spaces...);
  }
}

template<typename F, typename... SpaceTs>
void check_space_homogeneity(const F& func, const SpaceTs&... spaces) {
  const auto spaces_list = {std::cref(spaces)...};
  using Space = typename decltype(spaces_list)::value_type::type;
  using Expr = typename Space::value_type;
  std::optional<std::decay_t<std::invoke_result_t<F, Expr>>> exemplar;
  const Expr* exemplar_ptr = nullptr;
  for (const auto& space : spaces_list) {
    for (const auto& expr : space.get()) {
      // TODO: Benchmark if checking each term of an expression is too slow.
      if (should_check_space_element_characteristics(expr)) {
        const auto new_value = func(expr);
        if (exemplar.has_value()) {
          CHECK(exemplar == new_value)
            << "  space not homogeneous:\n"
            << dump_to_string(exemplar.value()) << " vs " << dump_to_string(new_value) << "\n"
            << "  for\n"
            << space_element_description(*exemplar_ptr)
            << " vs " << space_element_description(expr);
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

template<typename... Spaces>
auto space_ncoproduct(const Spaces&... spaces) {
  // Convert each space to normal co-form first. Serves two purposes:
  //   - Allows to mix co-spaces and regular spaces.
  //   - Precomputes Lyndon to speed up actual space coproduct.
  const std::tuple spaces_lyndon{ mapped(spaces, DISAMBIGUATE(ncoproduct))... };
  return mapped_parallel(
    std::apply(DISAMBIGUATE(cartesian_product), spaces_lyndon),
    applied(DISAMBIGUATE(ncoproduct))
  );
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
