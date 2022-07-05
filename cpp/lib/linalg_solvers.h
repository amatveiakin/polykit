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
ExprT find_equation(ExprT expr, const std::vector<ExprT>& space, const std::vector<int>& coeff_candidates = {1, -1}) {
  Profiler profiler(true);
  CHECK(space_contains(space, {expr}, DISAMBIGUATE(identity_function)));
  profiler.finish("check space contains");

  using StorageT = typename ExprT::StorageT;
  absl::flat_hash_map<StorageT, int> terms_count;  // how many expressions contain this term
  for (const auto& summand : space) {
    for (const auto& [term, coeff] : key_view(summand)) {
      terms_count[term] += 1;  // sic: `1`, not `coeff`
    }
  }

  std::vector<ExprT> space_remaining;
  for (const auto& summand : space) {
    bool found_coeff = false;
    for (const auto& [term, coeff] : key_view(summand)) {
      if (terms_count.at(term) == 1) {
        const int numerator = expr.coeff_for_key(term);
        const int denominator = coeff;
        CHECK(numerator % denominator == 0)
            << "need non-integer coeff" << numerator << "/" << denominator << " for summand " << expr;
        expr -= div_int(numerator, denominator) * summand;
        found_coeff = true;
        break;
      }
    }
    if (!found_coeff) {
      space_remaining.push_back(summand);
    }
  }
  CHECK(space_contains(space_remaining, {expr}, DISAMBIGUATE(identity_function)));
  profiler.finish(absl::StrCat(
    "done with unique terms (", space.size() - space_remaining.size(), "/", space.size(), ")")
  );
  if (expr.is_zero()) {
    return expr;
  }

  while (!space_remaining.empty()) {
    const auto summand = std::move(space_remaining.back());
    space_remaining.pop_back();
    bool found_coeff = false;
    for (const int coeff : coeff_candidates) {
      auto candidate_expr = expr + coeff * summand;
      if (space_contains(space_remaining, {candidate_expr}, DISAMBIGUATE(identity_function))) {
        expr = std::move(candidate_expr);
        found_coeff = true;
        break;
      }
    }
    CHECK(found_coeff) << "Cannot find coeff for " << summand;
    std::cout << space_remaining.size() << "..." << std::flush;
  }
  profiler.finish("done with other terms");
  CHECK(expr.is_zero());
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
