#include "corr_expression.h"

#include "coalgebra.h"


CorrExpr corr_expr_substitute(const CorrExpr& expr, const std::vector<int>& new_points) {
  return expr.mapped([&](const CorrFSymb& term) {
    return CorrFSymb{mapped(term.points, [&](int p) {
      return new_points.at(p - 1);
    })};
  });
}


static CorrCoExpr normalize_corr_coproduct(const CorrCoExpr& expr) {
  return normalize_coproduct(expr)
    // .filtered([](const CoCorrFSymb& term) {
    //   for (const CorrFSymb& fsymb : term) {
    //     CHECK_GE(fsymb.points.size(), 2);
    //     if (fsymb.points.size() == 2) {  // weight == 1
    //       if (num_distinct_elements(fsymb.points) == 1) {
    //         return false;
    //       }
    //     } else {
    //       if (num_distinct_elements(fsymb.points) <= 2) {
    //         return false;
    //       }
    //     }
    //   }
    //   return true;
    // })
  ;
}

CorrCoExpr corr_coproduct(const CorrExpr& lhs, const CorrExpr& rhs) {
  return normalize_corr_coproduct(
    outer_product<CorrCoExpr>(
      lhs, rhs,
      [](const Word& u, const Word& v) {
        MultiWord prod;
        prod.append_segment(u);
        prod.append_segment(v);
        return prod;
      },
      AnnOperator(fmt::coprod_lie())
    )
  );
}

CorrCoExpr corr_comultiply(const CorrExpr& expr, std::pair<int, int> form) {
  CorrCoExpr ret;
  expr.foreach([&](const CorrFSymb& term, int coeff) {
    for (int shift = 0; shift < term.points.size(); ++shift) {
      const auto points_potated = rotated_vector(term.points, shift);
      const int n = term.points.size();
      for (int i = 1; i < n-1; ++i) {
        std::vector<int> lhs_incides = seq_incl(0, i);
        std::vector<int> rhs_incides = concat({0}, seq_incl(i+1, n-1));
        CorrFSymb lhs{choose_indices(points_potated, lhs_incides)};
        CorrFSymb rhs{choose_indices(points_potated, rhs_incides)};
        const int lhs_weight = lhs.points.size() - 1;
        const int rhs_weight = rhs.points.size() - 1;
        if ((lhs_weight == form.first && rhs_weight == form.second) ||
            (lhs_weight == form.second && rhs_weight == form.first)) {
          ret.add_to(CoCorrFSymb{lhs, rhs}, coeff);
        }
      }
    }
  });
  CHECK_EQ(expr.annotations().expression.size(), 1);  // HACK
  return normalize_corr_coproduct(ret).annotate(
    fmt::comult() + expr.annotations().expression.element().first
  );
}
