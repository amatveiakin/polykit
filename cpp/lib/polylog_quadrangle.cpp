#include "polylog_quadrangle.h"

#include "algebra.h"
#include "mystic_algebra.h"
#include "sequence_iteration.h"
#include "util.h"


static LiraParam concat_format_symbols(const LiraParam& lhs, const LiraParam& rhs) {
  CHECK_EQ(lhs.foreweight(), 1) << to_string(lhs);
  CHECK_EQ(rhs.foreweight(), 1) << to_string(rhs);
  return LiraParam(
    1,
    concat(lhs.weights(), rhs.weights()),
    concat(lhs.ratios(), rhs.ratios())
  );
}

static LiraParam infuse_format_symbol(const LiraParam& lhs, const LiraParam& rhs) {
  CHECK_EQ(lhs.foreweight(), 1) << to_string(lhs);
  CHECK_EQ(rhs.foreweight(), 1) << to_string(rhs);
  CHECK_EQ(lhs.depth(), 1) << to_string(lhs);
  CHECK_GE(rhs.depth(), 1) << to_string(rhs);
  std::vector<int> weights = rhs.weights();
  std::vector<CompoundRatio> ratios = rhs.ratios();
  weights.front() += lhs.weights().front();
  ratios.front().add(lhs.ratios().front());
  return LiraParam(1, std::move(weights), std::move(ratios));
}


ThetaExpr LiQuadImpl(const std::vector<int>& points, bool sigma) {
  const int n = points.size();
  CHECK(n >= 4 && n % 2 == 0) << "Bad number of LiQuad points: " << n;
  auto ratio_from_points = [&](const std::vector<int>& points) {
    const auto& point_args = sigma ? rotated_vector(points, 1) : points;
    return CompoundRatio::from_cross_ratio(CrossRatio(point_args));
  };
  const int sigma_sign = sigma ? -1 : 1;
  if (n == 4) {
    return sigma_sign *
      TFormalSymbol(LiraParam(1, {1}, {ratio_from_points(points)}));
  }
  ThetaExpr ret;
  int last = n - 1;
  for (int a = 1; a < n; a += 2) {
    for (int b = a+1; b < n; b += 2) {
      const std::vector indices = {0, a, b, last};
      const LiraParam lower_part(1, {1},
        {ratio_from_points(choose_indices(points, {0, a, b, last}))});
      std::vector<ThetaExpr> upper_parts;
      if (a != 1) {
        upper_parts.push_back(LiQuadImpl(slice(points, 0, a+1), sigma));
      }
      if (b != a + 1) {
        upper_parts.push_back(LiQuadImpl(slice(points, a, b+1), !sigma));
      }
      if (last != b + 1) {
        upper_parts.push_back(LiQuadImpl(slice(points, b, last+1), sigma));
      }
      CHECK(!upper_parts.empty());
      const ThetaExpr upper_prod = theta_expr_quasi_shuffle_product(upper_parts);
      ret += upper_prod.mapped([&](const ThetaPack& expr) {
        return concat_format_symbols(lower_part, std::get<LiraParam>(expr));
      });
      if (sigma) {
        ret += upper_prod.mapped([&](const ThetaPack& expr) {
          return infuse_format_symbol(lower_part, std::get<LiraParam>(expr));
        });
      }
    }
  }
  return sigma_sign * ret;
}

ThetaExpr LiQuad(int foreweight, const std::vector<int>& points, LiFirstPoint first_point) {
  return update_foreweight(
    (
      neg_one_pow(div_int(points.size(), 2) + 1)  // sync with QLi and QLiNeg
      * LiQuadImpl(points, first_point == LiFirstPoint::even)
    ),
    foreweight
  ).annotate(
    fmt::function_num_args(
      fmt::lrsub_num(
        foreweight,
        fmt::super(fmt::opname("LiQuad"), {first_point == LiFirstPoint::odd ? "+" : "-"}),
        {}
      ),
      points
    )
  );
}
