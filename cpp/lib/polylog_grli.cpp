#include "polylog_grli.h"

#include "itertools.h"


template<typename Container>
int permutation_sign(Container v) {
  return sort_with_sign(v);
}


GammaExpr GrLiVec(const std::vector<int>& bonus_points, const std::vector<int>& main_points) {
  CHECK(main_points.size() % 2 == 0);
  const int weight = main_points.size() / 2;
  const auto original_sign = permutation_sign(main_points);
  GammaExpr ret;
  for (const auto& p : permutations(main_points)) {
    // TODO: Iterate permutations with sign more efficiently.
    //   Idea: does std::next_permutation guarantee that sign always changes?
    const int sign = permutation_sign(p) * original_sign;
    ret.add_to(mapped(range(weight), [&](const auto& k) {
      return Gamma(concat(bonus_points, slice(p, k, k + weight)));
    }), sign);
  }
  const std::string bonux_points_prefix = bonus_points.empty()
    ? std::string{}
    : absl::StrCat(str_join(bonus_points, ","), " / ");
  return ret.annotate(
    absl::StrCat(
      fmt::opname("GrLi"),
      fmt::parens(absl::StrCat(
        bonux_points_prefix,
        str_join(main_points, ",")
      ))
    )
  );
}
