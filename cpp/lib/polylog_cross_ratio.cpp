#include "polylog_cross_ratio.h"

#include "absl/strings/str_cat.h"

#include "algebra.h"
#include "check.h"


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

DeltaExpr LidoVec(int weight, const std::vector<X>& points) {
  return Lido_wrapper<DeltaExpr>(weight, points, identity_function);
}

DeltaExpr LidoVec(int weight, const std::vector<int>& points) {
  return LidoVec(weight, mapped(points, X::Var));
}

DeltaExpr LidoVec(int weight, std::initializer_list<int> points) {
  return LidoVec(weight, std::vector(points));
}

WordExpr LidoVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return Lido_wrapper<WordExpr>(weight, points, projector);
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

DeltaExpr LidoNegVec(int weight, const std::vector<X>& points) {
  return LidoNeg_wrapper<DeltaExpr>(weight, points, identity_function);
}

DeltaExpr LidoNegVec(int weight, const std::vector<int>& points) {
  return LidoNegVec(weight, mapped(points, X::Var));
}

DeltaExpr LidoNegVec(int weight, std::initializer_list<int> points) {
  return LidoNegVec(weight, std::vector(points));
}

WordExpr LidoNegVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return LidoNeg_wrapper<WordExpr>(weight, points, projector);
}


template<typename ResultT, typename ProjectorT>
static ResultT LidoSymm_wrapper(int weight, const std::vector<X>& points, const ProjectorT& projector) {
  auto lido_base = [&](const std::vector<X>& args) {
    return Lido_generic_wrapper<ResultT>(weight, args, lido_pos_node_func, projector);
  };
  ResultT ret;
  if (points.size() == 6) {
    auto [x1, x2, x3, x4, x5, x6] = to_array<6>(points);
    ret =
      + lido_base({x1,x2,x3,x4,x5,x6})
      - lido_base({x1,x2,x3,x4})
      - lido_base({x3,x4,x5,x6})
      - lido_base({x5,x6,x1,x2})
    ;
  } else if (points.size() == 8) {
    auto [x1, x2, x3, x4, x5, x6, x7, x8] = to_array<8>(points);
    ret =
      + lido_base({x1,x2,x3,x4,x5,x6,x7,x8})
      - (
        + lido_base({x1,x2,x3,x4,x5,x6})
        + lido_base({x3,x4,x5,x6,x7,x8})
        + lido_base({x5,x6,x7,x8,x1,x2})
        + lido_base({x7,x8,x1,x2,x3,x4})
      )
      + (
        + lido_base({x1,x2,x3,x4})
        + lido_base({x3,x4,x5,x6})
        + lido_base({x5,x6,x7,x8})
        + lido_base({x7,x8,x1,x2})
      )
      + (
        + lido_base({x1,x2,x5,x6})
        + lido_base({x3,x4,x7,x8})
        + lido_base({x5,x6,x1,x2})
        + lido_base({x7,x8,x3,x4})
      ).dived_int(2)
    ;
  } else {
    FATAL(absl::StrCat("Unsupported number of arguments for LidoSymm: ", points.size()));
  }
  return ret.annotate(fmt::function(
    fmt::sub_num("LidoSymm", {weight}),
    mapped(points, [](X x){ return to_string(x); })
  ));
}

DeltaExpr LidoSymmVec(int weight, const std::vector<X>& points) {
  return LidoSymm_wrapper<DeltaExpr>(weight, points, identity_function);
}

DeltaExpr LidoSymmVec(int weight, const std::vector<int>& points) {
  return LidoSymmVec(weight, mapped(points, X::Var));
}

DeltaExpr LidoSymmVec(int weight, std::initializer_list<int> points) {
  return LidoSymmVec(weight, std::vector(points));
}

WordExpr LidoSymmVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return LidoSymm_wrapper<WordExpr>(weight, points, projector);
}
