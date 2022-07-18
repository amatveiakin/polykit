// Old rank computations that are not ready to be factored out into tests, but could be
// useful for future reference.

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

#include "lib/gamma.h"
#include "lib/chern_arrow.h"
#include "lib/chern_cocycle.h"
#include "lib/polylog_cgrli.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_gr_space.h"

#include "lib/kappa.h"
#include "lib/polylog_type_d_space.h"


void computations_archive() {
  // const auto triplet_tmpl =
  //   + GammaExpr::single({Gamma({1,2}), Gamma({1,3})})
  //   + GammaExpr::single({Gamma({1,3}), Gamma({1,4})})
  //   + GammaExpr::single({Gamma({1,4}), Gamma({1,2})})
  // ;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // for (const int num_points : range_incl(4, 8)) {
  //   const int dimension = 3;
  //   const int weight = 3;
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto coords = combinations(points, dimension);
  //   Gr_NCoSpace space_comult;
  //   for (const auto& word : get_lyndon_words(coords, weight)) {
  //     const auto expr = GammaExpr::single(mapped(word, convert_to<Gamma>));
  //     const auto expr_pr = pr(expr);
  //     if (!expr_pr.is_zero()) {
  //       space_comult.push_back(ncomultiply(expr_pr, {1,2}));
  //     }
  //   }
  //   Gr_Space space_triplets;
  //   for (const auto& indices : permutations(slice(points, 1), 4)) {
  //     space_triplets.push_back(substitute_variables(triplet_tmpl, indices));
  //   }
  //   Gr_Space fx = mapped(GrFx(dimension, points), pr);
  //   Gr_NCoSpace second_space = space_ncoproduct(space_triplets, fx);
  //   const auto ranks = space_venn_ranks(
  //     space_comult,
  //     second_space,
  //     DISAMBIGUATE(identity_function)
  //   );
  //   std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  // }


  // const int dimension = 3;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // const int weight = 2;
  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Gr_Space l2 = mapped(GrL2(dimension, points), pr);
  //   Gr_Space l1 = mapped(GrFx(dimension, points), pr);
  //   // Gr_Space l2 = GrL2(dimension, points);
  //   // Gr_Space l1 = GrL1(dimension, points);
  //   Gr_NCoSpace space = mapped(
  //     cartesian_combinations(std::vector{std::pair{l2, 1}, std::pair{l1, weight-2}}),
  //     DISAMBIGUATE(ncoproduct_vec)
  //   );
  //   const auto ranks = space_mapping_ranks(
  //     space,
  //     DISAMBIGUATE(identity_function),
  //     DISAMBIGUATE(ncomultiply)
  //   );
  //   // const auto lambda3_rank = space_rank(
  //   //   mapped(combinations(l1, 3), DISAMBIGUATE(ncoproduct_vec)),
  //   //   DISAMBIGUATE(to_lyndon_basis)
  //   // );
  //   const int l1_rank = space_rank(l1, DISAMBIGUATE(to_lyndon_basis));
  //   const int lambda3_l1_rank = binomial(l1_rank, weight);
  //   // std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  //   std::cout << "p=" << num_points << ": " << lambda3_l1_rank << " - " << ranks.image();
  //   std::cout << " = " << (lambda3_l1_rank - ranks.image()) << "\n";
  // }


  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6};
  // const auto cl1 = CGrL_test_space(1, dimension, points);
  // const auto cl2 = CGrL_test_space(2, dimension, points);
  // const auto cl3 = CGrL_test_space(3, dimension, points);
  // const auto space = mapped(
  //   concat(
  //     cartesian_combinations(std::vector{std::pair{cl3, 1}, std::pair{cl1, 1}}),
  //     cartesian_combinations(std::vector{std::pair{cl2, 2}})
  //   ),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int weight = 4;
  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6};
  // const auto coords = combinations(slice(points, 1), dimension - 1);
  // Gr_ACoSpace space_words = mapped(
  //   gr_free_lie_coalgebra(weight, dimension, points),
  //   DISAMBIGUATE(expand_into_glued_pairs)
  // );
  // const auto space_l = mapped(
  //   cartesian_combinations(std::vector{
  //     std::pair{GrL2(dimension, points), 1},
  //     std::pair{GrL1(dimension, points), 1},
  //     std::pair{GrL1(dimension, points), 1},
  //   }),
  //   [](const auto& exprs) {
  //     return acoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_venn_ranks(
  //   space_words,
  //   space_l,
  //   DISAMBIGUATE(identity_function)
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int weight = 4;
  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6};
  // const auto coords = combinations(slice(points, 1), dimension - 1);
  // Gr_ACoSpace space_words = mapped(
  //   gr_free_lie_coalgebra(weight, dimension, points),
  //   // DISAMBIGUATE(expand_into_glued_pairs)
  //   [](const auto& expr) {
  //     return expand_into_glued_pairs(project_on(1, expr));
  //   }
  // );
  // const auto space_l = mapped(
  //   cartesian_combinations(std::vector{
  //     std::pair{GrL2(dimension, points), 1},
  //     std::pair{GrL1(dimension, points), 1},
  //     std::pair{GrL1(dimension, points), 1},
  //   }),
  //   // DISAMBIGUATE(acoproduct_vec)
  //   [](const auto& exprs) {
  //     return acoproduct_vec(mapped(exprs, [](const auto& expr) {
  //       return project_on(1, expr);
  //     }));
  //   }
  // );
  // const auto ranks = space_venn_ranks(
  //   space_words,
  //   space_l,
  //   DISAMBIGUATE(identity_function)
  // );
  // std::cout << to_string(ranks) << "\n";


  // for (const int weight : range_incl(2, 4)) {
  //   for (const int half_num_points : range_incl(2, 4)) {
  //     const auto points_raw = seq_incl(1, half_num_points);
  //     const auto& points_inv = concat(
  //       mapped(points_raw, [](const int idx) { return X(idx); }),
  //       mapped(points_raw, [](const int idx) { return -X(idx); })
  //     );
  //     const auto space = mapped(L(weight, points_inv), DISAMBIGUATE(to_lyndon_basis));
  //     const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
  //       return std::tuple{
  //         keep_non_weakly_separated_inv(expr),
  //         // ncomultiply(expr, {2,2}),
  //       };
  //     });
  //     std::cout << "w=" << weight << ", n=" << points_inv.size() << ": ";
  //     std::cout << to_string(ranks) << "\n";
  //   }
  // }


  // const std::vector points_inv = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // // const auto space_a = mapped(CL4(points_inv), DISAMBIGUATE(ncoproduct));
  // auto space_a = CL4(points_inv);
  // for (const int i : range(points_inv.size())) {
  //   auto p = points_inv;
  //   p[i] = Inf;
  //   append_vector(space_a, CL4(p));
  // }
  //
  // // const auto space_b = mapped(cartesian_power(L2({x1,x2,x3,x4,-x1}), 2), DISAMBIGUATE(ncoproduct_vec));
  // // const auto space_b = mapped(cartesian_power(L2({x1,x2,x3,x4,-x1,-x2,-x3,-x4}), 2), DISAMBIGUATE(ncoproduct_vec));
  // const auto new_func_comult =
  //   + ncoproduct(QLi2(x1,x2,x3,x4), QLi2(x4,-x1,-x4,x1))
  //   - ncoproduct(QLi2(x2,x3,x4,-x1), QLi2(-x1,-x2,x1,x2))
  //   + ncoproduct(QLi2(x3,x4,-x1,-x2), QLi2(-x2,-x3,x2,x3))
  //   - ncoproduct(QLi2(x4,-x1,-x2,-x3), QLi2(-x3,-x4,x3,x4))
  // ;
  // // const std::vector space_b = {new_func_comult};
  //
  // // const auto ranks = space_mapping_ranks(
  // //   concat(
  // //     space_a,
  // //     space_b
  // //   ),
  // //   // space_a,
  // //   DISAMBIGUATE(identity_function),
  // //   [](const auto& expr) {
  // //     if (expr.is_zero()) {
  // //       return std::tuple{
  // //         DeltaNCoExpr(),
  // //         DeltaNCoExpr(),
  // //       };
  // //     }
  // //     const int num_coparts = expr.element().first.size();  // TODO: add a helper function for this
  // //     if (num_coparts == 1) {
  // //       return std::tuple{
  // //         keep_non_weakly_separated_inv(expr),
  // //         ncomultiply(expr, {2,2}),
  // //       };
  // //     } else if (num_coparts == 2) {
  // //       return std::tuple{
  // //         DeltaNCoExpr(),
  // //         -expr,
  // //       };
  // //     } else {
  // //       FATAL(absl::StrCat("Unexpected num_coparts: ", num_coparts));
  // //     }
  // //   }
  // // );
  // // std::cout << to_string(ranks) << "\n";
  //
  // // const auto ranks = space_mapping_ranks(
  // //   mapped(space_a, DISAMBIGUATE(to_lyndon_basis)),
  // //   DISAMBIGUATE(identity_function),
  // //   DISAMBIGUATE(keep_non_weakly_separated_inv)
  // //   // [&](const auto& expr) {
  // //   //   return std::tuple{
  // //   //     keep_non_weakly_separated_inv(expr),
  // //   //     ncomultiply(expr, {2,2}) - new_func_comult,
  // //   //   };
  // //   // }
  // // );
  // // std::cout << to_string(ranks) << "\n";
  //
  // // const auto ranks = space_venn_ranks(
  // //   mapped(space_a, [](const auto& expr) { return ncomultiply(expr, {2,2}); }),
  // //   space_b,
  // //   DISAMBIGUATE(identity_function)
  // // );
  // // std::cout << to_string(ranks) << "\n";
  //
  // // CHECK(is_totally_weakly_separated_inv(QLi4(x1,x2,x3,x4,-x1,Inf)));
  // // std::cout << is_totally_weakly_separated_inv(QLi4(x1,x2,x3,x4,Inf,-x2)) << "\n";
  // const std::vector space = {
  //   QLi4(x1,x2,x3,x4,-x1,Inf),
  //   QLi4(x2,x3,x4,-x1,-x2,Inf),
  //   QLi4(x3,x4,-x1,-x2,-x3,Inf),
  //   QLi4(x4,-x1,-x2,-x3,-x4,Inf),
  //   QLi4(x1,x2,Inf,-x1,-x2,-x3),
  //   // QLi4(x1,x2,x3,x4,Inf,-x1),
  //   // QLi4(x2,x3,x4,-x1,Inf,-x2),
  //   // QLi4(x3,x4,-x1,-x2,Inf,-x3),
  //   // QLi4(x4,-x1,-x2,-x3,Inf,-x4),
  //   // QLi4(x1,x2,x3,Inf,x4,-x1),
  //   // QLi4(x2,x3,x4,Inf,-x1,-x2),
  //   // QLi4(x3,x4,-x1,Inf,-x2,-x3),
  //   // QLi4(x4,-x1,-x2,Inf,-x3,-x4),
  // };
  // // for (const auto& expr : space) {
  // //   CHECK(is_totally_weakly_separated_inv(expr));
  // // }
  // const auto ranks = space_mapping_ranks(
  //   mapped(space, DISAMBIGUATE(to_lyndon_basis)),
  //   DISAMBIGUATE(identity_function),
  //   [&](const auto& expr) {
  //     return std::tuple{
  //       // keep_non_weakly_separated_inv(expr),
  //       ncomultiply(expr, {2,2}) - new_func_comult,
  //     };
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";



}
