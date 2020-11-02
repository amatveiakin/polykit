#include "iterated_integral.h"

#include "absl/types/span.h"

#include "algebra.h"


static DeltaExpr I_3_point(const absl::Span<const X>& p) {
  CHECK_EQ(p.size(), 3);
  return D(p[2], p[1]) - D(p[1], p[0]);
}

// Optimization potential: Add cache
static DeltaExpr I_impl(const std::vector<X>& points) {
  const int num_points = points.size();
  CHECK_GE(num_points, 3);
  DeltaExpr ret;
  if (num_points == 3) {
    ret = I_3_point(absl::MakeConstSpan(points));
  } else {
    for (int i = 0; i <= num_points - 3; ++i) {
      ret += tensor_product(
        I_3_point(absl::MakeConstSpan(points).subspan(i, 3)),
        I_impl(removed_index(points, i+1))
      );
    }
  }
  return ret;
}

DeltaExpr IVec(const std::vector<X>& points) {
  return I_impl(points).annotate(
    fmt::function("I", mapped(points, [](X x){ return to_string(x); })));
}
