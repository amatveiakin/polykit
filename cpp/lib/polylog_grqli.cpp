#include "polylog_grqli.h"

#include "polylog_qli.h"


GammaExpr GrLogVec(const std::vector<int>& bonus_points, const std::vector<int>& log_points) {
  return pullback(
    LogVec(log_points),
    bonus_points
  ).without_annotations().annotate(
    absl::StrCat(
      fmt::opname("GrLog"),
      fmt::parens(str_join_skip_empty(
        std::array{
          str_join(bonus_points, ","),
          str_join(log_points, ",")
        },
        " / "
      ))
    )
  );
}

GammaExpr GrLogVec(const std::pair<std::vector<int>, std::vector<int>>& points) {
  return GrLogVec(points.first, points.second);
}

GammaExpr GrQLiVec(int weight, const std::vector<int>& bonus_points, const std::vector<int>& qli_points) {
  return pullback(
    QLiVec(weight, qli_points),
    bonus_points
  ).without_annotations().annotate(
    absl::StrCat(
      fmt::sub_num(fmt::opname("GrQLi"), {weight}),
      fmt::parens(str_join_skip_empty(
        std::array{
          str_join(bonus_points, ","),
          str_join(qli_points, ",")
        },
        " / "
      ))
    )
  );
}

GammaExpr GrQLiVec(int weight, const std::pair<std::vector<int>, std::vector<int>>& points) {
  return GrQLiVec(weight, points.first, points.second);
}
