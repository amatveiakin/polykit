// Note: Cutting formulas are the same as in `iterated_integral`.
// Difference: Epsilon instead of Delta, `..._3_point` implementation.

#include "polylog_multiarg.h"

#include "absl/types/span.h"

#include "algebra.h"
#include "check.h"
#include "format.h"


// Points:
//    0 => 0
//   -1 => 1
//    k => x_1 * ... * x_{k-1} for k >= 1

static constexpr int kZero = 0;
static constexpr int kOne = -1;

static constexpr inline bool is_var (int index) { return index >=  1; }
static constexpr inline bool is_zero(int index) { return index == kZero; }
static constexpr inline bool is_one (int index) { return index == kOne; }

static EpsilonExpr EVarProd(int from, int to) {
  EpsilonExpr ret;
  for (int i = from; i <= to; ++i) {
    ret += EVar(i);
  }
  return ret;
}

static EpsilonExpr Lily_2_point_irreducible(int a, int b) {
  const int num_vars = is_var(a) + is_var(b);
  CHECK_LE(num_vars, 1);
  if (num_vars == 0) {
    return EpsilonExpr();
  }
  if (is_var(a)) {
    std::swap(a, b);
  }
  CHECK(!is_var(a) && is_var(b));
  if (is_zero(a)) {
    // x_1...x_b - 0
    return EVarProd(1, b);
  } else {
    CHECK(is_one(a));
    // x_1...x_b - 1
    return EMonsterRangeInclusive(1, b);
  }
}

// Computes symbol for (p[2] - p[1]) / (p[1] - p[0]).
static EpsilonExpr Lily_3_point(const absl::Span<const int>& p) {
  CHECK_EQ(p.size(), 3);
  int a = p[0];
  int b = p[1];
  int c = p[2];
  // Note. Point `b` is fixed. Points `a` and `c` can be swapped with
  // the change of result sign.
  const int num_vars = is_var(a) + is_var(b) + is_var(c);
  if (num_vars == 0) {
    return EpsilonExpr();
  } else if (num_vars == 1) {
    // If there is only one variable, the fraction is irreducible.
    return + Lily_2_point_irreducible(c, b)
           - Lily_2_point_irreducible(b, a);
  } else if (num_vars == 2) {
    if (!is_var(a)) {
      CHECK(!is_var(a) && is_var(b) && is_var(c));
      CHECK_LT(b, c);
      if (is_zero(a)) {
        // x_1...x_c - x_1...x_b
        // ----------------------  =  x_{b+1}...x_c - 1
        //     x_1...x_b - 0
        return EMonsterRangeInclusive(b+1, c);
      } else {
        CHECK(is_one(a));
        // x_1...x_c - x_1...x_b      x_1 * ... * x_b * (x_{b+1}...x_c - 1)
        // ----------------------  =  -------------------------------------
        //     x_1...x_b - 1                      x_1...x_b - 1
        return + EVarProd(1, b)
               + EMonsterRangeInclusive(b+1, c)
               - EMonsterRangeInclusive(1, b);
      }
    } else if (!is_var(b)) {
      CHECK(is_var(a) && !is_var(b) && is_var(c));
      CHECK_LT(a, c);
      if (is_zero(b)) {
        // x_1...x_c - 0
        // -------------  =  x_{a+1} * ... * x_c
        // 0 - x_1...x_a
        return EVarProd(a+1, c);
      } else {
        CHECK(is_one(b));
        // x_1...x_c - 1
        // -------------
        // 1 - x_1...x_a
        return + EMonsterRangeInclusive(1, c)
               - EMonsterRangeInclusive(1, a);
      }
    } else {
      CHECK(is_var(a) && is_var(b) && !is_var(c));
      CHECK_LT(a, b);
      CHECK(is_zero(c));
      //     0 - x_1...x_b          x_{a+1} * ... * x_b
      // ----------------------  =  -------------------
      // x_1...x_b - x_1...x_a       x_{a+1}...x_b - 1
      return + EVarProd(a+1, b)
             - EMonsterRangeInclusive(a+1, b);
    }
  } else if (num_vars == 3) {
    // x_1...x_c - x_1...x_b      x_{a+1} * ... * x_b * (x_{b+1}...x_c - 1)
    // ----------------------  =  -----------------------------------------
    // x_1...x_b - x_1...x_a                  x_{a+1}...x_b - 1
    return + EVarProd(a+1, b)
           + EMonsterRangeInclusive(b+1, c)
           - EMonsterRangeInclusive(a+1, b);
  } else {
    FAIL(absl::StrCat("Bad num_vars: ", num_vars));
  }
}

// TODO: Add cache
EpsilonExpr Lily_impl(const std::vector<int>& points) {
  const int num_points = points.size();
  CHECK_GE(num_points, 3);
  EpsilonExpr ret;
  if (num_points == 3) {
    ret = Lily_3_point(absl::MakeConstSpan(points));
  } else {
    for (int i = 0; i <= num_points - 3; ++i) {
      ret += tensor_product(
        Lily_3_point(absl::MakeConstSpan(points).subspan(i, 3)),
        Lily_impl(removed_index(points, i+1))
      );
    }
  }
  return ret;
}


EpsilonExpr LilyVec(
    const std::vector<int>& weights,
    const std::vector<std::vector<int>>& points) {
  CHECK_EQ(weights.size(), points.size());
  std::vector<int> args{kZero, kOne};
  for (int i = 0; i < weights.size(); ++i) {
    const int w = weights[i];
    const int p = i + 1;
    CHECK_GE(w, 1);
    CHECK_GE(p, 1);
    for (int j = 1; j < w; ++j) {
      args.push_back(kZero);
    }
    args.push_back(p);
  }
  return epsilon_expr_substitute(Lily_impl(args), points).
      annotate_with_function(
        "Li_" + str_join(weights, "_"),
        mapped(points, [](const std::vector<int>& prod){
          return str_join(prod, "", [](int v) { return absl::StrCat("x", v); });
        }));
}
