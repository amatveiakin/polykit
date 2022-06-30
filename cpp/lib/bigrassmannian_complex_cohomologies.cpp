#include "bigrassmannian_complex_cohomologies.h"

#include "chern_arrow.h"
#include "lyndon.h"
#include "space_algebra.h"


template<typename SpaceF>
SpaceMappingRanks cohomology_ranks(int dimension, int num_points, const SpaceF& space) {
  ScopedDisableSpaceHomogeneityCheck dshc;
  return space_mapping_ranks(
    mapped_expanding(range_incl(dimension, num_points - 1), [&](const int dim) {
      return space(dim, to_vector(range_incl(1, num_points)));
    }),
    DISAMBIGUATE(to_lyndon_basis),
    [&](const auto& expr) {
      std::vector<std::decay_t<decltype(expr)>> row(num_points - 1);
      const int dim_index = expr.dimension() - dimension;
      row.at(dim_index) = to_lyndon_basis(chern_arrow_left(expr, num_points + 1));
      row.at(dim_index + 1) = to_lyndon_basis(chern_arrow_up(expr, num_points + 1));
      return row;
    }
  );
}

// TODO: Dualize
template<typename SpaceF>
int compute_cohomologies(int dimension, int num_points, const SpaceF& space) {
  return (
    + cohomology_ranks(dimension, num_points, space).kernel()
    - cohomology_ranks(dimension, num_points - 1, space).image()
  );
}


int bigrassmannian_complex_cohomology(
  int dimension, int num_points, const std::function<Gr_Space(int, const std::vector<int>&)>& space
) {
  return compute_cohomologies(dimension, num_points, space);
}

int bigrassmannian_complex_cohomology(
  int dimension, int num_points, const std::function<Gr_NCoSpace(int, const std::vector<int>&)>& space
) {
  return compute_cohomologies(dimension, num_points, space);
}
