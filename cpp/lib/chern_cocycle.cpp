// TODO: Should the signs of ChernCocycle_Dim1 and ChernCocycle_3_2_5 be adjusted after
//   changing the signs of cocycles in weight n, dimension n (or n-1), 2n points at odd n?

#include "chern_cocycle.h"

#include "absl/strings/substitute.h"

#include "chern_arrow.h"
#include "polylog_gli.h"


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

GammaNCoExpr ChernCocycle_3_2_5(const std::vector<int>& points) {
  const int weight = 3;
  CHECK_EQ(points.size(), 5);
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  return
    - ncoproduct(GLiVec(weight-1, args({1,2,3,5})), G(args({1,2})))
    - ncoproduct(GLiVec(weight-1, args({1,2,3,5})), G(args({3,5})))
    - ncoproduct(GLiVec(weight-1, args({1,3,4,5})), G(args({1,5})))
    - ncoproduct(GLiVec(weight-1, args({1,3,4,5})), G(args({3,4})))
    + ncoproduct(GLiVec(weight-1, args({1,2,3,4})), G(args({1,2})))
    + ncoproduct(GLiVec(weight-1, args({1,2,3,4})), G(args({3,4})))
    + ncoproduct(GLiVec(weight-1, args({1,2,4,5})), G(args({1,2})))
    + ncoproduct(GLiVec(weight-1, args({1,2,4,5})), G(args({4,5})))
    + ncoproduct(GLiVec(weight-1, args({2,3,4,5})), G(args({2,5})))
    + ncoproduct(GLiVec(weight-1, args({2,3,4,5})), G(args({3,4})))
  ;
}

GammaNCoExpr ChernCocycle_impl(int weight, int dimension, const std::vector<int>& points) {
  if (dimension == 1 && points.size() == weight + 1) {
    return ChernCocycle_Dim1(weight, points);
  }
  if (weight == 3 && dimension == 2 && points.size() == 5) {
    return ChernCocycle_3_2_5(points);
  }
  CHECK_EQ(weight * 2, points.size()) << "Not implemented";
  CHECK(points.size() % 2 == 0);
  if (dimension == weight) {
    const int n = weight;
    return ncoproduct(substitute_variables_1_based(
      + GLiVec(n, seq_incl(1, 2*n))
      + neg_one_pow(n) * b_plus(a_minus_minus(GLiVec(n, seq_incl(1, 2*n-2)), 2*n-1), 2*n),
      points
    ));
  } else if (dimension == weight - 1) {
    const int n = weight;
    return ncoproduct(substitute_variables_1_based(
      neg_one_pow(n) * a_plus(a_minus_minus(GLiVec(n, seq_incl(1, 2*n-2)), 2*n-1), 2*n),
      points
    ));
  } else {
    FATAL(absl::Substitute("Not implemented: ChernCocycle with weight=$0, dimension=$1", weight, dimension));
  }
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
