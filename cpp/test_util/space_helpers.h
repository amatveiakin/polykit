#pragma once

#include "cpp/lib/linear.h"
#include "cpp/lib/lyndon.h"
#include "cpp/lib/space_algebra.h"
#include "cpp/lib/vector_space.h"


template<typename SpaceF>
int simple_space_rank(const SpaceF& space, int num_points) {
  return matrix_rank(space_matrix(
    space(seq_incl(1, num_points)),
    DISAMBIGUATE(to_lyndon_basis)
  ));
}

struct ClusterCoRanks {
  int space = 0;
  int kernel = 0;
  bool operator==(const ClusterCoRanks& other) const { return space == other.space && kernel == other.kernel; }
};

template<typename SpaceT>
ClusterCoRanks cluster_co_ranks(const SpaceT& space) {
  const int space_rank = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
    return keep_non_weakly_separated(expr);
  }).kernel();
  const int kernel_rank = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
    return std::make_tuple(keep_non_weakly_separated(expr), ncomultiply(expr));
  }).kernel();
  return {space_rank, kernel_rank};
}
