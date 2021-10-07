#include "polylog_grqli.h"

#include "polylog_qli.h"


GammaExpr GrQLiVec(int weight, const std::vector<int>& bonus_points, const std::vector<int>& qli_points) {
  const auto qli = QLiVec(weight, qli_points);
  return qli.mapped<GammaExpr>([&](const auto& term) {
    return mapped(term, [&](const auto& d) {
      Gamma g(concat(bonus_points, {d.a().as_simple_var(), d.b().as_simple_var()}));
      CHECK(!g.is_nil());
      return g;
    });
  }).without_annotations().annotate(
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
