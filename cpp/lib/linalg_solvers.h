// TODO: Use proper linear solver instead of this rank-based nonsense.

#pragma once

#include "space_algebra.h"


// Optimization potential: If some expressions in `needle` contain unique terms, use these
//   to determine whether the expressions need to be included, without rank computations.
// Optimization potential: Fewer rank computations
// Optimization potential: Re-use matrix builder
template<typename SpaceT>
SpaceT minimal_containing_subspace(const SpaceT& haystack, const SpaceT& needle) {
  CHECK(space_contains(haystack, needle, DISAMBIGUATE(identity_function)));
  auto ret = haystack;
  int idx = 0;
  while (idx < ret.size()) {
    auto new_ret = ret;
    new_ret.erase(new_ret.begin() + idx);
    if (space_contains(new_ret, needle, DISAMBIGUATE(identity_function))) {
      ret = std::move(new_ret);
    } else {
      ++idx;
    }
  }
  return ret;
}

// Optimization potential: If some expressions in `space` contain unique terms, use these
//   to compute the coefficient.
// Note. If `coeff_candidates` does not include 0, the option of not including this summand
//   will not be considered.
template<typename ExprT>
ExprT find_equation(ExprT expr, std::vector<ExprT> space, const std::vector<int>& coeff_candidates = {1, -1}) {
  CHECK(space_contains(space, {expr}, DISAMBIGUATE(identity_function)));
  while (!space.empty()) {
    const auto summand = std::move(space.back());
    space.pop_back();
    bool found_coeff = false;
    for (const int coeff : coeff_candidates) {
      auto candidate_expr = expr + coeff * summand;
      if (space_contains(space, {candidate_expr}, DISAMBIGUATE(identity_function))) {
        expr = std::move(candidate_expr);
        found_coeff = true;
        break;
      }
    }
    CHECK(found_coeff) << "Cannot find coeff for " << summand;
  }
  return expr;
}

// Optimization potential: Re-use matrix builder
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
