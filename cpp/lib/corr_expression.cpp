#include "corr_expression.h"

#include "coalgebra.h"
#include "lexicographical.h"


CorrFSymb::CorrFSymb(std::vector<int> points_arg)
  : points(lexicographically_minimal_rotation(points_arg)) {}


CorrExpr substitute_variables(const CorrExpr& expr, const std::vector<int>& new_points) {
  return expr.mapped([&](const CorrFSymb& term) {
    return CorrFSymb{mapped(term.points, [&](int p) {
      return new_points.at(p - 1);
    })};
  });
}


static CorrCoExpr normalize_corr_coproduct(const CorrCoExpr& expr) {
  return internal::normalize_coproduct(expr)
    // .filtered([](const CoCorrFSymb& term) {
    //   for (const CorrFSymb& fsymb : term) {
    //     CHECK_GE(fsymb.points.size(), 2);
    //     if (fsymb.points.size() == 2) {  // weight == 1
    //       if (num_distinct_elements_unsorted(fsymb.points) == 1) {
    //         return false;
    //       }
    //     } else {
    //       if (num_distinct_elements_unsorted(fsymb.points) <= 2) {
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
      [](const CorrExpr::StorageT& u, const CorrExpr::StorageT& v) {
        return CorrCoExpr::StorageT{u, v};
      },
      AnnOperator(fmt::coprod_iterated())
    )
  );
}

CorrCoExpr corr_comultiply(const CorrExpr& expr, std::pair<int, int> form) {
  CorrCoExpr ret;
  expr.foreach([&](const CorrFSymb& term, int coeff) {
    for (int shift : range(term.points.size())) {
      const auto points_potated = rotated_vector(term.points, shift);
      const int n = term.points.size();
      for (int i : range(1, n-1)) {
        std::vector<int> lhs_incides = seq_incl(0, i);
        std::vector<int> rhs_incides = concat({0}, seq(i+1, n));
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
  return normalize_corr_coproduct(ret).copy_annotations_mapped(
    expr, [](const std::string& annotation) {
      return fmt::comult() + annotation;
    }
  );
}
