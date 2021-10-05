#include "lib/compare.h"
#include "lib/gamma.h"


struct GammaACoExprParam : GammaCoExpr::Param {
  static bool lyndon_compare(const VectorT::value_type& lhs, const VectorT::value_type& rhs) {
    using namespace cmp;
    return projected(lhs, rhs, [](const auto& v) {
      return std::tuple{desc_val(v.size()), asc_ref(v)};
    });
  };
};

using GammaACoExpr = Linear<GammaACoExprParam>;

using GrPolylogACoSpace = std::vector<GammaACoExpr>;

// Converts each term
//     x1 * x2 * ... * xn
// into a sum
//   + (x1*x2) @ x3 @ ... @ xn
//   + x1 @ (x2*x3) @ ... @ xn
//     ...
//   + x1 @ x2 @ ... @ (x{n-1}*xn)
//
GammaACoExpr expand_into_glued_pairs(const GammaExpr& expr) {
  using CoExprT = GammaACoExpr;
  return expr.mapped_expanding([](const auto& term) {
    CoExprT expanded_expr;
    for (const int i : range(term.size() - 1)) {
      std::vector<GammaExpr> expanded_term;
      for (const int j : range(term.size() - 1)) {
        if (j < i) {
          expanded_term.push_back(GammaExpr::single({term[j]}));
        } else if (j == i) {
          expanded_term.push_back(GammaExpr::single({term[j], term[j+1]}));
        } else {
          expanded_term.push_back(GammaExpr::single({term[j+1]}));
        }
      }
      expanded_expr += internal::abstract_coproduct_vec<CoExprT>(expanded_term);
    }
    return expanded_expr;
  });
}
