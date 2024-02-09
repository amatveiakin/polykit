// Old computations that are not ready to be factored out into tests, but could be
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
  //     std::identity{}
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
  //     std::identity{},
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
  //   std::identity{}
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
  //   std::identity{}
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
  //     const auto ranks = space_mapping_ranks(space, std::identity{}, [](const auto& expr) {
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
  // //   std::identity{},
  // //   [](const auto& expr) {
  // //     if (expr.is_zero()) {
  // //       return std::tuple{
  // //         DeltaNCoExpr(),
  // //         DeltaNCoExpr(),
  // //       };
  // //     }
  // //     const int num_coparts = expr.element().first.size();
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
  // //   std::identity{},
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
  // //   std::identity{}
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
  //   std::identity{},
  //   [&](const auto& expr) {
  //     return std::tuple{
  //       // keep_non_weakly_separated_inv(expr),
  //       ncomultiply(expr, {2,2}) - new_func_comult,
  //     };
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";


  // const auto fx = typeD_Fx();
  // const auto b2_generators = typeD_B2_generators();
  // const auto b2 = typeD_B2();
  // for (const int weight : range_incl(2, 6)) {
  //   const auto space_words = mapped(
  //     filtered(
  //       typeD_free_lie_coalgebra(weight),
  //       DISAMBIGUATE(is_totally_weakly_separated)
  //     ),
  //     DISAMBIGUATE(expand_into_glued_pairs)
  //   );
  //   const auto space_l = mapped(
  //     cartesian_combinations(concat(
  //       std::vector{std::pair{b2, 1}},
  //       std::vector(weight-2, std::pair{fx, 1})
  //     )),
  //     DISAMBIGUATE(acoproduct_vec)
  //   );
  //   const auto ranks = space_venn_ranks(
  //     space_words,
  //     space_l,
  //     std::identity{}
  //   );
  //   std::cout << "w=" << weight << ": " << to_string(ranks) << "\n";
  //   const auto new_space = mapped(b2_generators, [&](const auto& gen) {
  //     return expand_into_glued_pairs(NLog(weight, gen));
  //   });
  //   CHECK(space_contains(space_words, new_space, std::identity{}));
  //   CHECK(space_contains(space_l, new_space, std::identity{}));
  //   std::cout << "Contains: OK\n";
  // }


  // for (const int num_vars : range_incl(2, 4)) {
  //   for (const int weight : range_incl(2, 5)) {
  //     const auto& args = concat(
  //       mapped(range_incl(1, num_vars), [](const int idx) { return X(idx); }),
  //       mapped(range_incl(1, num_vars), [](const int idx) { return -X(idx); })
  //       // std::vector{Inf}
  //     );
  //     auto space = L(weight, args);
  //     space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       std::identity{},
  //       DISAMBIGUATE(keep_non_weakly_separated_inv)
  //     );
  //     std::cout << "p=" << args.size() << "(" << num_vars << "), w=" << weight << ": ";
  //     std::cout << to_string(ranks) << "\n";
  //   }
  // }

  // const int weight = 4;
  // auto space = typeC_CL(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5});
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // space.push_back(typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}));
  // space.push_back(typeC_QLi(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5}));
  // space.push_back(typeC_QLi(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5}));
  // space.push_back(typeC_QLi(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5}));
  // space.push_back(typeC_QLi(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5}));
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // // const auto prepare = [](const auto& expr) {
  // //   return to_lyndon_basis(project_on_x1(expr)).filtered([](const auto& term) {
  // //     return absl::c_count(term, Zero) == 0;
  // //   });
  // // };
  // // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // // const int weight = 4;
  //
  // // std::cout << prepare(
  // //   +  typeC_QLi(weight, points)
  // //   -4*QLi3(x1,-x2,-x3,-x4)
  // //   -4*QLi3(x1,x2,x3,-x4)
  // // );
  // // std::cout << prepare(QLi3(x1,x2,x3,-x1));
  // // std::cout << prepare(QLi3(-x4,x1,x2,x4));
  // // std::cout << prepare(QLi3(-x3,-x4,x1,x3));
  // // std::cout << prepare(QLi3(x1,-x1,-x3,-x4));
  // // std::cout << prepare(QLi3(x2,x1,-x4,-x2));
  // // std::cout << prepare(QLi3(x3,x2,x1,-x3));
  //
  // // std::cout << prepare(
  // //   +  typeC_QLi(weight, points)
  // //   -4*QLi3(x1,-x2,-x3,-x4)
  // //   -4*QLi3(x1,x2,x3,-x4)
  // //   +4*QLi3(x1,x2,x3,-x1)
  // //   +4*QLi3(-x4,x1,x2,x4)
  // //   +4*QLi3(x2,x1,-x4,-x2)
  // // );
  //
  // // std::cout << to_lyndon_basis(
  // //   + typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  // //   - typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1})
  // //   + 2 * (
  // //     + QLi3(x1,x2,x3,x4)
  // //     - QLi3(x2,x3,x4,-x1)
  // //     + QLi3(x3,x4,-x1,-x2)
  // //     - QLi3(x4,-x1,-x2,-x3)
  // //     + QLi3(-x1,-x2,-x3,-x4)
  // //     - QLi3(-x2,-x3,-x4,x1)
  // //     + QLi3(-x3,-x4,x1,x2)
  // //     - QLi3(-x4,x1,x2,x3)
  // //   )
  // // );

  // for (const int weight : range_incl(2, 7)) {
  //   const auto space = mapped_parallel(
  //     typeC_CL(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}), DISAMBIGUATE(to_lyndon_basis)
  //   );
  //   const auto a = to_lyndon_basis(typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}));
  //   const auto b = to_lyndon_basis(typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1}));
  //   std::cout << "w=" << weight << "\n";
  //   std::cout << "diff lies in = " << space_contains(space, {a - b}, std::identity{}) << "\n";
  //   std::cout << "sum lies in = " << space_contains(space, {a + b}, std::identity{}) << "\n";
  //   std::cout << "\n";
  // }


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
  // const auto ranks = space_venn_ranks(space, {expr}, std::identity{});
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
  //
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
  //
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
  // const auto rank = space_rank(space, std::identity{});
  // profiler.finish("rank");
  // std::cout << rank << "\n";


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
  //       a += sign * pullback(GLiVec(weight, main_points), pb_point);
  //     }
  //   }
  // }
  // profiler.finish("a");
  // GammaExpr b;
  // for (const int p: range(num_points)) {
  //   const int sign = neg_one_pow(p);
  //   b += sign * GLiVec(weight, removed_index(points, p));
  // }
  // profiler.finish("b");
  // const auto sum = to_lyndon_basis(a - b);
  // profiler.finish("lyndon");
  // std::cout << sum;


  // const auto expr = GLi4(1,2,3,4,5,6,7,8);
  // Gr_Space space;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // for (const int pb_arg : range_incl(1, 3)) {
  //   const auto main_args_pool = removed_index(points, pb_arg - 1);
  //   for (const auto& main_args : combinations(main_args_pool, 6)) {
  //     space.push_back(pullback(GLiVec(4, main_args), {pb_arg}));
  //   }
  // }
  // const auto ranks = space_venn_ranks(space, {expr}, [](const auto& expr) {
  //   return to_lyndon_basis(chern_arrow_left(expr, 9));
  // });
  // std::cout << to_string(ranks) << "\n";


  // const std::vector wedge_chern_space = {
  //   // ncoproduct(GLi2(1,2,3,4), G({1,2})),
  //   // ncoproduct(GLi2(1,2,3,4), G({1,4})),
  //   // ncoproduct(GLi2(1,2,3,4), G({1,5})),
  //   // ncoproduct(GLi2(1,2,3,4), G({2,3})),
  //   ncoproduct(GLi2(1,2,3,4), G({3,4})),
  //   ncoproduct(GLi2(1,2,3,4), G({4,5})),
  //   // ncoproduct(GLi2(1,2,3,5), G({1,2})),
  //   // ncoproduct(GLi2(1,2,3,5), G({1,5})),
  //   // ncoproduct(GLi2(1,2,3,5), G({2,3})),
  //   // ncoproduct(GLi2(1,2,3,5), G({3,4})),
  //   ncoproduct(GLi2(1,2,3,5), G({3,5})),
  //   ncoproduct(GLi2(1,2,3,5), G({4,5})),
  //   // ncoproduct(GLi2(1,2,4,5), G({1,2})),
  //   // ncoproduct(GLi2(1,2,4,5), G({1,5})),
  //   // ncoproduct(GLi2(1,2,4,5), G({2,3})),
  //   // ncoproduct(GLi2(1,2,4,5), G({2,4})),
  //   // ncoproduct(GLi2(1,2,4,5), G({3,4})),
  //   // ncoproduct(GLi2(1,2,4,5), G({4,5})),
  //   ncoproduct(GLi2(1,3,4,5), G({1,2})),
  //   // ncoproduct(GLi2(1,3,4,5), G({1,3})),
  //   ncoproduct(GLi2(1,3,4,5), G({1,5})),
  //   // ncoproduct(GLi2(1,3,4,5), G({2,3})),
  //   ncoproduct(GLi2(1,3,4,5), G({3,4})),
  //   ncoproduct(GLi2(1,3,4,5), G({4,5})),
  //   ncoproduct(GLi2(2,3,4,5), G({1,2})),
  //   // ncoproduct(GLi2(2,3,4,5), G({1,5})),
  //   // ncoproduct(GLi2(2,3,4,5), G({2,3})),
  //   ncoproduct(GLi2(2,3,4,5), G({2,5})),
  //   ncoproduct(GLi2(2,3,4,5), G({3,4})),
  //   ncoproduct(GLi2(2,3,4,5), G({4,5})),
  // };
  // const std::vector wedge_chern_space = {
  //   // ncoproduct(GLi2(1,2,3,4), G({1,2})),
  //   // ncoproduct(GLi2(1,2,3,4), G({1,4})),
  //   // ncoproduct(GLi2(1,2,3,4), G({1,5})),
  //   // ncoproduct(GLi2(1,2,3,4), G({2,3})),
  //   // ncoproduct(GLi2(1,2,3,4), G({3,4})), #
  //   // ncoproduct(GLi2(1,2,3,4), G({4,5})), *
  //   // ncoproduct(GLi2(1,2,3,5), G({1,2})),
  //   // ncoproduct(GLi2(1,2,3,5), G({1,5})),
  //   // ncoproduct(GLi2(1,2,3,5), G({2,3})),
  //   ncoproduct(GLi2(1,2,3,5), G({3,4})),
  //   ncoproduct(GLi2(1,2,3,5), G({3,5})),
  //   // ncoproduct(GLi2(1,2,3,5), G({4,5})), *
  //   // ncoproduct(GLi2(1,2,4,5), G({1,2})),
  //   // ncoproduct(GLi2(1,2,4,5), G({1,5})),
  //   // ncoproduct(GLi2(1,2,4,5), G({2,3})),
  //   // ncoproduct(GLi2(1,2,4,5), G({2,4})),
  //   ncoproduct(GLi2(1,2,4,5), G({3,4})),
  //   ncoproduct(GLi2(1,2,4,5), G({4,5})),
  //   ncoproduct(GLi2(1,3,4,5), G({1,2})),
  //   // ncoproduct(GLi2(1,3,4,5), G({1,3})),
  //   ncoproduct(GLi2(1,3,4,5), G({1,5})),
  //   // ncoproduct(GLi2(1,3,4,5), G({2,3})),
  //   // ncoproduct(GLi2(1,3,4,5), G({3,4})), #
  //   // ncoproduct(GLi2(1,3,4,5), G({4,5})), *
  //   ncoproduct(GLi2(2,3,4,5), G({1,2})),
  //   // ncoproduct(GLi2(2,3,4,5), G({1,5})),
  //   // ncoproduct(GLi2(2,3,4,5), G({2,3})),
  //   ncoproduct(GLi2(2,3,4,5), G({2,5})),
  //   // ncoproduct(GLi2(2,3,4,5), G({3,4})), #
  //   // ncoproduct(GLi2(2,3,4,5), G({4,5})), *
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
  // const auto ranks = space_venn_ranks(space, {pair}, std::identity{});
  // std::cout << to_string(ranks) << "\n";


  // const auto prepare = [](const GammaExpr& expr) {
  //   return prnt::with_object_to_string(
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
  // auto lhs = to_lyndon_basis(GLi3(1,2,3,4,5,6));
  // // lhs = lhs.filtered([](const auto& term) {
  // //   return absl::c_all_of(
  // //     transposed(mapped(term, [](const Gamma& g) { return g.index_vector(); })),
  // //     DISAMBIGUATE(is_strictly_increasing)
  // //   );
  // // });
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
  // std::cout << prepare(lhs - rhs);  // NOT ZERO: fitting not finished


  // Profiler profiler;
  // const auto expr = GLi5(1,2,3,4,5,6,7,8);
  // profiler.finish("expr");
  // auto coexpr = ncomultiply(expr, {1,4});
  // profiler.finish("comult");
  // std::cout << "\n";
  // // This zeroes out terms where we take three points from {1,2,3,4} and one point from {5,6,7,8}.
  // // Should also do vice versa to the the full equation.
  // coexpr +=
  //   + ncoproduct(GLi4(1,2,3,4,5,6,7,8), plucker({1,2,3,4}))
  //   + ncoproduct(
  //     + GLiVec(4, {5}, {1,2,4,6,7,8})
  //     - GLiVec(4, {6}, {1,2,4,5,7,8})
  //     + GLiVec(4, {7}, {1,2,4,5,6,8})
  //     ,
  //     plucker({1,2,4,8})
  //   )
  //   - ncoproduct(
  //     + GLiVec(4, {5}, {1,3,4,6,7,8})
  //     - GLiVec(4, {6}, {1,3,4,5,7,8})
  //     + GLiVec(4, {7}, {1,3,4,5,6,8})
  //     ,
  //     plucker({1,3,4,8})
  //   )
  //   + ncoproduct(
  //     + GLiVec(4, {5}, {2,3,4,6,7,8})
  //     - GLiVec(4, {6}, {2,3,4,5,7,8})
  //     + GLiVec(4, {7}, {2,3,4,5,6,8})
  //     ,
  //     plucker({2,3,4,8})
  //   )
  //   + ncoproduct(
  //     + GLiVec(4, {5}, {1,2,4,6,7,8})
  //     - GLiVec(4, {5}, {1,3,4,6,7,8})
  //     + GLiVec(4, {5}, {2,3,4,6,7,8})
  //     ,
  //     plucker({1,2,3,5})
  //   )
  //   - ncoproduct(
  //     + GLiVec(4, {6}, {1,2,4,5,7,8})
  //     - GLiVec(4, {6}, {1,3,4,5,7,8})
  //     + GLiVec(4, {6}, {2,3,4,5,7,8})
  //     ,
  //     plucker({1,2,3,6})
  //   )
  //   + ncoproduct(
  //     + GLiVec(4, {7}, {1,2,4,5,6,8})
  //     - GLiVec(4, {7}, {1,3,4,5,6,8})
  //     + GLiVec(4, {7}, {2,3,4,5,6,8})
  //     ,
  //     plucker({1,2,3,7})
  //   )
  //   - ncoproduct(GLiVec(4, {5}, {1,2,4,6,7,8}), plucker({1,2,4,5}))
  //   + ncoproduct(GLiVec(4, {6}, {1,2,4,5,7,8}), plucker({1,2,4,6}))
  //   - ncoproduct(GLiVec(4, {7}, {1,2,4,5,6,8}), plucker({1,2,4,7}))
  //   + ncoproduct(GLiVec(4, {5}, {1,3,4,6,7,8}), plucker({1,3,4,5}))
  //   - ncoproduct(GLiVec(4, {6}, {1,3,4,5,7,8}), plucker({1,3,4,6}))
  //   + ncoproduct(GLiVec(4, {7}, {1,3,4,5,6,8}), plucker({1,3,4,7}))
  //   - ncoproduct(GLiVec(4, {5}, {2,3,4,6,7,8}), plucker({2,3,4,5}))
  //   + ncoproduct(GLiVec(4, {6}, {2,3,4,5,7,8}), plucker({2,3,4,6}))
  //   - ncoproduct(GLiVec(4, {7}, {2,3,4,5,6,8}), plucker({2,3,4,7}))
  //   - ncoproduct(
  //     + GLiVec(4, {1,2,3,4,5,6,7,8})
  //     + GLiVec(4, {5}, {2,3,4,6,7,8})
  //     - GLiVec(4, {5}, {1,3,4,6,7,8})
  //     + GLiVec(4, {5}, {1,2,4,6,7,8})
  //     - GLiVec(4, {6}, {2,3,4,5,7,8})
  //     + GLiVec(4, {6}, {1,3,4,5,7,8})
  //     - GLiVec(4, {6}, {1,2,4,5,7,8})
  //     + GLiVec(4, {7}, {2,3,4,5,6,8})
  //     - GLiVec(4, {7}, {1,3,4,5,6,8})
  //     + GLiVec(4, {7}, {1,2,4,5,6,8})
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


  // for (const int dimension : range_incl(3, 4)) {
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
  //         // to_lyndon_basis(expr + neg_one_pow(dimension) * plucker_dual(expr, points)),
  //       };
  //     }
  //   );
  //   std::cout << to_string(ranks) << "\n";
  // }


  // // TODO: Factor out sigma_i (a.k.a "co-degeneration maps")
  // //   But first we need to figure out the story about propagating variable space information.
  // //   Some functions (cherns arrows, a+, a-, b+, b-) expect one-based numeration, while this
  // //   function expects 0-based. Also both of them need `dst_vars` to be passed explicitly,
  // //   which is not really convenient.
  // static constexpr auto sigma = [](int i, const auto& expr, int dst_vars) {
  //   CHECK_LT(i, dst_vars);
  //   return substitute_variables_0_based(expr, concat(seq_incl(0, i), seq(i, dst_vars)));
  // };
  // static constexpr auto add_to_each_multiple = [](X new_var, const auto& expr) {
  //   return expr.mapped_expanding([&](const auto& term) {
  //     return tensor_product(absl::MakeConstSpan(mapped(term, [&](const X var) {
  //       return
  //         + ProjectionExpr::single({var})
  //         + ProjectionExpr::single({new_var})
  //       ;
  //     })));
  //   });
  // };
  // static constexpr auto get_unsorted_partitions_allow_zero = [](int n, int num_summands) {
  //   const int q = n + num_summands - 1;
  //   return mapped(combinations(seq_incl(1, q), num_summands - 1), [&](const auto& s) {
  //     return mapped(diffs(concat(std::vector{0}, s, std::vector{q+1})), [](const int k) {
  //       return k - 1;
  //     });
  //   });
  // };
  // static constexpr auto kernel_element = [](const int weight, const int num_points) {
  //   return sum(get_unsorted_partitions_allow_zero(weight - num_points + 1, num_points), [&](const auto& subweights) {
  //     return tensor_product(absl::MakeConstSpan(mapped(range(num_points * 2 - 1), [&](const int k) {
  //       const int i = k / 2;
  //       return (k % 2 == 0)
  //         ? ProjectionExpr::single(std::vector(subweights.at(i), X(i)))
  //         : ProjectionExpr::single({X(i)}) - ProjectionExpr::single({X(i + 1)})
  //       ;
  //     })));
  //   });
  // };
  // for (const int num_points : range_incl(2, 6)) {
  //   for (const int weight : range_incl(2, 6)) {
  //     const int m = num_points - 1;
  //     const auto coords = seq(num_points);
  //     // const auto space = mapped(get_lyndon_words(coords, weight), [](const auto& word) {
  //     //   return ProjectionExpr::single(mapped(word, convert_to<X>));
  //     // });
  //     const auto space = mapped(combinations_with_replacement(coords, weight), [](const auto& word) {
  //       return ProjectionExpr::single(mapped(word, convert_to<X>));
  //     });
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(to_lyndon_basis),
  //       [&](const auto& expr) {
  //         return concat(
  //           mapped(
  //             range(m),
  //             [&](const int i) { return to_lyndon_basis(sigma(i, expr, m)); }
  //           ),
  //           mapped(
  //             range(num_points),
  //             [&](const int i) { return to_lyndon_basis(expr - add_to_each_multiple(i, expr)); }
  //           )
  //         );
  //       }
  //     );
  //     std::cout << "m=" << m << ", w=" << weight << ": " << to_string(ranks) << "\n";
  //     const auto expr = kernel_element(weight, num_points - 1);
  //     for (const int i : range(m - 1)) {
  //       CHECK(to_lyndon_basis(sigma(i, expr, m)).is_zero()) << i;
  //     }
  //     for (const int i : range(num_points - 1)) {
  //       CHECK(to_lyndon_basis(expr - add_to_each_multiple(i, expr)).is_zero());
  //     }
  //     // std::cout << "kernel ok\n";
  //   }
  // }


  // const auto cocycle = a_full(ChernCocycle(4, 1, {1,2,3,4,5}), 6);
  // // const auto space = mapped(
  // //   space_ncoproduct(
  // //     ChernGrL(2, 2, {1,2,3,4,5,6}),
  // //     GrFx(2, {1,2,3,4,5,6}),
  // //     GrFx(2, {1,2,3,4,5,6})
  // //   ),
  // //   DISAMBIGUATE(ncomultiply)
  // // );
  // // const auto frozen_vars = mapped(range_incl(1, 6), [](const auto a) {
  // //   return plucker({a, a % 6 + 1});
  // // });
  // // Gr_NCoSpace space;
  // // for (const int i : range(6)) {
  // //   const auto points = removed_index(seq_incl(1, 6), i);
  // //   append_vector(
  // //     space,
  // //     space_ncoproduct(
  // //       ChernGrL(2, 2, points),
  // //       GrFx(2, points),
  // //       frozen_vars
  // //     )
  // //   );
  // // }
  // // space = mapped(space, DISAMBIGUATE(ncomultiply));
  // // const auto ranks = space_venn_ranks(space, {cocycle}, std::identity{});
  // // std::cout << to_string(ranks) << "\n";
  //
  // const auto frozen_vars = [](int num_points) {
  //   return mapped(range_incl(1, num_points), [&](const auto a) {
  //     return plucker({a, a % num_points + 1});
  //   });
  // };
  // const auto chern_space = ChernGrL(2, 2, {1,2,3,4});
  // const auto frozen_vars_4 = frozen_vars(4);
  // const auto frozen_vars_5 = frozen_vars(5);
  // // const auto frozen_vars_6 = frozen_vars(6);
  // const auto frozen_vars_6 = space_ncoproduct(std::vector{
  //   plucker({1,2}),
  //   plucker({2,3}),
  //   // plucker({3,4}),
  //   plucker({4,5}),
  //   plucker({5,6}),
  //   // plucker({6,1}),
  // });
  // Gr_NCoSpace space;
  // for (const auto& i_inner : range(5)) {
  //   for (const auto& i_outer : range(6)) {
  //     // `inner` and `outer` are effectively a_i functions.
  //     const auto inner = [&](const auto& expr) {
  //       return substitute_variables_1_based(expr, removed_index(seq_incl(1, 5), i_inner));
  //     };
  //     const auto outer = [&](const auto& expr) {
  //       return substitute_variables_1_based(expr, removed_index(seq_incl(1, 6), i_outer));
  //     };
  //     const auto apply_inner = [&](const auto& space) { return mapped(space, inner); };
  //     const auto apply_outer = [&](const auto& space) { return mapped(space, outer); };
  //     // append_vector(space,
  //     //   apply_outer(
  //     //     apply_inner(
  //     //       space_ncoproduct(
  //     //         chern_space,
  //     //         frozen_vars_4,
  //     //         frozen_vars_4
  //     //       )
  //     //     )
  //     //   )
  //     // );
  //     append_vector(space,
  //       apply_outer(
  //         space_ncoproduct(
  //           apply_inner(
  //             space_ncoproduct(
  //               chern_space,
  //               frozen_vars_4
  //             )
  //           ),
  //           frozen_vars_5
  //         )
  //       )
  //     );
  //     // append_vector(space,
  //     //   apply_outer(
  //     //     space_ncoproduct(
  //     //       apply_inner(
  //     //         chern_space
  //     //       ),
  //     //       frozen_vars_5,
  //     //       frozen_vars_5
  //     //     )
  //     //   )
  //     // );
  //     // append_vector(space,
  //     //   space_ncoproduct(
  //     //     apply_outer(
  //     //       apply_inner(
  //     //         space_ncoproduct(
  //     //           chern_space,
  //     //           frozen_vars_4
  //     //         )
  //     //       )
  //     //     ),
  //     //     frozen_vars_6
  //     //   )
  //     // );
  //     append_vector(space,
  //       space_ncoproduct(
  //         apply_outer(
  //           space_ncoproduct(
  //             apply_inner(
  //               chern_space
  //             ),
  //             frozen_vars_5
  //           )
  //         ),
  //         frozen_vars_6
  //       )
  //     );
  //     // append_vector(space,
  //     //   space_ncoproduct(
  //     //     apply_outer(
  //     //       apply_inner(
  //     //         chern_space
  //     //       )
  //     //     ),
  //     //     frozen_vars_6,
  //     //     frozen_vars_6
  //     //   )
  //     // );
  //   }
  // }
  // space = mapped(space, DISAMBIGUATE(ncomultiply));
  // const auto ranks = space_venn_ranks(space, {cocycle}, std::identity{});
  // std::cout << to_string(ranks) << "\n";
}
