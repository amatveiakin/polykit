// Optimization potential. To improve compilation time:
//   - Add DECLARE_EXPR/DEFINE_EXPR macros, move lyndon and co-product to .cpp;
//   - Add a way to disable co-products and corresponding expressions.

#include <fstream>
#include <regex>

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
    .set_expression_line_limit(100)
    // .set_annotation_sorting(AnnotationSorting::length)
    .set_annotation_sorting(AnnotationSorting::lexicographic)
    .set_compact_x(true)
    .set_max_terms_in_annotations_one_liner(100)
  );

  // CHECK_EQ(argc, 2);
  // const int app_arg = atoi(argv[1]);


  // const int num_points = 7;
  // const int dimension = 3;
  // const auto points = seq_incl(1, num_points);
  // const auto fx = space_ncoproduct(GrFx(dimension, points));
  // const auto fx_prime = space_ncoproduct(GrFxPrime(dimension, points));
  // const auto l2 = space_ncoproduct(GrL2(dimension, points));
  // for (const int weight : range_incl(2, 10)) {
  //   Profiler profiler(true);
  //   const auto space_a = mapped_parallel(combinations(fx, weight), DISAMBIGUATE(ncoproduct));
  //   const auto space_b = mapped_parallel(cartesian_combinations(std::vector{
  //     std::pair{l2, 1},
  //     std::pair{fx, weight - 2},
  //   }), [](const auto& exprs) {
  //     return ncomultiply(ncoproduct(exprs));
  //   });
  //   profiler.finish("spaces");
  //   const auto ranks = space_venn_ranks(space_a, space_b, identity_function);
  //   profiler.finish("ranks");
  //   std::cout << "w=" << weight << ": " << to_string(ranks) << "\n";
  // }

  // const auto fx = space_ncoproduct(GrFx(dimension, points));
  // const auto l2 = space_ncoproduct(GrL2(dimension, points));
  // const int weight = 5;
  // const int num_prime = app_arg;
  // // for (const int num_prime : range_incl(0, weight)) {
  //   Profiler profiler(false);
  //   const auto space_a = mapped_parallel(cartesian_combinations(std::vector{
  //     std::pair{fx, weight - num_prime},
  //     std::pair{fx_prime, num_prime},
  //   }), DISAMBIGUATE(ncoproduct));
  //   const auto space_b = mapped_parallel(cartesian_combinations(std::vector{
  //     std::pair{l2, 1},
  //     std::pair{fx, weight - 2},
  //   }), [](const auto& exprs) {
  //     return ncomultiply(ncoproduct(exprs));
  //   });
  //   profiler.finish("spaces");
  //   const auto ranks = space_venn_ranks(space_a, space_b, identity_function);
  //   profiler.finish("ranks");
  //   std::cout << "w=" << weight << ", #f'=" << num_prime << ": " << to_string(ranks) << "\n";
  // // }


  // // Generalized Arnold's relationship.
  // const int num_points = 5;
  // const int dimension = 3;
  // const auto points = seq_incl(1, num_points);
  // const auto fx = GrFx(dimension, points);
  // const auto l2_prime = mapped(
  //   to_vector(permutations(points)),
  //   [](const auto& args) {
  //     const auto pl = [&](const std::vector<int>& subargs) {
  //       return plucker(choose_indices_one_based(args, subargs));
  //     };
  //     return
  //       + ncoproduct(pl({1,2,3}), pl({1,2,4}))
  //       + ncoproduct(pl({1,2,4}), pl({1,3,4}))
  //       + ncoproduct(pl({1,3,4}), pl({1,2,3}))
  //     ;
  //   }
  // );
  // const auto space_a = space_ncoproduct(fx, fx, fx);
  // const auto space_b = space_ncoproduct(l2_prime, fx);
  // const auto ranks = space_venn_ranks(space_a, space_b, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // const int num_points = 5;
  // const int dimension = 3;
  // const auto points = seq_incl(1, num_points);
  // const auto fx = GrFx(dimension, points);
  // const auto l2_prime = mapped(
  //   to_vector(permutations(points)),
  //   [](const auto& args) {
  //     const auto pl = [&](const std::vector<int>& subargs) {
  //       return plucker(choose_indices_one_based(args, subargs));
  //     };
  //     return
  //       + ncoproduct(pl({1,2,3}), pl({1,2,4}))
  //       + ncoproduct(pl({1,2,4}), pl({1,3,4}))
  //       + ncoproduct(pl({1,3,4}), pl({1,2,3}))
  //     ;
  //   }
  // );
  // // Basis:
  // const std::vector space_a = {
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({1,2,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({1,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({1,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({2,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,2,4}), plucker({3,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({1,2,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({1,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({1,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({2,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,4}), plucker({3,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({1,2,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({1,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({1,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({2,3,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({2,3,4}), plucker({3,4,5})),

  //   ncoproduct(plucker({1,2,3}), plucker({1,2,5}), plucker({3,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,3,5}), plucker({2,4,5})),
  //   ncoproduct(plucker({1,2,3}), plucker({1,4,5}), plucker({2,3,5})),
  // };
  // const auto space_b = space_ncoproduct(l2_prime, fx);
  // const auto ranks = space_venn_ranks(space_a, space_b, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // for (const int weight : range_incl(2, 3)) {
  //   for (const int dimension : range_incl(2, 3)) {
  //     for (const int num_points : range_incl(7, 9)) {
  //       Profiler profiler(true);
  //       const auto points = seq_incl(1, num_points);
  //       const auto fx = space_ncoproduct(GrFx(dimension, points));
  //       const auto space_a = mapped(
  //         combinations(fx, weight),
  //         DISAMBIGUATE(ncoproduct)
  //       );
  //       const auto space_b = mapped(
  //         cartesian_combinations(std::vector{
  //           std::pair{GrLArnold2(dimension, points), 1},
  //           std::pair{fx, weight - 2},
  //         }),
  //         DISAMBIGUATE(ncoproduct)
  //       );
  //       profiler.finish("spaces");
  //       const auto ranks = space_venn_ranks(space_a, space_b, identity_function);
  //       profiler.finish("ranks");
  //       const int result = ranks.a() - ranks.intersected();
  //       std::cout << "w=" << weight << ", d=" << dimension << ", p=" << num_points << ": " << result << "\n";
  //     }
  //   }
  // }


  const std::regex line_re("(-?[0-9]+) *([0-9]+)");
  std::ifstream input("../inputs/A3.m", std::ios::in);
  DeltaExpr a_expr;
  for (std::string line; std::getline(input, line); ) {
    trim(line);
    std::smatch match;
    CHECK(std::regex_match(line, match, line_re));
    CHECK_EQ(match.size(), 2+1);
    const int coeff = std::stoi(match[1].str());
    const auto& term_str = match[2].str();
    CHECK(term_str.size() % 2 == 0);
    std::vector<Delta> term;
    for (int i = 0; i < term_str.size(); i += 2) {
      term.push_back(Delta(term_str[i] - '0', term_str[i + 1] - '0'));
    }
    a_expr.add_to(term, coeff);
  }

  // int d = a_expr.element().second;
  // for (const auto& [_, coeff] : a_expr) {
  //   d = std::gcd(d, coeff);
  // }
  // std::cout << "GCD = " << d << "\n";  // == 16
  a_expr.div_int(16);
  // std::cout << a_expr;
  // std::cout << to_lyndon_basis(a_expr);

  const auto a = [&](const std::vector<int>& points) {
    return substitute_variables_1_based(a_expr, points).annotate(
      fmt::function_num_args("a", points)
    );
  };

  // std::cout << a({1,1,2,2,1,2});
  std::cout << a({1,2,3,4,5,6}) - a({2,3,4,5,6,1});

  // const auto expr = a({1,1,1,2,2,3});
  // std::cout << to_lyndon_basis(expr);

  // const auto space = LInf(6, {1,2,3,Inf});
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";
  // std::cout << space.front();

  // const auto space = L(6, to_vector(range_incl(1, 6)));
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";
}
