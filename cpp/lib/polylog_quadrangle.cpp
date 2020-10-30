#include "polylog_quadrangle.h"

#include "algebra.h"
#include "mystic_algebra.h"
#include "sequence_iteration.h"
#include "util.h"


static LiraParam concat_format_symbols(const LiraParam& lhs, const LiraParam& rhs) {
  return LiraParam(
    concat(lhs.weights(), rhs.weights()),
    concat(lhs.ratios(), rhs.ratios())
  );
}

static LiraParam infuse_format_symbol(const LiraParam& lhs, const LiraParam& rhs) {
  CHECK_EQ(lhs.depth(), 1);
  CHECK_GE(rhs.depth(), 1);
  std::vector<int> weights = rhs.weights();
  std::vector<CompoundRatio> ratios = rhs.ratios();
  weights.front() += lhs.weights().front();
  ratios.front().add(lhs.ratios().front());
  return LiraParam(std::move(weights), std::move(ratios));
}


ThetaExpr LiQuadImpl(const std::vector<int>& points, bool sigma) {
  const int n = points.size();
  CHECK(n >= 4 && n % 2 == 0) << "Bad number of LiQuad points: " << n;
  auto ratio_from_points = [&](const std::vector<int>& points) {
    return CompoundRatio::from_cross_ratio(CrossRatio(
      sigma ? rotated_vector(points, 1) : points));
  };
  if (n == 4) {
    return (sigma ? -1 : 1) *
      TFormalSymbolSigned(LiraParam({1}, {ratio_from_points(points)}));
  }
  ThetaExpr ret;
  int last = n - 1;
  for (int a = 1; a < n; a += 2) {
    for (int b = a+1; b < n; b += 2) {
      const std::vector indices = {0, a, b, last};
      const LiraParam lower_part({1},
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
      ret += lower_part.sign() * upper_prod.mapped<ThetaExpr>([&](const ThetaPack& expr) {
        return concat_format_symbols(lower_part, std::get<LiraParam>(expr));
      });
      if (sigma) {
        ret += lower_part.sign() * upper_prod.mapped<ThetaExpr>([&](const ThetaPack& expr) {
          return infuse_format_symbol(lower_part, std::get<LiraParam>(expr));
        });
      }
    }
  }
  return ret;
}

ThetaExpr LiQuad(const std::vector<int>& points) {
  return LiQuadImpl(points, false);
}
