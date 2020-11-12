#include "polylog_cross_ratio.h"

#include "absl/strings/str_cat.h"

#include "algebra.h"
#include "check.h"


struct Point {
  int idx = 0;
  bool odd = true;
};

static DeltaExpr Lido_4_point(const std::vector<Point>& p) {
  CHECK_EQ(p.size(), 4);
  return p[0].odd
    ?  neg_cross_ratio(p[0].idx, p[1].idx, p[2].idx, p[3].idx)
    : -neg_cross_ratio(p[1].idx, p[2].idx, p[3].idx, p[0].idx);
}

template<typename ResultT, typename ProjectorT>
static ResultT Lido_impl(int weight, const std::vector<Point>& points, const ProjectorT& projector) {
  const int num_points = points.size();
  CHECK(num_points >= 4 && num_points % 2 == 0) << "Bad number of Lido points: " << num_points;
  const int min_weight = (num_points - 2) / 2;
  CHECK_GE(weight, min_weight);
  const auto subsums = [&]() {
    ResultT ret;
    for (int i = 0; i < num_points - 3; ++i) {
      const auto foundation = concat(slice(points, 0, i+1), slice(points, i+3));
      ret += tensor_product(
        projector(Lido_4_point(slice(points, i, i+4))),
        Lido_impl<ResultT>(weight - 1, foundation, projector)
      );
    }
    return ret;
  };
  if (weight == min_weight) {
    if (num_points == 4) {
      return projector(Lido_4_point(points));
    } else {
      return subsums();
    }
  } else {
    ResultT ret = tensor_product(
      projector(cross_ratio(mapped(points, [](Point p) { return p.idx; }))),
      Lido_impl<ResultT>(weight - 1, points, projector)
    );
    if (num_points > 4) {
      ret += subsums();
    }
    return ret;
  }
}

template<typename ResultT, typename ProjectorT>
static ResultT Lido_wrapper(int weight, const std::vector<X>& points, const ProjectorT& projector) {
  const int num_points = points.size();
  std::vector<Point> tagged_points;
  for (int i = 0; i < points.size(); ++i) {
    tagged_points.push_back({points[i].var(), (i+1) % 2 == 1});
  }
  return Lido_impl<ResultT>(weight, tagged_points, projector)
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

WordExpr LidoVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return Lido_wrapper<WordExpr>(weight, points, projector);
}

template<typename ResultT, typename ProjectorT>
static ResultT LidoSymm_wrapper(int weight, const std::vector<X>& points, const ProjectorT& projector) {
  ResultT ret;
  if (points.size() == 6) {
    auto [x1, x2, x3, x4, x5, x6] = to_array<6>(points);
    ret =
      + Lido_wrapper<ResultT>(weight, {x1,x2,x3,x4,x5,x6}, projector)
      - Lido_wrapper<ResultT>(weight, {x1,x2,x3,x4}, projector)
      - Lido_wrapper<ResultT>(weight, {x3,x4,x5,x6}, projector)
      - Lido_wrapper<ResultT>(weight, {x5,x6,x1,x2}, projector)
    ;
  } else if (points.size() == 8) {
    auto [x1, x2, x3, x4, x5, x6, x7, x8] = to_array<8>(points);
    ret =
      + Lido_wrapper<ResultT>(weight, {x1,x2,x3,x4,x5,x6,x7,x8}, projector)
      - (
        + Lido_wrapper<ResultT>(weight, {x1,x2,x3,x4,x5,x6}, projector)
        + Lido_wrapper<ResultT>(weight, {x3,x4,x5,x6,x7,x8}, projector)
        + Lido_wrapper<ResultT>(weight, {x5,x6,x7,x8,x1,x2}, projector)
        + Lido_wrapper<ResultT>(weight, {x7,x8,x1,x2,x3,x4}, projector)
      )
      + (
        + Lido_wrapper<ResultT>(weight, {x1,x2,x3,x4}, projector)
        + Lido_wrapper<ResultT>(weight, {x3,x4,x5,x6}, projector)
        + Lido_wrapper<ResultT>(weight, {x5,x6,x7,x8}, projector)
        + Lido_wrapper<ResultT>(weight, {x7,x8,x1,x2}, projector)
      )
      + (
        + Lido_wrapper<ResultT>(weight, {x1,x2,x5,x6}, projector)
        + Lido_wrapper<ResultT>(weight, {x3,x4,x7,x8}, projector)
        + Lido_wrapper<ResultT>(weight, {x5,x6,x1,x2}, projector)
        + Lido_wrapper<ResultT>(weight, {x7,x8,x3,x4}, projector)
      ).dived_int(2)
    ;
  } else {
    FATAL(absl::StrCat("Unsupported number of arguments for LidoSymm: ", points.size()));
  }
  return ret.without_annotations().annotate(fmt::function(
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

WordExpr LidoSymmVecPr(
    int weight, const std::vector<X>& points,
    std::function<WordExpr(DeltaExpr)> projector) {
  return LidoSymm_wrapper<WordExpr>(weight, points, projector);
}
