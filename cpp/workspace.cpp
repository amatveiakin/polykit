#include <iostream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/summation.h"
#include "lib/util.h"


void init(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});  // show stack trace on crash
}

FormattingConfig formatting_config() {
  return FormattingConfig()
    .set_encoder(Encoder::ascii)       // use only ASCII symbols; the most universal option
    // .set_encoder(Encoder::unicode)  // unicode output looks nicer, but may be problematic for some terminals
    // .set_encoder(Encoder::latex)    // use LaTeX formatting
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)  // show full expressions
    .set_annotation_sorting(AnnotationSorting::length)
  ;
}


// Prints QLi symbols.
void qli_symbol() {
  auto expr = QLi3(1,2,3,4,5,6);
  auto lyndon = to_lyndon_basis(expr);
  std::cout << "Raw " << expr;
  std::cout << "In Lyndon basis " << lyndon;
  std::cout << "Most complex terms " << terms_with_min_distinct_variables(lyndon, 4);
  std::cout << "Projection on x1 " << project_on_x1(expr);
  // Note. Projection can be computed directly: `QLiVecPr(3, {1,2,3,4,5,6}, project_on_x1)`.
  // This is faster, so it is useful for large weight / number of arguments.
  std::cout << "With duplicates and infinity " << QLi3(1,2,3,4,3,Inf);
}

// Verifies that QLiSymm is indeed symmetric.
void check_qlisymm_is_symmetric() {
  // Both should be zero.
  std::cout << to_lyndon_basis(QLiSymm2(1,2,3,4,5,6) + QLiSymm2(2,3,4,5,6,1));
  std::cout << to_lyndon_basis(QLiSymm3(1,2,3,4,5,6) - QLiSymm3(2,3,4,5,6,1));
}

// Verifies equation:
//                  QLiSymm3(1,2,3,4,5,6,7,8) −
//   − ∑ (−1)^(x1+x2+x3+x4+x5+x6) QLiSymm3(x1,x2,x3,x4,x5,x6) +
//         + ∑ (−1)^(x1+x2+x3+x4) QLiSymm3(x1,x2,x3,x4) =
//                              = 0
void check_qlisymm_subset_equation() {
  const int total_points = 8;
  const int weight = 3;
  DeltaExpr expr;
  for (int num_args = 4; num_args <= total_points; num_args += 2) {
    for (const auto& seq : increasing_sequences(total_points, num_args)) {
      const auto args = mapped(seq, [](int x) { return x + 1; });
      const int sign = neg_one_pow(sum(args) + num_args / 2);
      expr += sign * QLiSymmVec(weight, args);
    }
  }
  // Should be zero.
  std::cout << to_lyndon_basis(expr);
}

// Prints iterated integral and correlator symbols.
void iterated_intergral_and_correlator() {
  std::cout << I(1,2,3,4,5,6);
  std::cout << Corr(1,2,3,4,5,6);
}

// Verifies that our formulas for QLi via correlators work (will also show the formula).
void qli_symm_via_corr() {
  // Should be zero.
  std::cout << to_lyndon_basis(
    + QLi3(1,2,3,4,5,6)
    - eval_formal_symbols(CorrQLi(3, {1,2,3,4,5,6}))
  );
}

// Computes QLi comultiplication.
void qli_comultiplication() {
  auto expr = to_lyndon_basis(QLi3(1,2,3,4,5,6));
  std::cout << expr;
  std::cout << comultiply(expr, {1,2});
}

// Computes ₁Li₁₁₁(x1,x2,x3)
void li_symbol() {
  std::cout << Li(1,1,1)({1},{2},{3});
}

// Verifies a quasi-shuffle relation for Li.
void li_quasisuffle_formula() {
  auto lhs =
    + Li(1,1,2)({1},{2},{3})
    + Li(1,2,1)({1},{3},{2})
    + Li(2,1,1)({3},{1},{2})
    + Li(1,3)  ({1},{2,3})
    + Li(3,1)  ({1,3},{2})
  ;
  auto rhs = shuffle_product_expr(
    Li(1,1)({1},{2}),
    Li(2)  ({3})
  );
  // These should be equal.
  std::cout << lhs - rhs;
  // In Lyndon basis this is just zero.
  std::cout << to_lyndon_basis(lhs);
}

// Computes CoLi comultiplication with a formal symbol of the left side.
void li_comultiplication() {
  std::cout << CoLi(1,1,1)({1},{2},{3});
  // Note. `comultiply(Li(...))` wouldn't work because `comultiply` only supports Lie algebras.
}

// Computes a symbol for Li with cross-ratios substituted into it (a.k.a. "Lira").
void lira_symbol() {
  // A shortcut for `substitute_ratios(Li(1,1)({1},{2}), {CR(1,2,3,6), CR(3,4,5,6)})`
  std::cout << Lira(1,1)(CR(1,2,3,6), CR(3,4,5,6));
}

// Writes a sum of Li formal symbols obtained by cutting a octagon into quadrangles.
void liquad_symbol() {
  std::cout << LiQuad(1, {1,2,3,4,5,6,7,8});
}

// Theorem 1.2 (Formula for quadrangular polylogarithms via multiple polylogarithms)
// from https://arxiv.org/pdf/2012.05599.pdf, i.e. check that LiQuad is equivalent to QLi.
void liquad_is_equivalent_to_li() {
  // Generate a sum of Li formal symbols.
  auto liquad_expr = LiQuad(1, {1,2,3,4,5,6,7,8});
  // Expand each formal symbol replacing it with a symbol for the corresponding Li function.
  auto liquad_expr_evaluated = eval_formal_symbols(liquad_expr);
  // Change expression type. Nothing interesting happens here, just a type convertion.
  // Note however that this would fail if the expression contains a "monsters" (an irreducible
  // differences of one and cross-ratio product).
  auto liquad_expr_converted = theta_expr_to_delta_expr(liquad_expr_evaluated);
  // Now take a good old QLi.
  auto li_expr = QLi3(1,2,3,4,5,6,7,8);
  // And... ta-dam! They are equal.
  std::cout << to_lyndon_basis(liquad_expr_converted - li_expr);
}


int main(int argc, char *argv[]) {
  init(argc, argv);
  ScopedFormatting sf(formatting_config());

  std::cout << "Hey! You managed to set up PolyKit, congrats :)\n";
  std::cout << "Navigate to workspace.cpp to see some examples.\n";

  // Examples. Uncomment to run.

  // | Functions that return a "DeltaExpr" - a linear expression where each term
  // | is a tensor product of residuals of two variables. See delta.h for details.
  // qli_symbol();
  // check_qlisymm_is_symmetric();
  // check_qlisymm_subset_equation();
  // iterated_intergral_and_correlator();
  // qli_symm_via_corr();
  // qli_comultiplication();

  // | Functions that return an "EpsilonExpr" - a linear expression where each term
  // | is a tensor product of x_i or (1 - x_{j_1}*...*x_{j_m}), or a formal symbol.
  // | See epsilon.h for details.
  // li_symbol();
  // li_quasisuffle_formula();
  // li_comultiplication();

  // | Functions that return an "ThetaExpr" - a linear expression where each term
  // | is a tensor product of differences (x_i - x_j) or (1 - <cross-ratio-product>),
  // | or a formal symbol. See theta.h for details.
  // lira_symbol();
  // liquad_symbol();
  // liquad_is_equivalent_to_li();
}
