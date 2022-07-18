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

// In order to reduce compilation time enable expressions only when necessary:

#if 1
#include "lib/bigrassmannian_complex_cohomologies.h"
#include "lib/gamma.h"
#include "lib/chern_arrow.h"
#include "lib/chern_cocycle.h"
#include "lib/polylog_cgrli.h"
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


  // for (const int num_vars : range_incl(2, 4)) {
  //   for (const int weight : range_incl(2, 5)) {
  //     const auto& args = concat(
  //       mapped(range_incl(1, num_vars), [](const int idx) { return X(idx); }),
  //       mapped(range_incl(1, num_vars), [](const int idx) { return -X(idx); })
  //       // std::vector{Inf}
  //     );
  //     auto space = L(weight, args);
  //     // TODO: Why so slow?
  //     space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(identity_function),
  //       DISAMBIGUATE(keep_non_weakly_separated_inv)
  //     );
  //     std::cout << "p=" << args.size() << "(" << num_vars << "), w=" << weight << ": ";
  //     std::cout << to_string(ranks) << "\n";
  //   }
  // }

  // const std::vector p = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // for (const auto& s : combinations(p, 2)) {
  //   const auto [a, b] = to_array<2>(s);
  //   const auto d = Delta(a, b);
  //   if (is_frozen_coord(d, 4)) {
  //     std::cout << to_string(d) << "\n";
  //   }
  // }



  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // const int weight = 5;
  // const auto expr = typeC_QLi(weight, points);
  // std::cout << is_totally_weakly_separated_inv(expr) << "\n";
  // auto space = L(weight, points);
  // // auto space = typeC_CL(weight, points);
  // space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << space_rank(space, DISAMBIGUATE(identity_function)) << "\n";
  // space.push_back(to_lyndon_basis(expr));
  // std::cout << space_rank(space, DISAMBIGUATE(identity_function)) << "\n";

  // const int weight = 4;
  // auto space = typeC_CL(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5});
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // space.push_back(typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}));
  // space.push_back(typeC_QLi(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5}));
  // space.push_back(typeC_QLi(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5}));
  // space.push_back(typeC_QLi(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5}));
  // space.push_back(typeC_QLi(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5}));
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(project_on_x1(expr)).filtered([](const auto& term) {
  //     return absl::c_count(term, Zero) == 0;
  //   });
  // };
  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // const int weight = 4;

  // std::cout << prepare(typeC_QLi(weight, points));
  // std::cout << prepare(QLi3(x1,-x2,-x3,-x4));
  // std::cout << prepare(QLi3(x1,x2,-x3,-x4));
  // std::cout << prepare(QLi3(x1,x2,x3,-x4));
  // std::cout << prepare(QLi3(x1,x2,x3,x4));

  // std::cout << prepare(
  //   +  typeC_QLi(weight, points)
  //   -4*QLi3(x1,-x2,-x3,-x4)
  //   -4*QLi3(x1,x2,x3,-x4)
  // );
  // std::cout << prepare(QLi3(x1,x2,x3,-x1));
  // std::cout << prepare(QLi3(-x4,x1,x2,x4));
  // std::cout << prepare(QLi3(-x3,-x4,x1,x3));
  // std::cout << prepare(QLi3(x1,-x1,-x3,-x4));
  // std::cout << prepare(QLi3(x2,x1,-x4,-x2));
  // std::cout << prepare(QLi3(x3,x2,x1,-x3));

  // std::cout << prepare(
  //   +  typeC_QLi(weight, points)
  //   -4*QLi3(x1,-x2,-x3,-x4)
  //   -4*QLi3(x1,x2,x3,-x4)
  //   +4*QLi3(x1,x2,x3,-x1)
  //   +4*QLi3(-x4,x1,x2,x4)
  //   +4*QLi3(x2,x1,-x4,-x2)
  // );

  // std::cout << to_lyndon_basis(
  //   + typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   - typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1})
  //   + 2 * (
  //     + QLi3(x1,x2,x3,x4)
  //     - QLi3(x2,x3,x4,-x1)
  //     + QLi3(x3,x4,-x1,-x2)
  //     - QLi3(x4,-x1,-x2,-x3)
  //     + QLi3(-x1,-x2,-x3,-x4)
  //     - QLi3(-x2,-x3,-x4,x1)
  //     + QLi3(-x3,-x4,x1,x2)
  //     - QLi3(-x4,x1,x2,x3)
  //   )
  // );

  // for (const int weight : range_incl(2, 7)) {
  //   const auto space = mapped_parallel(
  //     typeC_CL(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}), DISAMBIGUATE(to_lyndon_basis)
  //   );
  //   const auto a = to_lyndon_basis(typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}));
  //   const auto b = to_lyndon_basis(typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1}));
  //   std::cout << "w=" << weight << "\n";
  //   std::cout << "diff lies in = " << space_contains(space, {a - b}, DISAMBIGUATE(identity_function)) << "\n";
  //   std::cout << "sum lies in = " << space_contains(space, {a + b}, DISAMBIGUATE(identity_function)) << "\n";
  //   std::cout << "\n";
  // }

  // const int weight = 4;
  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // // const auto space = mapped_expanding(range(4), [&](const int shift) {
  // //   return std::array{
  // //     QLiVec(weight, choose_indices_one_based(rotated_vector(points, shift), {1,2,3,4})),
  // //     QLiVec(weight, choose_indices_one_based(rotated_vector(points, shift), {1,2,5,6})),
  // //   };
  // // });
  // const auto qli_a = to_lyndon_basis(
  //   + QLi4(x1,x2,x3,x4)
  //   + QLi4(x2,x3,x4,-x1)
  //   + QLi4(x3,x4,-x1,-x2)
  //   + QLi4(x4,-x1,-x2,-x3)
  //   + QLi4(-x1,-x2,-x3,-x4)
  //   + QLi4(-x2,-x3,-x4,x1)
  //   + QLi4(-x3,-x4,x1,x2)
  //   + QLi4(-x4,x1,x2,x3)
  // );
  // const auto qli_b = to_lyndon_basis(
  //   + QLi4(x1,x2,-x1,-x2)
  //   + QLi4(x2,x3,-x2,-x3)
  //   + QLi4(x3,x4,-x3,-x4)
  //   + QLi4(x4,-x1,-x4,x1)
  // );
  // const std::vector space = {qli_a, qli_b};

  // const auto typec_qli = to_lyndon_basis(
  //   + typeC_QLi(weight, points)
  //   + typeC_QLi(weight, rotated_vector(points, 1))
  // );
  // // std::cout << dump_to_string(space) << "\n";
  // // std::cout << to_string(space_venn_ranks(space, {expr}, DISAMBIGUATE(identity_function))) << "\n";
  // std::cout << 8*qli_a + qli_b + 2*typec_qli;

  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // for (const int weight : range_incl(2, 6)) {
  //   const int sign = neg_one_pow(weight);
  //   std::cout << to_lyndon_basis(
  //     + typeC_QLiSymm(weight, points)
  //     + sign * typeC_QLiSymm(weight, rotated_vector(points, 1))
  //   );
  // }

  // std::cout << to_lyndon_basis(typeC_QLi(weight, {x1,x1,x3,x4,-x1,-x1,-x3,-x4}));  // ZERO
  // std::cout << to_lyndon_basis(typeC_QLi(weight, {x1,x2,x2,x4,-x1,-x2,-x2,-x4}));
  // std::cout << to_lyndon_basis(QLi4(1,1,2,3,4,5));  // ZERO
  // std::cout << to_lyndon_basis(QLi4(1,2,2,3,4,5));


  // std::cout << to_lyndon_basis(
  //   + QLi2(x1,x2,x3,-x1)
  //   + QLi2(x2,x3,-x1,-x2)
  //   + QLi2(x3,-x1,-x2,-x3)
  //   + typeC_QLi(2, {x1,x2,-x1,-x2})
  //   + typeC_QLi(2, {x2,x3,-x2,-x3})
  //   + typeC_QLi(2, {x3,-x1,-x3,x1})
  // );
  // std::cout << to_lyndon_basis(
  //   + QLi2(x1,x2,x3,-x1)
  //   + QLi2(x2,x3,-x1,-x2)
  //   + QLi2(x3,-x1,-x2,-x3)
  //   + typeC_QLi(2, {x1,x2,-x1,-x2})
  //   - typeC_QLi(2, {x1,x3,-x1,-x3})
  //   + typeC_QLi(2, {x2,x3,-x2,-x3})
  // );

  // const int weight = 3;
  // const std::vector space = {
  //   typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   ,
  //   + typeC_QLi(weight, {x1,x2,-x1,-x2})
  //   + typeC_QLi(weight, {x2,x3,-x2,-x3})
  //   + typeC_QLi(weight, {x3,x4,-x3,-x4})
  //   + typeC_QLi(weight, {x4,-x1,-x4,x1})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x3,x4})
  //   + QLiSymmVec(weight, {x2,x3,x4,-x1})
  //   + QLiSymmVec(weight, {x3,x4,-x1,-x2})
  //   + QLiSymmVec(weight, {x4,-x1,-x2,-x3})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x3,-x1})
  //   + QLiSymmVec(weight, {x2,x3,x4,-x2})
  //   + QLiSymmVec(weight, {x3,x4,-x1,-x3})
  //   + QLiSymmVec(weight, {x4,-x1,-x2,-x4})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x3,-x3})
  //   + QLiSymmVec(weight, {x2,x3,x4,-x4})
  //   + QLiSymmVec(weight, {x3,x4,-x1,x1})
  //   + QLiSymmVec(weight, {x4,-x1,-x2,x2})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x4,-x1})
  //   + QLiSymmVec(weight, {x2,x3,-x1,-x2})
  //   + QLiSymmVec(weight, {x3,x4,-x2,-x3})
  //   + QLiSymmVec(weight, {x4,-x1,-x3,-x4})
  //   ,
  //   + typeC_QLi(weight, {x1,x3,-x1,-x3})
  //   + typeC_QLi(weight, {x2,x4,-x2,-x4})
  // };
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << " / " << space.size() << "\n";

  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(expr);
  //   // return to_lyndon_basis(project_on_x1(expr));
  //   // return to_lyndon_basis(project_on_x1(expr)).filtered([](const auto& term) {
  //   //   return num_distinct_elements_unsorted(mapped(term, [](X x) { return x.idx(); })) >= 3;
  //        // TODO: Update helper function for ProjectionExpr to do this (use mapped_filtered):
  //   //   // std::vector<int> variables;
  //   //   // for (X x: term) {
  //   //   //   if (!x.is_constant()) {
  //   //   //     variables.push_back(x.idx());
  //   //   //   }
  //   //   // }
  //   //   // return num_distinct_elements_unsorted(variables) >= 3;
  //   // });
  // };
  // for (const int weight : {2, 3}) {
  //   std::cout << prepare(
  //     + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //     - (
  //       + typeC_QLi(weight, {x1,x2,-x1,-x2})
  //       + typeC_QLi(weight, {x2,x3,-x2,-x3})
  //       + typeC_QLi(weight, {x3,x4,-x3,-x4})
  //       + typeC_QLi(weight, {x1,x4,-x1,-x4})
  //     )
  //     + (
  //       + typeC_QLi(weight, {x1,x3,-x1,-x3})
  //       + typeC_QLi(weight, {x2,x4,-x2,-x4})
  //     )
  //     - (
  //       + QLiSymmVec(weight, {x1,x2,x3,x4})
  //       + QLiSymmVec(weight, {x2,x3,x4,-x1})
  //       + QLiSymmVec(weight, {x3,x4,-x1,-x2})
  //       + QLiSymmVec(weight, {x4,-x1,-x2,-x3})
  //     )
  //     - (
  //       + QLiSymmVec(weight, {x1,x2,x4,-x1})
  //       + QLiSymmVec(weight, {x2,x3,-x1,-x2})
  //       + QLiSymmVec(weight, {x3,x4,-x2,-x3})
  //       + QLiSymmVec(weight, {x4,-x1,-x3,-x4})
  //     )
  //     + (
  //       + QLiSymmVec(weight, {x1,x2,x3,-x1})
  //       + QLiSymmVec(weight, {x2,x3,x4,-x2})
  //       + QLiSymmVec(weight, {x3,x4,-x1,-x3})
  //       + QLiSymmVec(weight, {x4,-x1,-x2,-x4})
  //     )
  //     + (
  //       + QLiSymmVec(weight, {x1,x2,x3,-x3})
  //       + QLiSymmVec(weight, {x2,x3,x4,-x4})
  //       + QLiSymmVec(weight, {x1,x3,x4,-x1})
  //       + QLiSymmVec(weight, {x2,x4,-x1,-x2})
  //     )
  //   );
  // }

  // const int weight = 4;
  // const auto space = typeC_CL(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5}),
  // const std::vector new_funcs = {
  //   + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   - typeC_QLiSymm(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5})
  //   + typeC_QLiSymm(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5})
  //   - typeC_QLiSymm(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5})
  //   + typeC_QLiSymm(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5})
  // };
  // std::cout << to_string(space_venn_ranks(space, new_funcs, DISAMBIGUATE(to_lyndon_basis))) << "\n";

  // const int weight = 4;
  // const auto space = typeC_CB(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5});
  // const auto expr =
  //   + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   - typeC_QLiSymm(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5})
  //   + typeC_QLiSymm(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5})
  //   - typeC_QLiSymm(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5})
  //   + typeC_QLiSymm(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5})
  //   - QLiSymm4(x1,x2,x3,x4,x5,-x1)
  //   - QLiSymm4(x2,x3,x4,x5,-x1,-x2)
  //   - QLiSymm4(x3,x4,x5,-x1,-x2,-x3)
  //   - QLiSymm4(x4,x5,-x1,-x2,-x3,-x4)
  //   - QLiSymm4(x5,-x1,-x2,-x3,-x4,-x5)
  // ;
  // std::cout << expr;
  // std::cout << to_string(space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis))) << "\n";


  // int weight = 5;
  // const auto space = CL(weight, {x1,x2,x3,x4,x5});
  // const auto expr = typeC_QLiSymm(weight, {x1,x2,x3,x4,x5,x5,x5,x5});
  // std::cout << to_string(space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis))) << "\n";
  // std::cout << to_lyndon_basis(expr);

  // const std::vector points = {x1,x2,x3,x4,x5,x6,-x1,-x2,-x3,-x4,-x5,-x6};
  // const std::vector space = {
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{3,4,5,6,7,2})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{1,2,7,8}))
  //   ),
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{5,6,7,8,3,4})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{3,8,9,2}))
  //   ),
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{5,6,7,8,9,4})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{3,4,9,10}))
  //   ),
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{7,8,9,10,5,6})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{5,10,11,4}))
  //   ),
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{7,8,9,10,11,6})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{5,6,11,12}))
  //   ),
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{9,10,11,12,7,8})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{7,12,1,6}))
  //   ),

  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{1,2,3,4})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{1,4,5,6,7,10,11,12}))
  //   ),
  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{3,4,5,2})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{5,6,7,8,11,12,1,2}))
  //   ),
  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{3,4,5,6})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{3,6,7,8,9,12,1,2}))
  //   ),
  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{5,6,7,4})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{7,8,9,10,1,2,3,4}))
  //   ),
  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{5,6,7,8})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{5,8,9,10,11,2,3,4}))
  //   ),
  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{7,8,9,6})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{9,10,11,12,3,4,5,6}))
  //   ),
  // };
  // const auto type_c_space = CL(3, points);
  // std::cout << to_string(space_venn_ranks(type_c_space, space, DISAMBIGUATE(to_lyndon_basis))) << "\n";

  // std::cout << to_lyndon_basis(
  //   tensor_product(
  //     QLiVec   (2, choose_indices_one_based(points, std::vector{3,4,5,6,7,2})),
  //     typeC_QLi(1, choose_indices_one_based(points, std::vector{1,2,7,8}))
  //   )
  // );
  // std::cout << to_lyndon_basis(
  //   tensor_product(
  //     QLiVec   (1, choose_indices_one_based(points, std::vector{1,2,3,4})),
  //     typeC_QLi(2, choose_indices_one_based(points, std::vector{1,4,5,6,7,10,11,12}))
  //   )
  // );


  // auto expr = QLi5(1,2,3,4,5,6,7,8);
  // std::cout << to_lyndon_basis(expr.filtered([](const auto& term) {
  //   return contains_naive(term, Delta(1,4));
  // }));


  // auto expr = to_lyndon_basis(QLi5(1,2,3,4,5,6,7,8));
  // auto expr = to_lyndon_basis(QLi6(1,2,3,4));

  // expr = expr.filtered([](const auto& term) {
  //   // return contains_naive(term, Delta(1,2)) && contains_naive(term, Delta(4,5));

  //   return
  //     absl::c_count(term, Delta(1,2)) > 0 &&
  //     absl::c_count(term, Delta(2,4)) > 0 &&
  //     absl::c_count(term, Delta(3,4)) > 0 &&
  //     absl::c_all_of(term, [](const auto d) {
  //       return
  //         d == Delta(1,2) ||
  //         d == Delta(2,4) ||
  //         d == Delta(3,4)
  //       ;
  //     })
  //   ;

  //   // return absl::c_count(term, Delta(1,2)) > 1;

  //   // const auto special_d = Delta(1,4);
  //   // if (term[0] != special_d) {
  //   //   return false;
  //   // }
  //   // const auto it = absl::c_find_if(term, [&](const auto& d) { return d != special_d; });
  //   // return std::find(it, term.end(), special_d) != term.end();
  // });
  // // print_sorted_by_num_distinct_variables(std::cout, expr);
  // std::cout << expr;

  // auto expr = to_lyndon_basis(QLi3(1,2,3,4));
  // expr = expr.filtered([](const auto& term) {
  //   return contains_naive(term, Delta(1,2)) && contains_naive(term, Delta(3,4));
  // });
  // print_sorted_by_num_distinct_variables(std::cout, expr);

  // const std::vector points = {1,2,3};
  // const int weight = 5;
  // const auto coords = mapped(combinations(points, 2), [](const auto& pair) {
  //   const auto [a, b] = to_array<2>(pair);
  //   return Delta(a, b);
  // });
  // const auto space = mapped(
  //   filtered(
  //     cartesian_power(coords, weight),
  //     [](auto term) {
  //       keep_unique_sorted(term);
  //       return all_unique_unsorted(term);
  //     }
  //   ),
  //   [](const auto& term) {
  //     // std::cout << dump_to_string(term) << "\n";
  //     return DeltaExpr::single(term);
  //   }
  // );
  // // const auto expr = QLiVec(weight, points);
  // const auto expr = DeltaExpr::single({Delta(1,2), Delta(1,2), Delta(1,3), Delta(1,2), Delta(2,3)});
  // std::cout << to_lyndon_basis(expr);
  // CHECK_EQ(expr.weight(), weight);
  // std::cout << to_string(space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis))) << "\n";


  // const std::vector points = {1,2,3,4,5,6};
  // const int weight = 5;
  // const int dimension = 3;
  // const auto coords = mapped(combinations(points, dimension), [](const auto& points) {
  //   return Gamma(points);
  // });
  // Profiler profiler;
  // auto space = mapped(
  //   filtered(
  //     cartesian_power(coords, weight),
  //     [](auto term) {
  //       keep_unique_sorted(term);
  //       return all_unique_unsorted(term) && is_weakly_separated(term);
  //     }
  //   ),
  //   [](const auto& term) {
  //     return GammaExpr::single(term);
  //   }
  // );
  // profiler.finish("space");
  // space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  // profiler.finish("lyndon");
  // const auto expr = to_lyndon_basis(CGrLiVec(weight, points));
  // profiler.finish("expr");
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(identity_function));
  // profiler.finish("ranks");
  // std::cout << to_string(ranks) << "\n";


  // const auto cgrl_dim3_reduced = [](int weight, const std::vector<int>& points) {
  //   Gr_Space space;
  //   for (const int bonus_point_idx : range(points.size())) {
  //     const auto bonus_args = choose_indices(points, {bonus_point_idx});
  //     const auto main_args = removed_index(points, bonus_point_idx);
  //     append_vector(space, mapped(CB(weight, main_args), [&](const auto& expr) {
  //       return pullback(expr, bonus_args);
  //     }));
  //   }
  //   for (const auto& args : combinations(points, 6)) {
  //     for (const int shift : {0, 1, 2}) {
  //       space.push_back(CGrLiVec(weight, rotated_vector(args, shift)));
  //     }
  //   }
  //   return space;
  // };
  // // std::cout << space_rank(CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6,7}), DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // // std::cout << space_rank(cgrl_dim3_reduced(4, {1,2,3,4,5,6,7}), DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // // std::cout << "\n";

  // const int weight = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8,9};
  // // const auto space = CGrL_test_space(weight, dimension, points);
  // Gr_Space space;
  // for (const int bonus_point_idx : range(points.size())) {
  //   const auto bonus_args = choose_indices(points, {bonus_point_idx});
  //   const auto main_args = removed_index(points, bonus_point_idx);
  //   append_vector(space, mapped(cgrl_dim3_reduced(weight, main_args), [&](const auto& expr) {
  //     return pullback(expr, bonus_args);
  //   }));
  // }
  // // for (const auto& args : combinations(points, 8)) {
  // //   space.push_back(CGrLiVec(weight, args));
  // // }

  // // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // // const auto expr = CGrLiVec(weight, {8,7,6,5,4,3,2,1});  // TODO: Test: lies in space on 8 points
  // const auto expr =
  //   + CGrLiVec(weight, {1,2,3,4,5,6,7,8})
  //   - CGrLiVec(weight, {1,2,3,4,5,6,7,9})
  //   + CGrLiVec(weight, {1,2,3,4,5,6,8,9})
  //   - CGrLiVec(weight, {1,2,3,4,5,7,8,9})
  //   + CGrLiVec(weight, {1,2,3,4,6,7,8,9})
  //   - CGrLiVec(weight, {1,2,3,5,6,7,8,9})
  //   + CGrLiVec(weight, {1,2,4,5,6,7,8,9})
  //   - CGrLiVec(weight, {1,3,4,5,6,7,8,9})
  //   + CGrLiVec(weight, {2,3,4,5,6,7,8,9})
  // ;
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // const int weight = 4;
  // const int dimension = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // Gr_Space space;
  // for (const int bonus_point_idx : range(points.size())) {
  //   const auto bonus_args = choose_indices(points, {bonus_point_idx});
  //   const auto main_args = removed_index(points, bonus_point_idx);
  //   append_vector(space, mapped(cgrl_dim3_reduced(weight, main_args), [&](const auto& expr) {
  //     return pullback(expr, bonus_args);
  //   }));
  // }
  // for (const auto& args : combinations(points, 8)) {
  //   space.push_back(CGrLiVec(weight, args));
  // }
  // // const auto expr = CGrLiVec(weight, {8,7,6,5,4,3,2,1});
  // // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // const auto space_new = OldChernGrL(weight, dimension, points);
  // const auto ranks = space_venn_ranks(space, space_new, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // for (const int weight : range_incl(1, 5)) {
  //   for (const int dimension : range_incl(2, 4)) {
  //     for (const int num_points : range_incl(4, 8)) {
  //       const auto points = seq_incl(1, num_points);
  //       const auto space = OldChernGrL(weight, dimension, points);
  //       std::cout << "w=" << weight << ", d=" << dimension << ", n=" << num_points << ": ";
  //       std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  //     }
  //   }
  // }

  // const auto space = OldChernGrL(4, 4, {1,2,3,4,5,6,7,8});
  // const auto expr = plucker_dual(CGrLi4(1,2,3,4,5,6,7,8), {1,2,3,4,5,6,7,8});
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // // TODO: Compute on the cluster
  // const int weight = 5;
  // const int dimension = 5;
  // const std::vector points = {1,2,3,4,5,6,7,8,9,10};
  // Gr_Space space;
  // for (const int bonus_point_idx : range(points.size())) {
  //   const auto bonus_args = choose_indices(points, {bonus_point_idx});
  //   const auto main_args = removed_index(points, bonus_point_idx);
  //   append_vector(space, mapped(OldChernGrL(weight, dimension - 1, main_args), [&](const auto& expr) {
  //     return pullback(expr, bonus_args);
  //   }));
  // }
  // const auto expr =
  //   + CGrLi5(1,2,3,4,5,6,7,8,9,10)
  //   + CGrLi5(2,3,4,5,6,7,8,9,10,1)
  // ;
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";

  // TODO: Test: CGrLiVec(w, {1,2,3,4}) is consistent with non-Grassmannian polylogs.

  // const int weight = 3;
  // const int dimension = 3;
  // const int num_points = 6;
  // const auto points = seq_incl(1, num_points);
  // const auto space = OldChernGrL(weight, dimension, points);
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [&](const auto& expr) {
  //     return std::tuple {
  //       to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,1})),
  //       to_lyndon_basis(expr + substitute_variables(expr, {6,5,4,3,2,1})),
  //       to_lyndon_basis(
  //         + substitute_variables(expr, {1,2,3,4,5,6})
  //         - substitute_variables(expr, {1,2,3,4,5,7})
  //         + substitute_variables(expr, {1,2,3,4,6,7})
  //         - substitute_variables(expr, {1,2,3,5,6,7})
  //         + substitute_variables(expr, {1,2,4,5,6,7})
  //         - substitute_variables(expr, {1,3,4,5,6,7})
  //         + substitute_variables(expr, {2,3,4,5,6,7})
  //       ),
  //       to_lyndon_basis(expr + plucker_dual(expr, points)),
  //     };
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int weight = 4;
  // const int dimension = 4;
  // const int num_points = 8;
  // const auto points = seq_incl(1, num_points);
  // // const auto space = OldChernGrL(weight, dimension, points);
  // const auto space = CGrL_test_space(weight, dimension, points);
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [&](const auto& expr) {
  //     return std::tuple {
  //       to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,7,8,1})),
  //       to_lyndon_basis(expr - substitute_variables(expr, {8,7,6,5,4,3,2,1})),
  //       to_lyndon_basis(
  //         + substitute_variables(expr, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(expr, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(expr, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(expr, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(expr, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(expr, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(expr, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(expr, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(expr, {2,3,4,5,6,7,8,9})
  //       ),
  //       to_lyndon_basis(
  //         + pullback(substitute_variables(expr, {1,2,3,4,5,6,7,8}), {9})
  //         - pullback(substitute_variables(expr, {1,2,3,4,5,6,7,9}), {8})
  //         + pullback(substitute_variables(expr, {1,2,3,4,5,6,8,9}), {7})
  //         - pullback(substitute_variables(expr, {1,2,3,4,5,7,8,9}), {6})
  //         + pullback(substitute_variables(expr, {1,2,3,4,6,7,8,9}), {5})
  //         - pullback(substitute_variables(expr, {1,2,3,5,6,7,8,9}), {4})
  //         + pullback(substitute_variables(expr, {1,2,4,5,6,7,8,9}), {3})
  //         - pullback(substitute_variables(expr, {1,3,4,5,6,7,8,9}), {2})
  //         + pullback(substitute_variables(expr, {2,3,4,5,6,7,8,9}), {1})
  //       ),
  //       // to_lyndon_basis(expr - plucker_dual(expr, points)),
  //     };
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";


  // const int weight = 4;
  // const int dimension = 3;
  // const int num_points = 7;
  // const auto points = seq_incl(1, num_points);
  // Profiler profiler;
  // const auto space = mapped_parallel(
  //   OldChernGrL(weight, dimension, points),
  //   [&](const auto& a) {
  //     const auto b =
  //       + substitute_variables(a, {1,2,3,4,5,6,7})
  //       + substitute_variables(a, {2,3,4,5,6,7,1})
  //       + substitute_variables(a, {3,4,5,6,7,1,2})
  //       + substitute_variables(a, {4,5,6,7,1,2,3})
  //       + substitute_variables(a, {5,6,7,1,2,3,4})
  //       + substitute_variables(a, {6,7,1,2,3,4,5})
  //       + substitute_variables(a, {7,1,2,3,4,5,6})
  //     ;
  //     const auto c = b - substitute_variables(b, {7,6,5,4,3,2,1});
  //     const auto d =
  //       + substitute_variables(c, {1,2,3,4,5,6,7})
  //       - substitute_variables(c, {1,2,3,4,5,6,8})
  //       + substitute_variables(c, {1,2,3,4,5,7,8})
  //       - substitute_variables(c, {1,2,3,4,6,7,8})
  //       + substitute_variables(c, {1,2,3,5,6,7,8})
  //       - substitute_variables(c, {1,2,4,5,6,7,8})
  //       + substitute_variables(c, {1,3,4,5,6,7,8})
  //       - substitute_variables(c, {2,3,4,5,6,7,8})
  //     ;
  //     // const auto e = d + plucker_dual(d, {1,2,3,4,5,6,7,8});

  //     const auto& expr = d;
  //     CHECK(to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,7,8,1})).is_zero());
  //     CHECK(to_lyndon_basis(expr - substitute_variables(expr, {8,7,6,5,4,3,2,1})).is_zero());
  //     CHECK(to_lyndon_basis(
  //       + substitute_variables(expr, {1,2,3,4,5,6,7,8})
  //       - substitute_variables(expr, {1,2,3,4,5,6,7,9})
  //       + substitute_variables(expr, {1,2,3,4,5,6,8,9})
  //       - substitute_variables(expr, {1,2,3,4,5,7,8,9})
  //       + substitute_variables(expr, {1,2,3,4,6,7,8,9})
  //       - substitute_variables(expr, {1,2,3,5,6,7,8,9})
  //       + substitute_variables(expr, {1,2,4,5,6,7,8,9})
  //       - substitute_variables(expr, {1,3,4,5,6,7,8,9})
  //       + substitute_variables(expr, {2,3,4,5,6,7,8,9})
  //     ).is_zero());
  //     // CHECK(to_lyndon_basis(expr - plucker_dual(expr, {1,2,3,4,5,6,7,8})).is_zero());

  //     return to_lyndon_basis(expr);
  //   }
  // );
  // profiler.finish("space");
  // const auto rank = space_rank(space, DISAMBIGUATE(identity_function));
  // profiler.finish("rank");
  // std::cout << rank << "\n";



  // const int weight = 4;
  // const int dimension = 3;
  // const int num_points = 6;
  // const auto points = seq_incl(1, num_points);
  // Profiler profiler;
  // const auto space = mapped_parallel(
  //   OldChernGrL(weight, dimension, points),
  //   [&](const auto& a) {
  //     const auto b =
  //       + substitute_variables(a, {1,2,3,4,5,6})
  //       - substitute_variables(a, {2,3,4,5,6,1})
  //       + substitute_variables(a, {3,4,5,6,1,2})
  //       - substitute_variables(a, {4,5,6,1,2,3})
  //       + substitute_variables(a, {5,6,1,2,3,4})
  //       - substitute_variables(a, {6,1,2,3,4,5})
  //     ;
  //     const auto c = b - substitute_variables(b, {6,5,4,3,2,1});
  //     const auto d =
  //       - substitute_variables(c, {1,2,3,4,5,6})
  //       + substitute_variables(c, {1,2,3,4,5,7})
  //       - substitute_variables(c, {1,2,3,4,6,7})
  //       + substitute_variables(c, {1,2,3,5,6,7})
  //       - substitute_variables(c, {1,2,4,5,6,7})
  //       + substitute_variables(c, {1,3,4,5,6,7})
  //       - substitute_variables(c, {2,3,4,5,6,7})
  //     ;
  //     const auto e =
  //       - pullback(substitute_variables(d, {1,2,3,4,5,6,7}), {8})
  //       + pullback(substitute_variables(d, {1,2,3,4,5,6,8}), {7})
  //       - pullback(substitute_variables(d, {1,2,3,4,5,7,8}), {6})
  //       + pullback(substitute_variables(d, {1,2,3,4,6,7,8}), {5})
  //       - pullback(substitute_variables(d, {1,2,3,5,6,7,8}), {4})
  //       + pullback(substitute_variables(d, {1,2,4,5,6,7,8}), {3})
  //       - pullback(substitute_variables(d, {1,3,4,5,6,7,8}), {2})
  //       + pullback(substitute_variables(d, {2,3,4,5,6,7,8}), {1})
  //     ;

  //     const auto& expr = e;
  //     CHECK(to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,7,8,1})).is_zero());
  //     CHECK(to_lyndon_basis(expr - substitute_variables(expr, {8,7,6,5,4,3,2,1})).is_zero());
  //     CHECK(to_lyndon_basis(
  //       + substitute_variables(expr, {1,2,3,4,5,6,7,8})
  //       - substitute_variables(expr, {1,2,3,4,5,6,7,9})
  //       + substitute_variables(expr, {1,2,3,4,5,6,8,9})
  //       - substitute_variables(expr, {1,2,3,4,5,7,8,9})
  //       + substitute_variables(expr, {1,2,3,4,6,7,8,9})
  //       - substitute_variables(expr, {1,2,3,5,6,7,8,9})
  //       + substitute_variables(expr, {1,2,4,5,6,7,8,9})
  //       - substitute_variables(expr, {1,3,4,5,6,7,8,9})
  //       + substitute_variables(expr, {2,3,4,5,6,7,8,9})
  //     ).is_zero());
  //     CHECK(to_lyndon_basis(expr - plucker_dual(expr, {1,2,3,4,5,6,7,8})).is_zero());
  //     CHECK(to_lyndon_basis(
  //       + pullback(substitute_variables(expr, {1,2,3,4,5,6,7,8}), {9})
  //       - pullback(substitute_variables(expr, {1,2,3,4,5,6,7,9}), {8})
  //       + pullback(substitute_variables(expr, {1,2,3,4,5,6,8,9}), {7})
  //       - pullback(substitute_variables(expr, {1,2,3,4,5,7,8,9}), {6})
  //       + pullback(substitute_variables(expr, {1,2,3,4,6,7,8,9}), {5})
  //       - pullback(substitute_variables(expr, {1,2,3,5,6,7,8,9}), {4})
  //       + pullback(substitute_variables(expr, {1,2,4,5,6,7,8,9}), {3})
  //       - pullback(substitute_variables(expr, {1,3,4,5,6,7,8,9}), {2})
  //       + pullback(substitute_variables(expr, {2,3,4,5,6,7,8,9}), {1})
  //     ).is_zero());

  //     return to_lyndon_basis(expr);
  //   }
  // );
  // profiler.finish("space");
  // const auto rank = space_rank(space, DISAMBIGUATE(identity_function));
  // profiler.finish("rank");
  // std::cout << rank << "\n";


  // const int weight = 4;
  // const auto cgrli_dual = [](const int weight, const std::vector<int>& points) {
  //   return plucker_dual(CGrLiVec(weight, points), points);
  // };
  // const auto expr =
  //   + cgrli_dual(weight, {1,2,3,4,5,6,7,8})
  //   - cgrli_dual(weight, {1,2,3,4,5,6,7,9})
  //   + cgrli_dual(weight, {1,2,3,4,5,6,8,9})
  //   - cgrli_dual(weight, {1,2,3,4,5,7,8,9})
  //   + cgrli_dual(weight, {1,2,3,4,6,7,8,9})
  //   - cgrli_dual(weight, {1,2,3,5,6,7,8,9})
  //   + cgrli_dual(weight, {1,2,4,5,6,7,8,9})
  //   - cgrli_dual(weight, {1,3,4,5,6,7,8,9})
  //   + cgrli_dual(weight, {2,3,4,5,6,7,8,9})
  // ;
  // const auto expr =
  //   + CGrLiVec(weight, {1,2,3,4,5,6,7,8})
  //   + CGrLiVec(weight, {2,3,4,5,6,7,8,1})
  // ;
  // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << to_string(ranks) << "\n";


  // // TODO: Try to dyhedralize and find space basis
  // for (const int weight : range_incl(3, 4)) {
  //   for (const int num_points : range_incl(8, 8)) {
  //     const auto points = seq_incl(1, num_points);
  //     Profiler profiler;
  //     const auto space = concat(
  //       wedge_OldChernGrL(weight, 4, points),
  //       wedge_OldChernGrL(weight, 5, points),
  //       wedge_OldChernGrL(weight, 6, points)
  //     );
  //     profiler.finish("make space");
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(to_lyndon_basis),
  //       [&](const auto& expr) {
  //         const auto x = to_lyndon_basis(chern_arrow_left(expr, num_points + 1));
  //         const auto y = to_lyndon_basis(chern_arrow_up(expr, num_points + 1));
  //         const auto z = GammaNCoExpr();
  //         switch (expr.dimension()) {
  //           // case 4: return std::tuple{x, y};
  //           // case 4: return std::tuple{x, y, z};
  //           // case 5: return std::tuple{z, x, y};
  //           case 4: return std::tuple{x, y, z, z};
  //           case 5: return std::tuple{z, x, y, z};
  //           case 6: return std::tuple{z, z, x, y};
  //           // case 2: return std::tuple{x, y, z, z, z};
  //           // case 3: return std::tuple{z, x, y, z, z};
  //           // case 4: return std::tuple{z, z, x, y, z};
  //           // case 5: return std::tuple{z, z, z, x, y};
  //           // case 2: return std::tuple{x, y, z, z, z, z};
  //           // case 3: return std::tuple{z, x, y, z, z, z};
  //           // case 4: return std::tuple{z, z, x, y, z, z};
  //           // case 5: return std::tuple{z, z, z, x, y, z};
  //           // case 6: return std::tuple{z, z, z, z, x, y};
  //           default: FATAL("Unexpected dimension");
  //         };
  //       }
  //     );
  //     std::cout << "w=" << weight << ", n=" << num_points << ":  " << to_string(ranks) << "\n";
  //   }
  // }


  // const int weight = 5;
  // const int mid_point = weight + 1;
  // const int num_points = mid_point * 2 - 1;
  // const auto points = seq_incl(1, num_points);
  // const int num_pullbacks = 1;
  // const int num_before = 1;
  // const int num_after = 1;
  // Profiler profiler;
  // GammaExpr a;
  // for (const auto& pb: combinations(seq(mid_point - 1), num_pullbacks)) {
  //   for (const auto& before: combinations(seq(mid_point - 1 - num_pullbacks), num_before)) {
  //     for (auto after: combinations(seq(mid_point - 1), num_after)) {
  //       const int num_pbs = num_pullbacks;  // workaround: lambdas cannot capture structured bindings
  //       after = mapped(after, [&](const int p) { return p + mid_point - num_pbs; });
  //       const int sign = neg_one_pow(sum(pb) + sum(before) + sum(after));
  //       const auto [pb_point, non_pb_points] = split_indices(points, pb);
  //       const auto main_points = removed_indices(non_pb_points, concat(before, after));
  //       a += sign * pullback(CGrLiVec(weight, main_points), pb_point);
  //     }
  //   }
  // }
  // profiler.finish("a");
  // GammaExpr b;
  // for (const int p: range(num_points)) {
  //   const int sign = neg_one_pow(p);
  //   b += sign * CGrLiVec(weight, removed_index(points, p));
  // }
  // profiler.finish("b");
  // const auto sum = to_lyndon_basis(a - b);
  // profiler.finish("lyndon");
  // std::cout << sum;


  // const auto space = mapped(OldChernGrL(4, 3, {1,2,3,4,5,6,7}), [](const auto& expr) {
  //   return chern_arrow_up(expr, 8);
  // });
  // const auto cgrli_expr = CGrLi4(1,2,3,4,5,6,7,8);
  // const auto ranks = space_venn_ranks(space, {cgrli_expr}, [](const auto& expr) {
  //   return to_lyndon_basis(chern_arrow_left(expr, 9));
  // });
  // std::cout << to_string(ranks) << "\n";

  // const std::vector points = {1,2,3,4,5,6,7,8,9,10};
  // for (const int weight: {4,5}) {
  //   const auto expr = CGrLiVec(weight, points);
  //   std::cout << to_lyndon_basis(expr + plucker_dual(expr, points));
  // }


  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(chern_arrow_left(expr, 7));
  // };
  // const auto expr =
  //   + CGrLi3(1,2,3,4,5,6)
  //   - pullback(CGrLi3(2,3,4,5), {1})
  //   + pullback(CGrLi3(2,3,4,6), {1})
  //   - pullback(CGrLi3(2,3,5,6), {1})
  //   + pullback(CGrLi3(1,3,4,5), {2})
  //   - pullback(CGrLi3(1,3,4,6), {2})
  //   + pullback(CGrLi3(1,3,5,6), {2})
  // ;
  // std::cout << prepare(expr);

  // const auto expr = CGrLi4(1,2,3,4,5,6,7,8);
  // Gr_Space space;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // for (const int pb_arg : range_incl(1, 3)) {
  //   const auto main_args_pool = removed_index(points, pb_arg - 1);
  //   for (const auto& main_args : combinations(main_args_pool, 6)) {
  //     space.push_back(pullback(CGrLiVec(4, main_args), {pb_arg}));
  //   }
  // }
  // const auto ranks = space_venn_ranks(space, {expr}, [](const auto& expr) {
  //   return to_lyndon_basis(chern_arrow_left(expr, 9));
  // });
  // std::cout << to_string(ranks) << "\n";



  // std::cout << dump_to_string(wedge_ChernGrL(3, 2, {1,2,3,4,5})) << "\n";

  // const std::vector wedge_chern_space = {
  //   // ncoproduct(CGrLi2(1,2,3,4), G({1,2})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({1,4})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({1,5})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({2,3})),
  //   ncoproduct(CGrLi2(1,2,3,4), G({3,4})),
  //   ncoproduct(CGrLi2(1,2,3,4), G({4,5})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({1,2})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({1,5})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({2,3})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({3,4})),
  //   ncoproduct(CGrLi2(1,2,3,5), G({3,5})),
  //   ncoproduct(CGrLi2(1,2,3,5), G({4,5})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({1,2})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({1,5})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({2,3})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({2,4})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({3,4})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({4,5})),
  //   ncoproduct(CGrLi2(1,3,4,5), G({1,2})),
  //   // ncoproduct(CGrLi2(1,3,4,5), G({1,3})),
  //   ncoproduct(CGrLi2(1,3,4,5), G({1,5})),
  //   // ncoproduct(CGrLi2(1,3,4,5), G({2,3})),
  //   ncoproduct(CGrLi2(1,3,4,5), G({3,4})),
  //   ncoproduct(CGrLi2(1,3,4,5), G({4,5})),
  //   ncoproduct(CGrLi2(2,3,4,5), G({1,2})),
  //   // ncoproduct(CGrLi2(2,3,4,5), G({1,5})),
  //   // ncoproduct(CGrLi2(2,3,4,5), G({2,3})),
  //   ncoproduct(CGrLi2(2,3,4,5), G({2,5})),
  //   ncoproduct(CGrLi2(2,3,4,5), G({3,4})),
  //   ncoproduct(CGrLi2(2,3,4,5), G({4,5})),
  // };

  // const std::vector wedge_chern_space = {
  //   // ncoproduct(CGrLi2(1,2,3,4), G({1,2})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({1,4})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({1,5})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({2,3})),
  //   // ncoproduct(CGrLi2(1,2,3,4), G({3,4})), #
  //   // ncoproduct(CGrLi2(1,2,3,4), G({4,5})), *
  //   // ncoproduct(CGrLi2(1,2,3,5), G({1,2})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({1,5})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({2,3})),
  //   ncoproduct(CGrLi2(1,2,3,5), G({3,4})),
  //   ncoproduct(CGrLi2(1,2,3,5), G({3,5})),
  //   // ncoproduct(CGrLi2(1,2,3,5), G({4,5})), *
  //   // ncoproduct(CGrLi2(1,2,4,5), G({1,2})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({1,5})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({2,3})),
  //   // ncoproduct(CGrLi2(1,2,4,5), G({2,4})),
  //   ncoproduct(CGrLi2(1,2,4,5), G({3,4})),
  //   ncoproduct(CGrLi2(1,2,4,5), G({4,5})),
  //   ncoproduct(CGrLi2(1,3,4,5), G({1,2})),
  //   // ncoproduct(CGrLi2(1,3,4,5), G({1,3})),
  //   ncoproduct(CGrLi2(1,3,4,5), G({1,5})),
  //   // ncoproduct(CGrLi2(1,3,4,5), G({2,3})),
  //   // ncoproduct(CGrLi2(1,3,4,5), G({3,4})), #
  //   // ncoproduct(CGrLi2(1,3,4,5), G({4,5})), *
  //   ncoproduct(CGrLi2(2,3,4,5), G({1,2})),
  //   // ncoproduct(CGrLi2(2,3,4,5), G({1,5})),
  //   // ncoproduct(CGrLi2(2,3,4,5), G({2,3})),
  //   ncoproduct(CGrLi2(2,3,4,5), G({2,5})),
  //   // ncoproduct(CGrLi2(2,3,4,5), G({3,4})), #
  //   // ncoproduct(CGrLi2(2,3,4,5), G({4,5})), *
  // };

  // const auto space = mapped(
  //   wedge_chern_space,
  //   // wedge_ChernGrL(3, 2, {1,2,3,4,5}),
  //   [](const auto& expr) {
  //     return std::tuple {
  //       chern_arrow_left(expr, 6),
  //       chern_arrow_up(expr, 6),
  //     };
  //   }
  // );
  // const auto pair = std::tuple {
  //   ncomultiply(ChernCocycle(3, 2, {1,2,3,4,5,6})),
  //   ncomultiply(ChernCocycle(3, 3, {1,2,3,4,5,6})),
  // };
  // const auto ranks = space_venn_ranks(space, {pair}, DISAMBIGUATE(identity_function));
  // std::cout << to_string(ranks) << "\n";

  // const auto wedge_chern_expr =
  //   + ncoproduct(CGrLi2(1,2,3,5), G({3,4}))
  //   - ncoproduct(CGrLi2(1,2,4,5), G({3,4}))
  //   - ncoproduct(CGrLi2(1,2,3,5), G({3,5}))
  //   + ncoproduct(CGrLi2(1,2,4,5), G({4,5}))
  //   + ncoproduct(CGrLi2(1,3,4,5), G({1,2}))
  //   - ncoproduct(CGrLi2(2,3,4,5), G({1,2}))
  //   - ncoproduct(CGrLi2(1,3,4,5), G({1,5}))
  //   + ncoproduct(CGrLi2(2,3,4,5), G({2,5}))
  // ;
  // std::cout << (
  //   + chern_arrow_left(wedge_chern_expr, 6)
  //   - ncomultiply(ChernCocycle(3, 2, {1,2,3,4,5,6}))
  // );  // ZERO
  // std::cout << (
  //   + chern_arrow_up(wedge_chern_expr, 6)
  //   - ncomultiply(ChernCocycle(3, 3, {1,2,3,4,5,6}))
  // );  // ZERO

  // std::cout << (
  //   + chern_arrow_left(ChernCocycle(3, 2, {1,2,3,4,5}), 6)
  //   - ncomultiply(ChernCocycle(3, 2, {1,2,3,4,5,6}))
  // );  // ZERO
  // std::cout << (
  //   + chern_arrow_up(ChernCocycle(3, 2, {1,2,3,4,5}), 6)
  //   - ncomultiply(ChernCocycle(3, 3, {1,2,3,4,5,6}))
  // );  // ZERO

  // const auto expr =
  //   + ncomultiply(ChernCocycle(3, 2, {1,2,3,4,5}))
  //   - chern_arrow_left(ChernCocycle(3, 1, {1,2,3,4}), 5)
  // ;
  // std::cout << expr;  // ZERO

  // TODO: Test that it's always true (fix the sign first)
  // comultiply(cocycle(w, d, n)) == left(cocycle(w, d, n-1)) + up(cocycle(w, d-1, n-1))

  // std::cout << ChernCocycle(4, 1, {1,2,3,4,5});


  // const auto prepare = [](const GammaExpr& expr) {
  //   return prnt::set_object_to_string(
  //     expr.filtered([](const auto& term) {
  //       return all_vars(term).size() == 5 && common_vars(term).size() == 1;
  //     }),
  //     [](const auto& term) {
  //       const auto vars = common_vars(term);
  //       return absl::StrCat(
  //         internal::GammaExprParam::object_to_string(term),
  //         "   ",
  //         str_join(vars, ",")
  //       );
  //     }
  //   );
  // };
  // auto lhs = to_lyndon_basis(CGrLi3(1,2,3,4,5,6));
  // lhs = lhs.filtered([](const auto& term) {
  //   return absl::c_all_of(
  //     transposed(mapped(term, [](const Gamma& g) { return g.index_vector(); })),
  //     DISAMBIGUATE(is_strictly_increasing)
  //   );
  // });
  // std::cout << prepare(lhs);

  // GammaExpr rhs;
  // for (const auto& [points, sign] : permutations_with_sign({1,2,3,4,5,6})) {
  //   rhs += sign * tensor_product(absl::MakeConstSpan({
  //     G(choose_indices_one_based(points, {1,2,3})),
  //     G(choose_indices_one_based(points, {2,3,4})),
  //     G(choose_indices_one_based(points, {3,4,5})),
  //   }));
  // }
  // rhs = to_lyndon_basis(rhs)
  //   .dived_int(2)
  //   .filtered(DISAMBIGUATE(is_weakly_separated))
  // ;
  // std::cout << prepare(rhs);
  // std::cout << prepare(lhs - rhs);


  // GammaExpr p, q;
  // for (const auto& [points, sign] : permutations_with_sign({1,2,3,4})) {
  //   p += sign * tensor_product(absl::MakeConstSpan({
  //     G(choose_indices_one_based(points, {1,2})),
  //     G(choose_indices_one_based(points, {2,3})),
  //   }));
  //   q += sign * CGrLiVec(2, points);
  // }
  // p = to_lyndon_basis(p);
  // q = to_lyndon_basis(q).without_annotations();
  // std::cout << p;
  // std::cout << q;
  // std::cout << p.dived_int(2) + q.dived_int(24);

  // GammaExpr p, q;
  // for (const auto& [points, sign] : permutations_with_sign({1,2,3,4,5,6})) {
  //   p += sign * tensor_product(absl::MakeConstSpan({
  //     G(choose_indices_one_based(points, {1,2,3})),
  //     G(choose_indices_one_based(points, {2,3,4})),
  //     G(choose_indices_one_based(points, {3,4,5})),
  //   }));
  //   q += sign * CGrLiVec(3, points);
  // }
  // p = to_lyndon_basis(p);
  // q = to_lyndon_basis(q).without_annotations();
  // std::cout << p;
  // std::cout << q;
  // std::cout << p.dived_int(2) - q.dived_int(240);


  // Profiler profiler;
  // const auto expr = CGrLi5(1,2,3,4,5,6,7,8);
  // profiler.finish("expr");
  // auto coexpr = ncomultiply(expr, {1,4});
  // profiler.finish("comult");
  // std::cout << "\n";
  // // This zeroes out terms where we take three points from {1,2,3,4} and one point from {5,6,7,8}.
  // // Should also do vice versa to the the full equation.
  // coexpr +=
  //   + ncoproduct(CGrLi4(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   + ncoproduct(
  //     + CGrLiVec(4, {5}, {1,2,4,6,7,8})
  //     - CGrLiVec(4, {6}, {1,2,4,5,7,8})
  //     + CGrLiVec(4, {7}, {1,2,4,5,6,8})
  //     ,
  //     plucker({1,2,4,8})
  //   )
  //   - ncoproduct(
  //     + CGrLiVec(4, {5}, {1,3,4,6,7,8})
  //     - CGrLiVec(4, {6}, {1,3,4,5,7,8})
  //     + CGrLiVec(4, {7}, {1,3,4,5,6,8})
  //     ,
  //     plucker({1,3,4,8})
  //   )
  //   + ncoproduct(
  //     + CGrLiVec(4, {5}, {2,3,4,6,7,8})
  //     - CGrLiVec(4, {6}, {2,3,4,5,7,8})
  //     + CGrLiVec(4, {7}, {2,3,4,5,6,8})
  //     ,
  //     plucker({2,3,4,8})
  //   )
  //   + ncoproduct(
  //     + CGrLiVec(4, {5}, {1,2,4,6,7,8})
  //     - CGrLiVec(4, {5}, {1,3,4,6,7,8})
  //     + CGrLiVec(4, {5}, {2,3,4,6,7,8})
  //     ,
  //     plucker({1,2,3,5})
  //   )
  //   - ncoproduct(
  //     + CGrLiVec(4, {6}, {1,2,4,5,7,8})
  //     - CGrLiVec(4, {6}, {1,3,4,5,7,8})
  //     + CGrLiVec(4, {6}, {2,3,4,5,7,8})
  //     ,
  //     plucker({1,2,3,6})
  //   )
  //   + ncoproduct(
  //     + CGrLiVec(4, {7}, {1,2,4,5,6,8})
  //     - CGrLiVec(4, {7}, {1,3,4,5,6,8})
  //     + CGrLiVec(4, {7}, {2,3,4,5,6,8})
  //     ,
  //     plucker({1,2,3,7})
  //   )
  //   - ncoproduct(CGrLiVec(4, {5}, {1,2,4,6,7,8}), plucker({1,2,4,5}))
  //   + ncoproduct(CGrLiVec(4, {6}, {1,2,4,5,7,8}), plucker({1,2,4,6}))
  //   - ncoproduct(CGrLiVec(4, {7}, {1,2,4,5,6,8}), plucker({1,2,4,7}))
  //   + ncoproduct(CGrLiVec(4, {5}, {1,3,4,6,7,8}), plucker({1,3,4,5}))
  //   - ncoproduct(CGrLiVec(4, {6}, {1,3,4,5,7,8}), plucker({1,3,4,6}))
  //   + ncoproduct(CGrLiVec(4, {7}, {1,3,4,5,6,8}), plucker({1,3,4,7}))
  //   - ncoproduct(CGrLiVec(4, {5}, {2,3,4,6,7,8}), plucker({2,3,4,5}))
  //   + ncoproduct(CGrLiVec(4, {6}, {2,3,4,5,7,8}), plucker({2,3,4,6}))
  //   - ncoproduct(CGrLiVec(4, {7}, {2,3,4,5,6,8}), plucker({2,3,4,7}))
  //   - ncoproduct(
  //     + CGrLiVec(4, {1,2,3,4,5,6,7,8})
  //     + CGrLiVec(4, {5}, {2,3,4,6,7,8})
  //     - CGrLiVec(4, {5}, {1,3,4,6,7,8})
  //     + CGrLiVec(4, {5}, {1,2,4,6,7,8})
  //     - CGrLiVec(4, {6}, {2,3,4,5,7,8})
  //     + CGrLiVec(4, {6}, {1,3,4,5,7,8})
  //     - CGrLiVec(4, {6}, {1,2,4,5,7,8})
  //     + CGrLiVec(4, {7}, {2,3,4,5,6,8})
  //     - CGrLiVec(4, {7}, {1,3,4,5,6,8})
  //     + CGrLiVec(4, {7}, {1,2,4,5,6,8})
  //     ,
  //     plucker({1,2,3,8})
  //   )
  // ;
  // std::cout << coexpr;
  // std::cout << coexpr.termwise_abs().mapped<GammaExpr>([](const auto& term) {
  //   return term.at(0);
  // });
  // // const auto lhs = filter_coexpr(coexpr, 0, std::vector{Gamma({1,2,3,8})});
  // // const auto rhs = ;
  // // std::cout << lhs;
  // // std::cout << rhs;
  // // std::cout << lhs + rhs;


  // const auto expr =
  //   + CGrLi4(1,2,3,4,5,6,7,8)
  //   - CGrLi4(1,2,3,4,5,6,7,9)
  //   + CGrLi4(1,2,3,4,5,6,8,9)
  //   - CGrLi4(1,2,3,4,5,7,8,9)
  //   + CGrLi4(1,2,3,4,6,7,8,9)
  // ;
  // auto coexpr = ncomultiply(expr, {1,});
  // std::cout << coexpr.termwise_abs().mapped<GammaExpr>([](const auto& term) {
  //   return term.at(0);
  // });
  // // std::cout << to_lyndon_basis(
  // //   + expr
  // //   + substitute_variables(expr, {1,2,4,3,5,6,7,8,9})
  // // );


  // for (const int p : range_incl(3, 5)) {
  //   const auto lhs = CGrLiVec(p-1, seq_incl(1, 2*p));
  //   const auto rhs = neg_one_pow(p) * a_minus(b_plus(CGrLiVec(p-1, seq_incl(1, 2*p-2)), 2*p-1), 2*p);
  //   std::cout << to_lyndon_basis(lhs - rhs);
  // }

  // for (const int p : range_incl(3, 5)) {
  //   const auto lhs = CGrLiVec(p-1, seq_incl(1, 2*p));
  //   const auto x = CGrLiVec(p-1, seq_incl(1, 2*p-2));
  //   const auto rhs = neg_one_pow(p) * b_minus(a_full(x, 2*p-1) - a_plus(x, 2*p-1), 2*p);
  //   std::cout << to_lyndon_basis(lhs - rhs);
  // }

  // for (const int p : range_incl(3, 4)) {
  //   const auto x = CGrLiVec(p, seq_incl(1, 2*p-2));
  //   const auto expr = a_full(
  //     + CGrLiVec(p, seq_incl(1, 2*p))
  //     + neg_one_pow(p-1) * (
  //       + a_plus(b_full(x, 2*p-1), 2*p)
  //       - a_plus(b_plus(x, 2*p-1), 2*p)
  //     )
  //     , 2*p+1
  //   );
  //   std::cout << to_lyndon_basis(expr);
  // }



  // for (const int n : range_incl(4, 5)) {
  //   for (const int p : range_incl(3, 5)) {
  //     if (!are_CGrLi_args_ok(n-1, 2*p)) {
  //       continue;
  //     }

  //     const auto gli_large = CGrLiVec(n-1, seq_incl(1, 2*p));
  //     const auto gli_small = CGrLiVec(n-1, seq_incl(1, 2*p-2));

  //     const auto s = neg_one_pow(p);
  //     const auto lhs = ncomultiply(CGrLiVec(n, seq_incl(1, 2*p)), {1,n-1});
  //     const auto rhs =
  //       + ncoproduct(gli_large, plucker(seq_incl(1, p)))
  //       + ncoproduct(gli_large, plucker(seq_incl(p+1, 2*p)))

  //       - s * a_minus(ncoproduct(
  //         b_plus(gli_small, 2*p-1),
  //         plucker(concat(seq_incl(1, p-1), {2*p-1}))
  //       ), 2*p)
  //       - s * a_plus(ncoproduct(
  //         b_minus(gli_small, 2*p-1),
  //         plucker(seq_incl(p, 2*p-1))
  //       ), 2*p)

  //       + s * b_plus(ncoproduct(
  //         a_minus(gli_small, 2*p-1),
  //         plucker(seq_incl(1, p-1))
  //       ), 2*p)
  //       + s * b_minus(ncoproduct(
  //         a_plus(gli_small, 2*p-1),
  //         plucker(seq_incl(p, 2*p-2))
  //       ), 2*p)

  //       + s *  a_minus(b_plus(
  //         ncoproduct(gli_small, plucker(seq_incl(1, p-1)))
  //       , 2*p-1), 2*p)
  //       + s * a_plus(b_minus(
  //         ncoproduct(gli_small, plucker(seq_incl(p, 2*p-2)))
  //       , 2*p-1), 2*p)

  //       - ncoproduct(
  //         + gli_large
  //         - s * a_minus(b_plus(gli_small, 2*p-1), 2*p)
  //         ,
  //         plucker(concat(seq_incl(1, p-1), {2*p}))
  //       )
  //       - ncoproduct(
  //         + gli_large
  //         - s * a_plus(b_minus(gli_small, 2*p-1), 2*p)
  //         ,
  //         plucker(seq_incl(p, 2*p-1))
  //       )
  //     ;
  //     const auto coexpr = lhs + rhs;
  //     // std::cout << coexpr.termwise_abs().mapped<GammaExpr>([](const auto& term) {
  //     //   return term.at(0);
  //     // });
  //     std::cout << coexpr;
  //   }
  // }


  // std::vector exprs_odd_num_points = {
  //   CGrLi2[{5}](1,2,3,4),
  //   GrQLi2(5)(1,2,3,4),
  //   GrLi(5)(1,2,3,4),
  //   G({1,2,3,4,5}),
  //   tensor_product(G({1,2,3}), G({3,4,5})),
  // };
  // std::vector exprs_even_num_points = {
  //   CGrLi2(1,2,3,4),
  //   GrQLi2()(1,2,3,4),
  //   GrLi(5,6)(1,2,3,4),
  //   G({1,2,3,4,5,6}),
  //   tensor_product(G({1,2,3,4}), G({3,4,5,6})),
  // };
  // for (const auto& expr : exprs_odd_num_points) {
  //   const int n = detect_num_variables(expr);
  //   CHECK(a_minus_minus(expr, n+1) == a_minus(expr, n+1));
  //   CHECK(a_plus_plus(expr, n+1) == a_plus(expr, n+1));
  //   CHECK(b_minus_minus(expr, n+1) == b_minus(expr, n+1));
  //   CHECK(b_plus_plus(expr, n+1) == b_plus(expr, n+1));
  // }
  // using ArrowF = std::function<GammaExpr(const GammaExpr&, int)>;
  // const std::vector<ArrowF> basic_arrows = {
  //   DISAMBIGUATE(a_full),
  //   DISAMBIGUATE(a_minus),
  //   DISAMBIGUATE(a_plus),
  //   DISAMBIGUATE(b_full),
  //   DISAMBIGUATE(b_minus),
  //   DISAMBIGUATE(b_plus),
  // };
  // const std::vector<ArrowF> extra_arrows = {
  //   DISAMBIGUATE(a_minus_minus),
  //   DISAMBIGUATE(a_plus_plus),
  //   DISAMBIGUATE(b_minus_minus),
  //   DISAMBIGUATE(b_plus_plus),
  // };
  // for (const bool even_num_point : {false, true}) {
  //   std::cout << "\n\n# " << (even_num_point ? "Even" : "Odd") << " num points:\n\n";
  //   const auto& test_exprs = even_num_point ? exprs_even_num_points : exprs_odd_num_points;
  //   // TODO: Only use extra arrows when makes sense
  //   // const auto arrows = <?> ? basic_arrows : concat(basic_arrows, extra_arrows);
  //   const auto arrows = concat(basic_arrows, extra_arrows);
  //   absl::flat_hash_set<std::pair<int, int>> zeros;
  //   for (const int out : range(arrows.size())) {
  //     for (const int in : range(arrows.size())) {
  //       const auto make_eqn = [&](const auto& expr, const int n) {
  //         return arrows[out](arrows[in](expr, n+1), n+2);
  //       };
  //       bool eqn_holds = true;
  //       for (const auto& expr : test_exprs) {
  //         const int n = detect_num_variables(expr);
  //         const auto eqn = make_eqn(expr, n);
  //         if (!eqn.is_zero()) {
  //           eqn_holds = false;
  //           break;
  //         }
  //       }
  //       if (eqn_holds) {
  //         const auto expr = GammaExpr().annotate("x");
  //         const auto eqn = make_eqn(expr, 0);
  //         std::cout << annotations_one_liner(eqn.annotations()) << " == 0\n";
  //         zeros.insert({out, in});
  //       }
  //     }
  //   }
  //   std::cout << "\n";
  //   for (const int l_out : range(arrows.size())) {
  //     for (const int l_in : range(arrows.size())) {
  //       for (const int r_out : range(arrows.size())) {
  //         for (const int r_in : range(arrows.size())) {
  //           const bool is_trivial =
  //             std::tie(l_out, l_in) >= std::tie(r_out, r_in)
  //             || zeros.contains({l_out, l_in})
  //             || zeros.contains({r_out, r_in})
  //           ;
  //           if (is_trivial) {
  //             continue;
  //           }
  //           for (const int sign : {-1, 1}) {
  //             const auto make_eqn = [&](const auto& expr, const int n) {
  //               return
  //                 + arrows[l_out](arrows[l_in](expr, n+1), n+2)
  //                 + sign * arrows[r_out](arrows[r_in](expr, n+1), n+2)
  //               ;
  //             };
  //             bool eqn_holds = true;
  //             for (const auto& expr : test_exprs) {
  //               const int n = detect_num_variables(expr);
  //               const auto eqn = make_eqn(expr, n);
  //               if (!eqn.is_zero()) {
  //                 eqn_holds = false;
  //                 break;
  //               }
  //             }
  //             if (eqn_holds) {
  //               const auto expr = GammaExpr().annotate("x");
  //               const auto eqn = make_eqn(expr, 0);
  //               std::cout << annotations_one_liner(eqn.annotations()) << " == 0\n";
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }


  // for (const int dimension : range_incl(3, 5)) {
  //   const int weight = dimension - 1;
  //   const int num_points = dimension * 2;
  //   const auto points = seq_incl(1, num_points);
  //   const auto space = ChernGrL(weight, dimension, points);
  //   const auto ranks = space_mapping_ranks(
  //     space,
  //     DISAMBIGUATE(to_lyndon_basis),
  //     [&](const auto& expr) {
  //       return std::tuple{
  //         to_lyndon_basis(a_minus(expr, num_points + 1)),
  //         to_lyndon_basis(a_plus(expr, num_points + 1)),
  //         to_lyndon_basis(b_minus(expr, num_points + 1)),
  //         to_lyndon_basis(b_plus(expr, num_points + 1)),
  //         to_lyndon_basis(expr + neg_one_pow(dimension) * plucker_dual(expr, points)),
  //       };
  //     }
  //   );
  //   std::cout << to_string(ranks) << "\n";
  //   // for (const auto& expr : space) {
  //   //   if (
  //   //     to_lyndon_basis(a_minus(expr, num_points + 1)).is_zero() &&
  //   //     to_lyndon_basis(a_plus(expr, num_points + 1)).is_zero() &&
  //   //     to_lyndon_basis(b_minus(expr, num_points + 1)).is_zero() &&
  //   //     to_lyndon_basis(b_plus(expr, num_points + 1)).is_zero()
  //   //   ) {
  //   //     std::cout << expr;
  //   //   }
  //   // }
  // }


  // const int n = 6;
  // const auto x = ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}));
  // const auto y =
  //   + b_minus(a_minus_minus(x, n+1), n+2)
  //   + a_plus(b_plus_plus(x, n+1), n+2)
  //   + a_plus(b_minus(x, n+1), n+2)
  //   - a_plus(b_minus_minus(x, n+1), n+2)
  // ;
  // std::cout << to_lyndon_basis(a_full(y, n+3));


  // TODO: TeX this proof (Proposition 4.10, former 5.9)

  // std::cout <<
  // + ncomultiply(CGrLi4(1,2,3,4,5,6,7,8), {1,3})
  // - ncoproduct(CGrLi3(1,2,3,4,5,6,7,8) - a_plus(b_minus(CGrLi3(1,2,3,4,5,6), 7), 8), plucker({4,5,6,7}))
  // - ncoproduct(CGrLi3(1,2,3,4,5,6,7,8) - a_minus(b_plus(CGrLi3(1,2,3,4,5,6), 7), 8), plucker({1,2,3,8}))
  // - a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)
  // - a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)
  // + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // + b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)
  // + b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)
  // + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  // + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({5,6,7,8}))
  // ;
  // std::cout <<
  // + ncomultiply(CGrLi4(1,2,3,4,5,6), {1,3})
  // - ncoproduct(CGrLi3(1,2,3,4,5,6) + a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  // - ncoproduct(CGrLi3(1,2,3,4,5,6) + a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  // + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  // + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  // - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  // - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  // - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  // + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  // + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  // ;

  // const auto x =
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6,7,8) - a_plus(b_minus(CGrLi3(1,2,3,4,5,6), 7), 8), plucker({4,5,6,7}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6,7,8) - a_minus(b_plus(CGrLi3(1,2,3,4,5,6), 7), 8), plucker({1,2,3,8}))
  //   - a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)
  //   - a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)
  //   + b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({5,6,7,8}))
  // ;
  // const auto y =
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6) + a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6) + a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;

  // const auto x =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({5,6,7,8}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6,7,8) - a_plus(b_minus(CGrLi3(1,2,3,4,5,6), 7), 8), plucker({4,5,6,7}))  // zero
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6,7,8) - a_minus(b_plus(CGrLi3(1,2,3,4,5,6), 7), 8), plucker({1,2,3,8}))  // zero
  //   - a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)
  //   - a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)
  //   + b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)
  //   + b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // ;
  // const auto y =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;

  // const auto x =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))  // rewrite
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({5,6,7,8}))  // rewrite
  //   - a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)
  //   - a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)
  //   + b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)
  //   + b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // ;
  // const auto y =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;

  // const auto x =
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)  // changed
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)  // changed
  //   - a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)  // next
  //   - a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)
  //   + b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)
  //   + b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // ;
  // const auto y =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;


  // const auto x =
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus(ncoproduct(b_plus_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)  // changed
  //   // - a_plus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6,7})), 8)  // reminder, zero after a_full
  //   + a_minus(ncoproduct(b_minus_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)  // changed
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)  // reminder
  //   + b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)  // next
  //   + b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)  // next
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // ;
  // const auto y =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;

  // const auto x =
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus(ncoproduct(b_plus_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6,7})), 8)
  //   + a_minus(ncoproduct(b_minus_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3,7})), 8)
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   - b_plus(ncoproduct(a_plus_plus(CGrLi3(1,2,3,4,5,6), 7), plucker({1,2,3})), 8)  // changed
  //   - b_plus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8)  // reminder
  //   - b_minus(ncoproduct(a_minus_minus(CGrLi3(1,2,3,4,5,6), 7), plucker({4,5,6})), 8)  // changed
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)  // reminder
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // ;
  // const auto y =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;

  // const auto x =
  //   // sort amd move ncoproduct inside
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus(b_plus_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   - b_plus(a_plus_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - b_minus(a_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   - b_plus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8)
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)
  // ;
  // const auto y =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout <<
  //   a_full(x - a_plus(b_minus_minus(y, 7), 8), 9)
  // ;

  // const auto x6 =
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus(b_plus_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   - b_plus(a_plus_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - b_minus(a_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // ;
  // const auto x4 =
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   - b_plus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8)
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)
  // ;
  // const auto y6 =
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3}))
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5}))
  //   - ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6}))
  // ;
  // const auto y4 =
  //   - ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5}))
  //   - ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6}))
  //   + a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6)
  //   + a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6)
  //   - b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6)
  //   - b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6)
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6)
  //   - a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6)
  // ;
  // std::cout << a_full(x6 - a_plus(b_minus_minus(y6, 7), 8), 9);
  // std::cout << a_full(x4 - a_plus(b_minus_minus(y4, 7), 8), 9);

  // const auto xy6 =
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus(b_plus_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   - b_plus(a_plus_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - b_minus(a_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_plus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_plus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - b_plus (a_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_minus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)

  //   - b_minus(a_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)
  //   + a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - b_plus (a_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)

  //   + a_minus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)
  //   + a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_plus (b_full       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_plus (b_full       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_plus (b_full       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_minus(b_full       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_full (b_full       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   - a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)

  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);

  // const auto xy6 =
  //   + a_full (b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,6})), 7), 8)
  //   + a_full (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({3,4,5})), 7), 8)

  //   - a_plus (b_minus_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_minus      (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_minus(b_plus       (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  //   + a_plus (b_plus_plus  (ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({4,5,6})), 7), 8)
  // ;
  // std::cout << a_full(xy6, 9);


  // std::cout << a_full(
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   - b_plus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8)
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6})), 7), 8)
  //   - a_plus(b_minus_minus(a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6), 7), 8)
  //   - a_plus(b_minus_minus(a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6), 7), 8)
  //   + a_plus(b_minus_minus(b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8)
  //   + a_plus(b_minus_minus(b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6), 7), 8)
  //   + a_plus(b_minus_minus(a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6), 7), 8)
  //   + a_plus(b_minus_minus(a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8)
  // , 9);  // zero

  // expr "Q"
  // std::cout << a_full(
  //   - b_plus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8)
  //   + a_plus(b_minus_minus(b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8)
  //   + a_plus(b_minus_minus(a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8)
  // , 9);  // zero

  // std::cout << a_full(
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6})), 7), 8)
  //   - a_plus(b_minus_minus(a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6), 7), 8)
  //   - a_plus(b_minus_minus(a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6), 7), 8)
  //   + a_plus(b_minus_minus(b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6), 7), 8)
  //   + a_plus(b_minus_minus(a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6), 7), 8)
  // , 9);  // zero (reminder)

  // std::cout << a_full(
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus_minus(b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6), 7), 8)
  //   + a_plus(b_minus_minus(a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({3,4})), 5), 6), 7), 8)  // zero
  // , 9);  // zero

  // std::cout << a_full(
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6})), 7), 8)
  //   - a_plus(b_minus_minus(a_plus(ncoproduct(b_minus(CGrLi3(1,2,3,4), 5), plucker({3,4,5})), 6), 7), 8)  // zero
  //   - a_plus(b_minus_minus(a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6), 7), 8)
  // , 9);  // zero (reminder)

  // std::cout << a_full(
  //   - b_minus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({4,5,6})), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_plus(b_minus(CGrLi3(1,2,3,4), 5), 6), plucker({3,4,5})), 7), 8)
  //   + a_plus(b_minus_minus(b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({3,4})), 6), 7), 8)
  // , 9);  // zero

  // std::cout << a_full(
  //   - a_minus(ncoproduct(b_full(b_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3,7})), 8)
  //   + a_plus(b_minus_minus(ncoproduct(a_minus(b_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,6})), 7), 8)
  //   - a_plus(b_minus_minus(a_minus(ncoproduct(b_plus(CGrLi3(1,2,3,4), 5), plucker({1,2,5})), 6), 7), 8)
  // , 9);  // zero

  // // consider a term from "Q"
  // std::cout <<
  //   + a_full(a_plus(b_minus_minus(a_minus(b_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  // ;

  // a = a- + a++ = a-- + a+
  // a- = a-- + a+ - a++
  //   but a-- term is zero
  // std::cout <<
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   - a_full(b_full(b_minus(a_minus(a_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   + a_full(b_full(b_minus(a_minus(a_plus_plus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  // ;

  // std::cout <<
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   - a_full(b_full(b_minus(a_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  //   + a_full(b_full(b_minus(a_minus(ncoproduct(a_plus_plus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  // ;

  // std::cout <<
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   + a_full(b_full(a_minus(b_minus(ncoproduct(a_plus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  //   - a_full(b_full(a_minus(b_minus(ncoproduct(a_plus_plus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  // ;

  // // move b_minus inside
  // std::cout <<
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   + a_full(b_full(a_minus(ncoproduct(b_minus(a_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,3})), 7), 8), 9)
  //   - a_full(b_full(a_minus(ncoproduct(b_minus(a_plus_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,3})), 7), 8), 9)
  // ;

  // // temporary swap b_full and a_minus, change a_minus to a_plus_plus because a_full(a_full(x)) == 0 and swap back
  // std::cout <<
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   - a_full(b_full(a_plus_plus(ncoproduct(b_minus(a_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,3})), 7), 8), 9)
  //   + a_full(b_full(a_plus_plus(ncoproduct(b_minus(a_plus_plus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2,3})), 7), 8), 9)
  // ;

  // std::cout <<
  //   + a_full(b_full(b_minus(a_minus(a_minus(ncoproduct(CGrLi3(1,2,3,4), plucker({1,2})), 5), 6), 7), 8), 9)
  //   - a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_plus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   + a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_plus_plus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  // ;

  // // now consider another term from "Q"
  // std::cout <<
  //   + a_full(a_plus(b_minus_minus(b_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  //   - a_full(b_full(b_plus(a_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  // ;

  // std::cout <<
  //   - a_full(b_full(b_plus(a_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  //   - a_full(b_full(b_minus_minus(a_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  // ;

  // std::cout <<
  //   - a_full(b_full(b_plus(a_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  //   + a_full(b_full(a_plus_plus(b_minus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  // ;

  // std::cout <<
  //   - a_full(b_full(b_plus(a_plus(ncoproduct(a_minus(CGrLi3(1,2,3,4), 5), plucker({1,2})), 6), 7), 8), 9)
  //   + a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  // ;

  // // now consider the last term from "Q"
  // std::cout <<
  //   - a_full(b_plus(ncoproduct(a_full(a_plus(b_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   - a_full(b_plus(ncoproduct(b_minus_minus(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  // ;

  // std::cout <<
  //   - a_full(b_plus(ncoproduct(b_minus_minus(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   - a_full(b_plus(b_minus_minus(ncoproduct(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2})), 7), 8), 9)
  // ;

  // std::cout <<
  //   - a_full(b_plus(b_minus_minus(ncoproduct(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2})), 7), 8), 9)
  //   + a_full(b_full(b_minus_minus(ncoproduct(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2})), 7), 8), 9)
  // ;

  // std::cout <<
  //   + a_full(b_full(b_minus_minus(ncoproduct(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), plucker({1,2})), 7), 8), 9)
  //   - a_full(b_full(ncoproduct(b_minus_minus(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  // ;

  // // now collect all terms from "Q" together
  // std::cout <<
  //   - a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_plus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   + a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_plus_plus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   + a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   + a_full(b_full(ncoproduct(b_minus_minus(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  // ;

  // std::cout <<
  //   + a_full(b_full(ncoproduct(a_plus_plus(b_minus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  //   + a_full(b_full(ncoproduct(b_minus_minus(a_plus(a_minus_minus(CGrLi3(1,2,3,4), 5), 6), 7), plucker({1,2,3})), 8), 9)
  // ;
  // // Q.E.D.


  // for (const int p : range_incl(3, 4)) {
  //   std::cout << to_lyndon_basis(
  //     + CGrLiVec(p, seq_incl(1, 2*p+2))
  //     - neg_one_pow(p-1) * a_plus(b_minus(CGrLiVec(p, seq_incl(1, 2*p)), 2*p+1), 2*p+2)
  //   );
  // }

  // using ArrowF = std::function<GammaNCoExpr(const GammaNCoExpr&, int)>;
  // const std::vector<ArrowF> arrows = {
  //   DISAMBIGUATE(a_full),
  //   DISAMBIGUATE(a_minus),
  //   DISAMBIGUATE(a_plus),
  //   DISAMBIGUATE(b_full),
  //   DISAMBIGUATE(b_minus),
  //   DISAMBIGUATE(b_plus),
  //   DISAMBIGUATE(a_minus_minus),
  //   DISAMBIGUATE(a_plus_plus),
  //   DISAMBIGUATE(b_minus_minus),
  //   DISAMBIGUATE(b_plus_plus),
  // };
  // for (const auto p : range_incl(3, 4)) {
  //   for (const auto& out : arrows) {
  //     for (const auto& in : arrows) {
  //       for (const int sign : {-1, 1}) {
  //         const auto eqn = to_lyndon_basis(
  //           + CGrLiVec(p, seq_incl(1, 2*p+2))
  //           + sign * neg_one_pow(p) * out(in(CGrLiVec(p, seq_incl(1, 2*p)), 2*p+1), 2*p+2)
  //         );
  //         if (eqn.is_zero()) {
  //           std::cout << annotations_one_liner(eqn.annotations()) << " = 0\n";
  //         }
  //       }
  //     }
  //   }
  //   std::cout << "\n";
  // }

  // for (const auto p : range_incl(3, 4)) {
  //   std::cout << to_lyndon_basis(
  //     a_full(
  //       + CGrLiVec(p, seq_incl(1, 2*p))
  //       + neg_one_pow(p-1) * a_plus(b_minus_minus(CGrLiVec(p, seq_incl(1, 2*p-2)), 2*p-1), 2*p),
  //       2*p+1
  //     )
  //   );
  // }
  // for (const auto p : range_incl(3, 4)) {
  //   std::cout << to_lyndon_basis(
  //     b_full(
  //       + CGrLiVec(p, seq_incl(1, 2*p))
  //       + neg_one_pow(p) * b_plus(a_minus_minus(CGrLiVec(p, seq_incl(1, 2*p-2)), 2*p-1), 2*p),
  //       2*p+1
  //     )
  //   );
  // }

  // std::cout << to_lyndon_basis(
  //   + ncoproduct(CGrLi3(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   - a_plus(b_minus(ncoproduct(CGrLi3(1,2,3,4,5,6), plucker({1,2,3})), 7), 8)
  // );
}
