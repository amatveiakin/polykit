#include "polylog_qli.h"

#include "absl/strings/str_cat.h"

#include "algebra.h"
#include "check.h"
#include "sequence_iteration.h"


DeltaExpr LogVec(const XArgs& args) {
  return cross_ratio(args.as_x()).annotate(fmt::function_num_args(
    fmt::opname("Log"),
    args
  ));
}


struct Point {
  X x = 0;
  bool odd = true;
};

// Note: "pos" in "qli_pos_node_func" corresponds to the index in the
// function name: "QLi+"; "neg" in "neg_cross_ratio" stands for the fact
// that this is one minus cross ratio. There is no mistake in this mismatch.
static DeltaExpr qli_pos_node_func(const std::vector<Point>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0].odd
    ?  neg_cross_ratio(p[0].x, p[1].x, p[2].x, p[3].x)
    : -neg_cross_ratio(p[1].x, p[2].x, p[3].x, p[0].x);
}

static DeltaExpr qli_neg_node_func(const std::vector<Point>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0].odd
    ?  neg_inv_cross_ratio(p[0].x, p[1].x, p[2].x, p[3].x)
    : -neg_inv_cross_ratio(p[1].x, p[2].x, p[3].x, p[0].x);
}

template<typename ResultT, typename QLiNodeT, typename ProjectorT>
static ResultT QLi_impl(
    int weight,
    const std::vector<Point>& points,
    const QLiNodeT& qli_node_func,
    const ProjectorT& projector) {
  const int num_points = points.size();
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of QLi points: " << num_points;
  const int min_weight = (num_points - 2) / 2;
  CHECK_GE(weight, min_weight);
  const auto subsums = [&]() {
    ResultT ret;
    for (int i : range(num_points - 3)) {
      const auto foundation = concat(slice(points, 0, i+1), slice(points, i+3));
      ret += tensor_product(
        projector(qli_node_func(slice(points, i, i+4))),
        QLi_impl<ResultT>(weight - 1, foundation, qli_node_func, projector)
      );
    }
    return ret;
  };
  if (weight == min_weight) {
    if (num_points == 4) {
      return projector(qli_node_func(points));
    } else {
      return subsums();
    }
  } else {
    ResultT ret = tensor_product(
      projector(cross_ratio(mapped(points, [](Point p) { return p.x; }))),
      QLi_impl<ResultT>(weight - 1, points, qli_node_func, projector)
    );
    if (num_points > 4) {
      ret += subsums();
    }
    return ret;
  }
}

template<typename ResultT, typename QLiNodeT, typename ProjectorT>
static ResultT QLi_generic_wrapper(
    int weight,
    const std::vector<X>& points,
    const QLiNodeT& qli_node_func,
    const ProjectorT& projector) {
  if (points.size() == 2) {
    CHECK_GE(weight, 1);
    return weight == 1 ? projector(D(points[0], points[1])) : ResultT{};
  }
  std::vector<Point> tagged_points;
  for (int i : range(points.size())) {
    tagged_points.push_back({points[i], (i+1) % 2 == 1});
  }
  return QLi_impl<ResultT>(weight, tagged_points, qli_node_func, projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT QLi_wrapper(
    int weight,
    const std::vector<X>& points,
    const ProjectorT& projector) {
  return QLi_generic_wrapper<ResultT>(weight, points, qli_pos_node_func, projector)
    .annotate(fmt::function_num_args(
      fmt::sub_num(fmt::opname("QLi"), {weight}),
      points
    ));
}

DeltaExpr QLiVec(int weight, const XArgs& points) {
  return QLi_wrapper<DeltaExpr>(weight, points.as_x(), identity_function);
}

ProjectionExpr QLiVecPr(int weight, const XArgs& points, DeltaProjector projector) {
  return QLi_wrapper<ProjectionExpr>(weight, points.as_x(), projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT QLiNeg_wrapper(
    int weight,
    const std::vector<X>& points,
    const ProjectorT& projector) {
  return (
      neg_one_pow(weight) *
      QLi_generic_wrapper<ResultT>(weight, points, qli_neg_node_func, projector)
    ).annotate(fmt::function_num_args(
      fmt::sub_num(fmt::super(fmt::opname("QLi"), {"-"}), {weight}),
      points
    ));
}

DeltaExpr QLiNegVec(int weight, const XArgs& points) {
  return QLiNeg_wrapper<DeltaExpr>(weight, points.as_x(), identity_function);
}

ProjectionExpr QLiNegVecPr(int weight, const XArgs& points, DeltaProjector projector) {
  return QLiNeg_wrapper<ProjectionExpr>(weight, points.as_x(), projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT QLiSymm_wrapper(int weight, const std::vector<X>& points, const ProjectorT& projector) {
  auto qli_base = [&](const std::vector<X>& args) {
    return QLi_generic_wrapper<ResultT>(weight, args, qli_pos_node_func, projector);
  };
  ResultT ret;
  const int num_points = points.size();
  // Note: weight 1 can be defined as `QLi1(x1,x2,x3,x4) - QLi1(x1,x2) - QLi1(x3,x4)`.
  // This is symmetric, but doesn't fit comultiplication formulae.
  CHECK_GT(weight, 1) << "QLiSymm is not defined for weight 1";
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of QLi points: " << num_points;
  const int num_pairs = num_points / 2;
  for (const auto& seq : all_sequences(2, num_pairs)) {
    const int num_pairs_included = sum(seq);
    if (num_pairs_included < 2) {
      continue;
    }
    const int sign = neg_one_pow(num_pairs - num_pairs_included);
    std::vector<X> args;
    for (int pair_idx : range(seq.size())) {
      if (seq[pair_idx]) {
        args.push_back(points[2*pair_idx  ]);
        args.push_back(points[2*pair_idx+1]);
      }
    }
    ret += sign * qli_base(args);
  }
  return ret.annotate(fmt::function(
    fmt::sub_num(fmt::opname("QLiSymm"), {weight}),
    mapped(points, [](X x){ return to_string(x); })
  ));
}

DeltaExpr QLiSymmVec(int weight, const XArgs& points) {
  return QLiSymm_wrapper<DeltaExpr>(weight, points.as_x(), identity_function);
}

ProjectionExpr QLiSymmVecPr(int weight, const XArgs& points, DeltaProjector projector) {
  return QLiSymm_wrapper<ProjectionExpr>(weight, points.as_x(), projector);
}
