#include "polylog_grqli.h"

#include "polylog_qli.h"


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
