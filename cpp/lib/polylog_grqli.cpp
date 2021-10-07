#include "polylog_grqli.h"

#include "polylog_qli.h"


GammaExpr GrQLiVec(int weight, const XArgs& bonus_points, const XArgs& qli_points) {
  CHECK(!contains_naive(bonus_points.as_x(), Inf)) << "GrQLi tautologically zero";
  const auto qli = QLiVec(weight, qli_points);
  return qli.mapped<GammaExpr>([&](const auto& term) {
    return mapped(term, [&](const auto& d) {
      Gamma g(concat(bonus_points.as_x(), {d.a(), d.b()}));
      CHECK(!g.is_nil());
      return g;
    });
  }).without_annotations().annotate(
    absl::StrCat(
      fmt::sub_num(fmt::opname("GrQLi"), {weight}),
      fmt::parens(str_join_skip_empty(
        std::array{
          str_join(bonus_points.as_x(), ","),
          str_join(qli_points.as_x(), ",")
        },
        " / "
      ))
    )
  );
}
