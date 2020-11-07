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

// TODO: Move to util
// TODO: Optimize: https://en.wikipedia.org/wiki/Lexicographically_minimal_string_rotation
std::vector<X> lexicographically_minimal_rotation(std::vector<X> v) {
  std::vector<X> min = v;
  for (int i = 1; i < v.size(); ++i) {
    absl::c_rotate(v, v.begin() + 1);
    if (v < min) {
      min = v;
    }
  }
  return min;
}

DeltaExpr CorrVec(const std::vector<X>& points) {
  return I_impl(concat({Inf}, points)).annotate(
    fmt::function("Corr", mapped(
      lexicographically_minimal_rotation(points),
      [](X x){ return to_string(x); })
    )
  );
}
