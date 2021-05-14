#include "iterated_integral.h"

#include "absl/types/span.h"

#include "algebra.h"
#include "lexicographical.h"


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
    for (int i : range_incl(num_points - 3)) {
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
    fmt::function_num_args(fmt::opname("I"), points)
  );
}

DeltaExpr IVec(const XArgs& points) {
  return IVec_wrapper<DeltaExpr>(points.as_x(), identity_function);
}

ProjectionExpr IVecPr(const XArgs& points, DeltaProjector projector) {
  return IVec_wrapper<ProjectionExpr>(points.as_x(), projector);
}

template<typename ResultT, typename ProjectorT>
ResultT CorrVec_wrapper(const std::vector<X>& points, const ProjectorT& projector) {
  return I_impl<ResultT>(concat({Inf}, points), projector).annotate(
    fmt::function_num_args(fmt::opname("Corr"), lexicographically_minimal_rotation(points))
  );
}

DeltaExpr CorrVec(const XArgs& points) {
  return CorrVec_wrapper<DeltaExpr>(points.as_x(), identity_function);
}

ProjectionExpr CorrVecPr(const XArgs& points, DeltaProjector projector) {
  return CorrVec_wrapper<ProjectionExpr>(points.as_x(), projector);
}

DeltaExpr eval_formal_symbols(const CorrExpr& expr) {
  return expr.mapped_expanding([](const CorrFSymb& formal_symbol) {
    return CorrVec(formal_symbol.points).without_annotations();
  });
}
