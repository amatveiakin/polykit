#include "iterated_integral.h"

#include "absl/types/span.h"

#include "algebra.h"


static DeltaExpr I_3_point(const absl::Span<const X>& p) {
  CHECK_EQ(p.size(), 3);
  return D(p[2], p[1]) - D(p[1], p[0]);
}

template<typename ResultT, typename ProjectorT>
static ResultT I_impl(const std::vector<X>& points, const ProjectorT& projector) {
  const int num_points = points.size();
  CHECK_GE(num_points, 3);
  ResultT ret;
  if (num_points == 3) {
    ret = projector(I_3_point(absl::MakeConstSpan(points)));
  } else {
    for (int i = 0; i <= num_points - 3; ++i) {
      ret += tensor_product(
        projector(I_3_point(absl::MakeConstSpan(points).subspan(i, 3))),
        I_impl<ResultT>(removed_index(points, i+1), projector)
      );
    }
  }
  return ret;
}

template<typename ResultT, typename ProjectorT>
static ResultT IVec_wrapper(const std::vector<X>& points, const ProjectorT& projector) {
  return I_impl<ResultT>(points, projector).annotate(
    fmt::function("I", mapped(points, [](X x){ return to_string(x); })));
}

DeltaExpr IVec(const std::vector<X>& points) {
  return IVec_wrapper<DeltaExpr>(points, identity_function);
}

WordExpr IVecPr(
    const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return IVec_wrapper<WordExpr>(points, projector);
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

template<typename ResultT, typename ProjectorT>
ResultT CorrVec_wrapper(const std::vector<X>& points, const ProjectorT& projector) {
  return I_impl<ResultT>(concat({Inf}, points), projector).annotate(
    fmt::function("Corr", mapped(
      lexicographically_minimal_rotation(points),
      [](X x){ return to_string(x); })
    )
  );
}

DeltaExpr CorrVec(const std::vector<X>& points) {
  return CorrVec_wrapper<DeltaExpr>(points, identity_function);
}

WordExpr CorrVecPr(
    const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return CorrVec_wrapper<WordExpr>(points, projector);
}
