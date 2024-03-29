// For all polylog vector spaces...
//
//   TODO: Be consistent about parallelisation.
//
//   Optimization potential: Pre-compute basis.
//
//   Optimization potential: Convert spaces to Lyndon basis on construction.
//   Pros:
//     + Less verbose, don't have to pass to_lyndon_basis to each rank computations.
//     + Could be faster if Lyndon basis is required multiple times, e.g. when spaces
//       are used to construct larger co-spaces via coproduct.
//   Cons:
//     - What if there are Hopf algebra use-cases?
//     - Could be slower if, in fact, only comultiplication of a space is required.
//
//   Optimization potential: Store spaces as a collection of `std::shared_ptr<const Expr>`
//     instead of raw `Expr`.
//   Pros:
//     + Less copying, especially in expressions like `cartesian_product(space_a, space_b)`.
//   Cons:
//     - More verbose interface due to having to pack/update shared_ptr.
//     - Might slow down parallel code if all threads are reading from the same place
//       (although it's all read-only access - how bad is that?)

#include "vector_space.h"


static thread_local bool space_homogeneity_check_flag = true;

bool space_homogeneity_check_enabled() {
  return space_homogeneity_check_flag;
}

ScopedDisableSpaceHomogeneityCheck::ScopedDisableSpaceHomogeneityCheck() {
  old_value = space_homogeneity_check_flag;
  space_homogeneity_check_flag = false;
}

ScopedDisableSpaceHomogeneityCheck::~ScopedDisableSpaceHomogeneityCheck() {
  space_homogeneity_check_flag = old_value;
}
