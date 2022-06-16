#include "chern_cocycle.h"

#include "absl/strings/substitute.h"

#include "polylog_cgrli.h"


GammaNCoExpr ChernCocycle_Dim1(int weight, const std::vector<int>& points) {
  CHECK_EQ(points.size(), weight + 1);
  GammaNCoExpr ret;
  for (const int p : range(points.size())) {
    std::vector<GammaExpr> components;
    for (const int q : range(points.size())) {
      if (q != p) {
        components.push_back(G(choose_indices(points, {p, q})));
      }
    }
    const int sign = neg_one_pow(p);
    ret += sign * ncoproduct_vec(components);
  }
  return ret;
}

GammaNCoExpr ChernCocycle_3_2(const std::vector<int>& points) {
  const int weight = 3;
  CHECK_EQ(points.size(), 5);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return
    - ncoproduct(CGrLiVec(weight-1, args({1,2,3,5})), G(args({1,2})))
    - ncoproduct(CGrLiVec(weight-1, args({1,2,3,5})), G(args({3,5})))
    - ncoproduct(CGrLiVec(weight-1, args({1,3,4,5})), G(args({1,5})))
    - ncoproduct(CGrLiVec(weight-1, args({1,3,4,5})), G(args({3,4})))
    + ncoproduct(CGrLiVec(weight-1, args({1,2,3,4})), G(args({1,2})))
    + ncoproduct(CGrLiVec(weight-1, args({1,2,3,4})), G(args({3,4})))
    + ncoproduct(CGrLiVec(weight-1, args({1,2,4,5})), G(args({1,2})))
    + ncoproduct(CGrLiVec(weight-1, args({1,2,4,5})), G(args({4,5})))
    + ncoproduct(CGrLiVec(weight-1, args({2,3,4,5})), G(args({2,5})))
    + ncoproduct(CGrLiVec(weight-1, args({2,3,4,5})), G(args({3,4})))
  ;
}

GammaNCoExpr ChernCocycle_impl(int weight, int dimension, const std::vector<int>& points) {
  if (dimension == 1 && points.size() == weight + 1) {
    return ChernCocycle_Dim1(weight, points);
  }
  if (weight == 3 && dimension == 2 && points.size() == 5) {
    return ChernCocycle_3_2(points);
  }
  CHECK_EQ(weight * 2, points.size()) << "Not implemented";
  CHECK(points.size() % 2 == 0);
  const int mid = points.size() / 2 - 1;
  GammaExpr ret;
  if (dimension == weight) {
    ret += CGrLiVec(weight, points);
    for (const int before : range(mid)) {
      for (const int after : range(mid + 1, points.size())) {
        const int sign = neg_one_pow(before + after + weight + 1);
        ret += sign * pullback(
          CGrLiVec(weight, removed_indices(points, {before, after})),
          choose_indices(points, {before})
        );
      }
    }
    ret = plucker_dual(ret, points);
  } else if (dimension == weight - 1) {
    for (const int before : range(mid)) {
      for (const int after : range(mid + 1, points.size())) {
        const int sign = neg_one_pow(before + after + weight + 1);
        ret += sign * CGrLiVec(weight, removed_indices(points, {before, after}));
      }
    }
    ret *= neg_one_pow(weight + 1);
  } else {
    FATAL(absl::Substitute("Not implemented: ChernCocycle with weight=$0, dimension=$1", weight, dimension));
  }
  return ncoproduct(ret);
}

// TODO: How does `dimension` parameter in ChernCocycle relate to Grassmannian dimension?
GammaNCoExpr ChernCocycle(int weight, int dimension, const std::vector<int>& points) {
  return ChernCocycle_impl(weight, dimension, points).without_annotations().annotate(
    fmt::function_num_args(
      fmt::function_num_args(
        fmt::sub_num(fmt::opname("ChernCocycle"), {weight}),
        {dimension}
      ),
      points
    )
  );
}
