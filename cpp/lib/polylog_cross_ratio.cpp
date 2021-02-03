#include "polylog_cross_ratio.h"

#include "absl/strings/str_cat.h"

#include "algebra.h"
#include "check.h"
#include "sequence_iteration.h"


struct Point {
  X x = 0;
  bool odd = true;
};

// Note: "pos" in "lido_pos_node_func" corresponds to the index of the
// function defined in the article: "CLi+"; "neg" in "neg_cross_ratio"
// stands for the fact that this is one minus cross ratio. There is
// no mistake in this mismatch.
static DeltaExpr lido_pos_node_func(const std::vector<Point>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0].odd
    ?  neg_cross_ratio(p[0].x, p[1].x, p[2].x, p[3].x)
    : -neg_cross_ratio(p[1].x, p[2].x, p[3].x, p[0].x);
}

static DeltaExpr lido_neg_node_func(const std::vector<Point>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0].odd
    ?  neg_inv_cross_ratio(p[0].x, p[1].x, p[2].x, p[3].x)
    : -neg_inv_cross_ratio(p[1].x, p[2].x, p[3].x, p[0].x);
}

template<typename ResultT, typename LidoNodeT, typename ProjectorT>
static ResultT Lido_impl(
    int weight,
    const std::vector<Point>& points,
    const LidoNodeT& lido_node_func,
    const ProjectorT& projector) {
  const int num_points = points.size();
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of Lido points: " << num_points;
  const int min_weight = (num_points - 2) / 2;
  CHECK_GE(weight, min_weight);
  const auto subsums = [&]() {
    ResultT ret;
    for (int i = 0; i < num_points - 3; ++i) {
      const auto foundation = concat(slice(points, 0, i+1), slice(points, i+3));
      ret += tensor_product(
        projector(lido_node_func(slice(points, i, i+4))),
        Lido_impl<ResultT>(weight - 1, foundation, lido_node_func, projector)
      );
    }
    return ret;
  };
  if (weight == min_weight) {
    if (num_points == 4) {
      return projector(lido_node_func(points));
    } else {
      return subsums();
    }
  } else {
    ResultT ret = tensor_product(
      projector(cross_ratio(mapped(points, [](Point p) { return p.x; }))),
      Lido_impl<ResultT>(weight - 1, points, lido_node_func, projector)
    );
    if (num_points > 4) {
      ret += subsums();
    }
    return ret;
  }
}

template<typename ResultT, typename LidoNodeT, typename ProjectorT>
static ResultT Lido_generic_wrapper(
    int weight,
    const std::vector<X>& points,
    const LidoNodeT& lido_node_func,
    const ProjectorT& projector) {
  if (points.size() == 2) {
    CHECK_GE(weight, 1);
    return weight == 1 ? projector(D(points[0], points[1])) : ResultT{};
  }
  std::vector<Point> tagged_points;
  for (int i = 0; i < points.size(); ++i) {
    tagged_points.push_back({points[i], (i+1) % 2 == 1});
  }
  return Lido_impl<ResultT>(weight, tagged_points, lido_node_func, projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT Lido_wrapper(
    int weight,
    const std::vector<X>& points,
    const ProjectorT& projector) {
  return Lido_generic_wrapper<ResultT>(weight, points, lido_pos_node_func, projector)
    .annotate(fmt::function(
      fmt::sub_num("Lido", {weight}),
      mapped(points, [](X x){ return to_string(x); })
    ));
}

DeltaExpr LidoVec(int weight, SpanX points) {
  return Lido_wrapper<DeltaExpr>(weight, points.as_x(), identity_function);
}

ProjectionExpr LidoVecPr(int weight, SpanX points, DeltaProjector projector) {
  return Lido_wrapper<ProjectionExpr>(weight, points.as_x(), projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT LidoNeg_wrapper(
    int weight,
    const std::vector<X>& points,
    const ProjectorT& projector) {
  return (
      neg_one_pow(weight) *
      Lido_generic_wrapper<ResultT>(weight, points, lido_neg_node_func, projector)
    ).annotate(fmt::function(
      fmt::sub_num("LidoNeg", {weight}),
      mapped(points, [](X x){ return to_string(x); })
    ));
}

DeltaExpr LidoNegVec(int weight, SpanX points) {
  return LidoNeg_wrapper<DeltaExpr>(weight, points.as_x(), identity_function);
}

ProjectionExpr LidoNegVecPr(int weight, SpanX points, DeltaProjector projector) {
  return LidoNeg_wrapper<ProjectionExpr>(weight, points.as_x(), projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT LidoSymm_wrapper(int weight, const std::vector<X>& points, const ProjectorT& projector) {
  auto lido_base = [&](const std::vector<X>& args) {
    return Lido_generic_wrapper<ResultT>(weight, args, lido_pos_node_func, projector);
  };
  ResultT ret;
  const int num_points = points.size();
  // Note: weight 1 can be defined as `Lido1(x1,x2,x3,x4) - Lido1(x1,x2) - Lido1(x3,x4)`.
  // This is symmetric, but doesn't fit comultiplication formulae.
  CHECK_GT(weight, 1) << "LydoSymm is not defined for weight 1";
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of Lido points: " << num_points;
  const int num_pairs = num_points / 2;
  for (const auto& seq : all_sequences(2, num_pairs)) {
    const int num_pairs_included = absl::c_accumulate(seq, 0);
    if (num_pairs_included < 2) {
      continue;
    }
    const int sign = neg_one_pow(num_pairs - num_pairs_included);
    std::vector<X> args;
    for (int pair_idx = 0; pair_idx < seq.size(); ++pair_idx) {
      if (seq[pair_idx]) {
        args.push_back(points[2*pair_idx  ]);
        args.push_back(points[2*pair_idx+1]);
      }
    }
    ret += sign * lido_base(args);
  }
  return ret.annotate(fmt::function(
    fmt::sub_num("LidoSymm", {weight}),
    mapped(points, [](X x){ return to_string(x); })
  ));
}

DeltaExpr LidoSymmVec(int weight, SpanX points) {
  return LidoSymm_wrapper<DeltaExpr>(weight, points.as_x(), identity_function);
}

ProjectionExpr LidoSymmVecPr(int weight, SpanX points, DeltaProjector projector) {
  return LidoSymm_wrapper<ProjectionExpr>(weight, points.as_x(), projector);
}
