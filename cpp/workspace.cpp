// Optimization potential. To improve compilation time:
//   - Add DECLARE_EXPR/DEFINE_EXPR macros, move lyndon and co-product to .cpp;
//   - Add a way to disable co-products and corresponding expressions.

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/integer_math.h"
#include "lib/itertools.h"
#include "lib/linalg.h"
#include "lib/linalg_solvers.h"
#include "lib/lyndon.h"
#include "lib/polylog_qli.h"
#include "lib/polylog_type_ac_space.h"
#include "lib/polylog_type_c_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/space_algebra.h"
#include "lib/summation.h"
#include "lib/zip.h"

// In order to reduce compilation time enable expressions only when necessary:

#if 1
#include "lib/bigrassmannian_complex_cohomologies.h"
#include "lib/gamma.h"
#include "lib/chern_arrow.h"
#include "lib/chern_cocycle.h"
#include "lib/polylog_gli.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_gr_space.h"
#elif defined(HAS_GAMMA_EXPR)
#  error "Expression type leaked: check header structure"
#endif

#if 0
#include "lib/kappa.h"
#include "lib/polylog_type_d_space.h"
#elif defined(HAS_KAPPA_EXPR)
#  error "Expression type leaked: check header structure"
#endif

#if 0
#include "lib/corr_expression.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_via_correlators.h"
#elif defined(HAS_CORR_EXPR)
#  error "Expression type leaked: check header structure"
#endif

#if 0
#include "lib/epsilon.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/loops_aux.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/snowpal.h"
#include "lib/theta.h"
#elif defined(HAS_EPSILON_EXPR) || defined(HAS_THETA_EXPR) || defined(HAS_LIRA_EXPR)
#  error "Expression type leaked: check header structure"
#endif


int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_unicode_version(UnicodeVersion::simple)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_expression_line_limit(30)
    // .set_annotation_sorting(AnnotationSorting::length)
    .set_annotation_sorting(AnnotationSorting::lexicographic)
    .set_compact_x(true)
    .set_max_terms_in_annotations_one_liner(100)
  );


  std::vector exprs_odd_num_points = {
    GLi2[{7}](1,2,3,4,5,6),
    GrQLi2(7)(1,2,3,4,5,6),
    GrLi(5)(1,2,3,4),
    G({1,2,3,4,5}),
    tensor_product(G({1,2,3}), G({3,4,5})),
  };
  std::vector exprs_even_num_points = {
    GLi2(1,2,3,4,5,6),
    GrQLi2()(1,2,3,4,5,6),
    GrLi(5,6)(1,2,3,4),
    G({1,2,3,4,5,6}),
    tensor_product(G({1,2,3,4}), G({3,4,5,6})),
  };
  std::vector exprs_any_num_points = concat(exprs_odd_num_points, exprs_even_num_points);

  const auto ab_functions = list_nco_ab_function(ABDoublePlusMinus::Include);
  const std::vector c_functions = {
    c_minus,
    c_plus,
  };

  // std::vector<std::string> zero_eqns;
  // for (const auto& ab : ab_functions) {
  //   for (const auto& c : c_functions) {
  //     for (const bool c_inside : {false, true}) {
  //       const auto make_eqn = [&](const auto& x, const int n) {
  //         return c_inside
  //           ? ab(c(x, n), n+1)
  //           : c(ab(x, n+1), n+1)
  //         ;
  //       };
  //       bool eqn_holds = true;
  //       for (const auto& expr : exprs_any_num_points) {
  //         const int n = detect_num_variables(expr);
  //         const auto eqn = make_eqn(ncoproduct(expr), n);
  //         if (!eqn.is_zero()) {
  //           eqn_holds = false;
  //           break;
  //         }
  //       }
  //       if (eqn_holds) {
  //         const auto x = GammaNCoExpr().annotate("x");
  //         const auto eqn = make_eqn(x, 0);
  //         std::cout << annotations_one_liner(eqn.annotations()) << " == 0";
  //       }
  //     }
  //   }
  // }

  for (const int l_ab_idx : range(ab_functions.size())) {
    for (const int l_c_idx : range(c_functions.size())) {
      for (const bool l_c_inside : {false, true}) {
        for (const int r_ab_idx : range(ab_functions.size())) {
          for (const int r_c_idx : range(c_functions.size())) {
            for (const bool r_c_inside : {false, true}) {
              const bool is_trivial =
                std::tie(l_ab_idx, l_c_idx, l_c_inside) <= std::tie(r_ab_idx, r_c_idx, r_c_inside)
              ;
              if (is_trivial) {
                continue;
              }
              const auto l_ab = ab_functions.at(l_ab_idx);
              const auto l_c = c_functions.at(l_c_idx);
              const auto r_ab = ab_functions.at(r_ab_idx);
              const auto r_c = c_functions.at(r_c_idx);
              for (const int sign : {-1, 1}) {
                const auto make_eqn = [&](const auto& x, const int n) {
                  const auto lhs = l_c_inside
                    ? l_ab(l_c(x, n), n+1)
                    : l_c(l_ab(x, n+1), n+1)
                  ;
                  const auto rhs = r_c_inside
                    ? r_ab(r_c(x, n), n+1)
                    : r_c(r_ab(x, n+1), n+1)
                  ;
                  return lhs + sign * rhs;
                };
                bool eqn_holds = true;
                for (const auto& expr : exprs_any_num_points) {
                  const int n = detect_num_variables(expr);
                  const auto eqn = make_eqn(ncoproduct(expr), n);
                  if (!eqn.is_zero()) {
                    eqn_holds = false;
                    break;
                  }
                }
                if (eqn_holds) {
                  const auto x = GammaNCoExpr().annotate("x");
                  const auto eqn = make_eqn(x, 0);
                  std::cout << annotations_one_liner(eqn.annotations()) << " == 0\n";
                }
              }
            }
          }
        }
      }
    }
  }
}
