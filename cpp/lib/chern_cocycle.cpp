#include "chern_cocycle.h"

#include "absl/strings/substitute.h"

#include "polylog_cgrli.h"


GammaExpr ChernCocycle(int weight, int dimension, const std::vector<int>& points) {
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
  } else {
    FATAL(absl::Substitute("Not implemented: ChernCocycle with weight=$0, dimension=$1", weight, dimension));
  }
  return ret;
}
