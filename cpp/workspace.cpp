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


Gr_Space CGrL3_Dim3_Depth1_test_space(const std::vector<int>& points) {
  const int weight = 3;
  Gr_Space space;
  for (const int bonus_point_idx : range(points.size())) {
    const auto bonus_args = choose_indices(points, {bonus_point_idx});
    const auto main_args = removed_index(points, bonus_point_idx);
    append_vector(space, mapped(CL(weight, main_args), [&](const auto& expr) {
      return pullback(expr, bonus_args);
    }));
  }
  return space;
}

int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_unicode_version(UnicodeVersion::simple)
    .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    // .set_expression_line_limit(100)
    .set_annotation_sorting(AnnotationSorting::length)
    // .set_annotation_sorting(AnnotationSorting::lexicographic)
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


  // const std::regex line_re("(-?[0-9]+) *([0-9]+)");
  // std::ifstream input("../inputs/A3.m", std::ios::in);
  // DeltaExpr a_expr;
  // for (std::string line; std::getline(input, line); ) {
  //   trim(line);
  //   std::smatch match;
  //   CHECK(std::regex_match(line, match, line_re));
  //   CHECK_EQ(match.size(), 2+1);
  //   const int coeff = std::stoi(match[1].str());
  //   const auto& term_str = match[2].str();
  //   CHECK(term_str.size() % 2 == 0);
  //   std::vector<Delta> term;
  //   for (int i = 0; i < term_str.size(); i += 2) {
  //     term.push_back(Delta(term_str[i] - '0', term_str[i + 1] - '0'));
  //   }
  //   a_expr.add_to(term, coeff);
  // }

  // // int d = a_expr.element().second;
  // // for (const auto& [_, coeff] : a_expr) {
  // //   d = std::gcd(d, coeff);
  // // }
  // // std::cout << "GCD = " << d << "\n";  // == 16
  // a_expr.div_int(16);
  // // std::cout << a_expr;
  // // std::cout << to_lyndon_basis(a_expr);

  // const auto a = [&](const std::vector<int>& points) {
  //   return substitute_variables_1_based(a_expr, points).annotate(
  //     fmt::function_num_args("a", points)
  //   );
  // };

  // // std::cout << a({1,1,2,2,1,2});
  // std::cout << a({1,2,3,4,5,6}) - a({2,3,4,5,6,1});

  // // const auto expr = a({1,1,1,2,2,3});
  // // std::cout << to_lyndon_basis(expr);

  // // const auto space = LInf(6, {1,2,3,Inf});
  // // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // // std::cout << to_string(ranks) << "\n";
  // // std::cout << space.front();

  // // const auto space = L(6, to_vector(range_incl(1, 6)));
  // // const auto ranks = space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis));
  // // std::cout << to_string(ranks) << "\n";



  // const int num_points = 11;
  // const int num_args = num_points / 2 - 1;
  // auto source = sum_looped_vec(
  //   [&](const auto& args) {
  //     return LiQuad(num_points / 2 - 1, args);
  //   },
  //   num_points,
  //   seq_incl(1, num_points - 1)
  // );

  // auto expr = theta_expr_to_lira_expr_without_products(source.without_annotations());

  // StringExpr stats;

  // constexpr char kInvalidInput[] = "Invalid input: ";
  // bool short_form_ratios = true;
  // std::cout << "Functional\n" << source.annotations() << "\n";
  // while (true) {
  //   std::vector<std::vector<int>> balls;
  //   std::unique_ptr<Snowpal> snowpal;
  //   auto reset_snowpal = [&]() {
  //     snowpal = absl::make_unique<Snowpal>(expr, num_points);
  //     for (const auto& b : balls) {
  //       snowpal->add_ball(b);
  //     }
  //   };
  //   reset_snowpal();
  //   std::cout << expr;
  //   while (true) {
  //     std::cout << "\n> ";
  //     std::string input;
  //     std::getline(std::cin, input);
  //     trim(input);
  //     if (input.empty()) {
  //       continue;
  //     } else if (input == "q" || input == "quit") {
  //       return 0;
  //     } else if (input == "sf" || input == "short_forms") {
  //       short_form_ratios = !short_form_ratios;
  //       if (short_form_ratios) {
  //         std::cout << "Short form cross-ratios: enabled\n";
  //       } else {
  //         std::cout << "Short form cross-ratios: disabled\n";
  //       }
  //       to_ostream(std::cout, *snowpal, short_form_ratios);
  //       continue;
  //     } else if (input == "r" || input == "reset") {
  //       break;
  //     } else if (input == "b" || input == "back") {
  //       if (balls.empty()) {
  //         std::cout << kInvalidInput << "nothing to remove\n";
  //         continue;
  //       }
  //       balls.pop_back();
  //       reset_snowpal();
  //       to_ostream(std::cout, *snowpal, short_form_ratios);
  //       continue;
  //     }
  //     std::vector<int> ball;
  //     try {
  //       for (const auto& var_str : absl::StrSplit(input, " ", absl::SkipEmpty())) {
  //         const int var = std::stoi(std::string(var_str));
  //         if (var < 1 || var > num_points) {
  //           throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
  //         }
  //         ball.push_back(var);
  //       }
  //       // for (const char ch : input) {
  //       //   if (std::isspace(ch)) {
  //       //     continue;
  //       //   }
  //       //   const int var = std::stoi(std::string(1, ch));
  //       //   if (var < 1 || var > num_points) {
  //       //     throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
  //       //   }
  //       //   ball.push_back(var);
  //       // }
  //     } catch (const std::exception& e) {
  //       std::cout << kInvalidInput << e.what() << "\n";
  //       continue;
  //     }
  //     try {
  //       snowpal->add_ball(ball);
  //       balls.push_back(ball);
  //       to_ostream(std::cout, *snowpal, short_form_ratios);
  //     } catch (const IllegalTreeCutException& e) {
  //       std::cout << kInvalidInput << e.what() << "\n";
  //       reset_snowpal();
  //     }
  //   }
  // }


  // for (const auto& expr : GrL1(3, {1,2,3,4,5,6})) {
  //   std::cout << expr;
  // }
  // std::cout << GrL1(3, {1,2,3,4,5,6}).size() << "\n";
  // std::cout << GrLog(1)(2,3,4,5) << "\n";


  // Gr_Space space;
  // for (const auto x : range_incl(1, 6)) {
  //   for (const auto& p : combinations(removed_index(seq_incl(1, 6), x - 1), 4)) {
  //     space.push_back(GrLogVec({x}, p));
  //   }
  // }
  // std::cout << dump_to_string(space) << "\n";
  // absl::flat_hash_map<GammaExpr, std::vector<std::string>> groups;
  // for (const auto& exprs : combinations(space, 2)) {
  //   const auto [a, b] = to_array<2>(exprs);
  //   for (const int sign : {-1, 1}) {
  //     const auto s =
  //       annotations_one_liner(a.annotations()) +
  //       (sign == -1 ? " - " : " + ") +
  //       annotations_one_liner(b.annotations());
  //     groups[a + sign * b].push_back(s);
  //   }
  // }
  // const auto groups_sorted = sorted_by_projection(
  //   to_vector(groups),
  //   [](const auto& g) { return g.second.size(); }
  // );
  // size_t sep_len = 0;
  // for (const auto& group : groups_sorted) {
  //   for (const auto& expr : group.second) {
  //     sep_len = std::max(sep_len, expr.length());
  //   }
  // }
  // const std::string sep(sep_len, '-');
  // std::cout << sep << "\n";
  // for (const auto& group : groups_sorted) {
  //   for (const auto& expr : group.second) {
  //     std::cout << expr << "\n";
  //   }
  //   std::cout << sep << "\n";
  // }

  // const std::vector space {
  //   // full rank
  //   GrLog(2)(1,4,5,6) + GrLog(6)(1,2,3,4),
  //   GrQLi1(2)(1,4,5,6) - GrQLi1(6)(2,3,4,1),
  //   GrQLi1(2)(3,4,5,6) - GrQLi1(4)(2,3,6,1),
  //   GrQLi1(4)(1,2,5,6) - GrQLi1(6)(2,3,4,5),
  //
  //   // corank 1
  //   // GrQLi1(1)(2,3,4,6) - GrQLi1(4)(3,5,6,1),
  //   // GrQLi1(1)(2,3,5,6) - GrQLi1(5)(3,4,6,1),
  //   // GrQLi1(2)(1,3,4,6) - GrQLi1(4)(3,5,6,2),
  //   // GrQLi1(2)(1,3,5,6) - GrQLi1(5)(3,4,6,2),
  // };
  // std::cout << space_rank(space, identity_function) << "\n";


  // struct Args {
  //   std::vector<int> bonus_points;
  //   std::vector<int> log_points;
  //   bool operator==(const Args& other) const { return bonus_points == other.bonus_points && log_points == other.log_points; }
  // };
  // struct ArgsAndLog {
  //   Args args;
  //   GammaExpr log_expr;
  // };
  // struct ArgsPair {
  //   Args a;
  //   Args b;
  //   int b_sign;
  // };

  // Profiler profiler;
  // const int dim = 3;
  // const int num_points = 9;
  // const auto points = seq_incl(1, num_points);
  // std::vector<ArgsAndLog> logs;
  // for (const auto& bonus_p : combinations(points, dim - 2)) {
  //   for (const auto& log_p : combinations(removed_indices_one_based(points, bonus_p), 4)) {
  //     const auto args = Args{bonus_p, log_p};
  //     const auto expr = GrLogVec(bonus_p, log_p);
  //     logs.push_back(ArgsAndLog{args, expr});
  //   }
  // }
  // profiler.finish("expressions");
  // absl::flat_hash_map<GammaExpr, std::vector<ArgsPair>> groups;
  // for (const auto& pair : combinations(logs, 2)) {
  //   const auto [a, b] = to_array<2>(pair);
  //   for (const int sign : {-1, 1}) {
  //     groups[a.log_expr + sign * b.log_expr].push_back(ArgsPair{a.args, b.args, sign});
  //   }
  // }
  // profiler.finish("groups");
  // const auto groups_sorted = sorted_by_projection(
  //   to_vector(groups),
  //   [](const auto& g) { return g.second.size(); }
  // );
  // profiler.finish("sorting");
  // const std::string sep = "-------------------------------";
  // std::cout << sep << "\n";
  // std::vector<ArgsPair> linked;
  // int total_interesting_groups = 0;
  // for (const auto& [expr, group] : groups_sorted) {
  //   Gr_Space space;
  //   space.push_back(expr);
  //   for (const auto& args_pair : group) {
  //     const auto& args_a = args_pair.a;
  //     const auto& args_b = args_pair.b;
  //     const auto b_sign = args_pair.b_sign;
  //     const auto& b_log_points = b_sign == 1
  //       ? rotated_vector(args_b.log_points, 1)
  //       : args_b.log_points;
  //     space.push_back(
  //       + GrQLiVec(1, args_a.bonus_points, args_a.log_points)
  //       - GrQLiVec(1, args_b.bonus_points, b_log_points)
  //     );
  //   }
  //   const auto rank = space_rank(space, identity_function);
  //   const int corank = space.size() - rank;
  //   if (corank > 0) {
  //     total_interesting_groups++;
  //     append_vector(linked, group);
  //     for (const auto& expr : space) {
  //       std::cout << annotations_one_liner(expr.annotations()) << "\n";
  //     }
  //     std::cout << "corank = " << corank << "\n";
  //     std::cout << sep << "\n";
  //   }
  // }
  // profiler.finish("ranks");
  // std::cout << "Total interesting groups: " << total_interesting_groups << "\n";

  // absl::flat_hash_map<GammaExpr, std::vector<std::array<Args, 3>>> triplets;
  // for (const auto& pair : combinations(linked, 2)) {
  //   const auto& [xy, yz] = to_array<2>(pair);
  //   if (xy.b == yz.a) {
  //     const auto& x = xy.a;
  //     const auto& y = xy.b;
  //     const auto& z = yz.b;
  //     const int x_sign = 1;
  //     const int y_sign = xy.b_sign;
  //     const int z_sign = xy.b_sign * yz.b_sign;
  //     const auto expr =
  //       + x_sign * GrLogVec(x.bonus_points, x.log_points)
  //       + y_sign * GrLogVec(y.bonus_points, y.log_points)
  //       + z_sign * GrLogVec(z.bonus_points, z.log_points)
  //     ;
  //     const std::array triplet = {xy.a, xy.b, yz.b};
  //     // const auto expr = sum(mapped(triplet, [](const auto& args) {
  //     //   return GrLogVec(args.bonus_points, args.log_points);
  //     // }));
  //     triplets[expr].push_back(triplet);
  //   }
  // }
  // profiler.finish("triplets");
  // std::cout << sep << "\n";
  // for (const auto& [expr, triplets] : triplets) {
  //   if (triplets.size() <= 2) {
  //     continue;
  //   }
  //   for (const auto& triplet : triplets) {
  //     for (const auto& args : triplet) {
  //       std::cout << dump_to_string(args.bonus_points) << " / " << dump_to_string(args.log_points) << ",  ";
  //     }
  //     std::cout << "\n";
  //   }
  //   std::cout << sep << "\n";
  // }



  // const auto points = seq_incl(1, 5);
  // const auto space = GrL1(2, points);
  // const auto s2 = space_ncoproduct(space, space);
  // std::cout << dump_to_string(space) << "\n";
  // std::cout << space_rank(s2, identity_function) << "\n";
  // // for (const auto& expr : s2) {
  // //   std::cout << expr;
  // //   std::cout << "\n---------------\n";
  // //   std::cout << substitute_variables_1_based(expr, {5,4,3,2,1});
  // //   std::cout << "\n===============\n";
  // // }
  // const auto ranks = space_mapping_ranks(s2, identity_function, [](const auto& expr) {
  //   return std::tuple {
  //     + expr
  //     + substitute_variables_1_based(expr, {5,4,3,2,1})
  //     ,
  //     // + expr
  //     // - substitute_variables_1_based(expr, {2,3,4,5,1})
  //     // ,
  //     + substitute_variables_1_based(expr, {1,2,3,4,5})
  //     - substitute_variables_1_based(expr, {1,2,3,4,6})
  //     + substitute_variables_1_based(expr, {1,2,3,5,6})
  //     - substitute_variables_1_based(expr, {1,2,4,5,6})
  //     + substitute_variables_1_based(expr, {1,3,4,5,6})
  //     - substitute_variables_1_based(expr, {2,3,4,5,6})
  //   };
  // });
  // std::cout << to_string(ranks) << "\n";

  // const auto expr =
  //   + ncoproduct(Log(1,2,3,4), Log(2,3,4,5))
  //   + ncoproduct(Log(2,3,4,5), Log(3,4,5,1))
  //   + ncoproduct(Log(3,4,5,1), Log(4,5,1,2))
  //   + ncoproduct(Log(4,5,1,2), Log(5,1,2,3))
  //   + ncoproduct(Log(5,1,2,3), Log(1,2,3,4))
  // ;
  // std::cout << expr;
  // // std::cout << expr + substitute_variables_1_based(expr, {5,4,3,2,1});
  // std::cout << expr - substitute_variables_1_based(expr, {2,3,4,5,1});


  // const auto snew = std::vector{
  //   ncoproduct(Log(1,2,3,4), Log(2,3,4,5)),
  //   ncoproduct(Log(1,2,3,4), Log(3,4,5,1)),
  //   ncoproduct(Log(2,3,4,5), Log(3,4,5,1)),
  //   ncoproduct(Log(2,3,4,5), Log(4,5,1,2)),
  //   ncoproduct(Log(3,4,5,1), Log(4,5,1,2)),
  //   ncoproduct(Log(3,4,5,1), Log(5,1,2,3)),
  //   ncoproduct(Log(4,5,1,2), Log(5,1,2,3)),
  //   ncoproduct(Log(4,5,1,2), Log(1,2,3,4)),
  //   ncoproduct(Log(5,1,2,3), Log(1,2,3,4)),
  //   ncoproduct(Log(5,1,2,3), Log(2,3,4,5)),
  // };
  // std::cout << space_rank(snew, identity_function) << "\n";

  // const auto snew = std::vector{
  //   ncoproduct(Log(1,2,3,4), Log(2,3,4,5)),
  //   ncoproduct(Log(1,2,3,4), Log(3,4,5,1)),
  //   ncoproduct(Log(2,3,4,5), Log(3,4,5,1)),
  //   ncoproduct(Log(2,3,4,5), Log(4,5,1,2)),
  //   ncoproduct(Log(3,4,5,1), Log(4,5,1,2)),
  //   ncoproduct(Log(3,4,5,1), Log(5,1,2,3)),
  //   ncoproduct(Log(4,5,1,2), Log(5,1,2,3)),
  //   ncoproduct(Log(4,5,1,2), Log(1,2,3,4)),
  //   ncoproduct(Log(5,1,2,3), Log(1,2,3,4)),
  //   ncoproduct(Log(5,1,2,3), Log(2,3,4,5)),
  // };
  // for (const auto& signs : cartesian_power({-1, 1, 0}, snew.size())) {
  //   const auto expr = sum(mapped(zip(signs, snew), [](const auto& pair) {
  //     const auto [sign, expr] = pair;
  //     return sign * expr;
  //   }));
  //   const auto sum_expr =
  //     + substitute_variables_1_based(expr, {1,2,3,4,5})
  //     - substitute_variables_1_based(expr, {1,2,3,4,6})
  //     + substitute_variables_1_based(expr, {1,2,3,5,6})
  //     - substitute_variables_1_based(expr, {1,2,4,5,6})
  //     + substitute_variables_1_based(expr, {1,3,4,5,6})
  //     - substitute_variables_1_based(expr, {2,3,4,5,6})
  //   ;
  //   if (sum_expr.is_zero()) {
  //     std::cout << expr;
  //   }
  // }
  // // std::cout << space_rank(snew, identity_function) << "\n";


  // const auto compute = [](int weight, int dimension, int num_points) {
  //   return bigrassmannian_complex_cohomology(dimension, num_points, [&](const int dim, const auto& points) {
  //     return CGrL_test_space(weight, dim, points);
  //   });
  // };

  // for (EACH : range(3)) {
  //   Profiler profiler;
  //   CHECK_EQ(compute(3, 4, 8), 1);
  //   profiler.finish("A");
  //   CHECK_EQ(compute(4, 4, 7), 0);
  //   profiler.finish("B");
  //   CHECK_EQ(compute(4, 4, 8), 2);
  //   profiler.finish("C");
  // }


  // int v = 1;
  // Matrix m;
  // m.insert(0, 0) = v;
  // m.insert(0, 1) = v;
  // m.insert(0, 2) = v;
  // m.insert(1, 2) = v;
  // std::cout << dump_to_string(find_kernel_vector(m)) << "\n";
  // // std::cout << dump_to_string(linear_solve(m, {0, 1})) << "\n";
  // // int rank = matrix_rank_raw_linbox(m);
  // // std::cout << "rank = " << rank << "\n";




  // const int weight = 4;
  // const int dimension = 3;
  // for (const int num_points : range_incl(6, 6)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   const auto space = co_space(weight, 2, [&](const int weight) {
  //     switch (weight) {
  //       case 1: return normalize_space_remove_consecutive(GrFx(dimension, points), dimension, num_points);
  //       case 2: return normalize_space_remove_consecutive(GrL2(dimension, points), dimension, num_points);
  //       case 3: return normalize_space_remove_consecutive(CGrL3_Dim3_test_space(points), dimension, num_points);
  //       default: FATAL("Unsupported weight");
  //     }
  //   });
  //   profiler.finish("space");
  //   const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   });
  //   profiler.finish("ranks");

  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << "\n";

  //   const auto mapping_matrix = space_matrix(space, [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   });
  //   std::cout << dump_to_string(mapping_matrix) << "\n";
  //   const auto kernel_vector = find_kernel_vector(mapping_matrix);
  //   profiler.finish("kernel");
  //   const auto equation = sum(mapped(zip(kernel_vector, space), [](const auto& pair) {
  //     const auto [coeff, expr] = pair;
  //     return coeff * expr;
  //   }));
  //   std::cout << equation;
  //   CHECK(ncomultiply(equation).is_zero());
  //   CHECK(keep_non_weakly_separated(equation).is_zero());
  //   std::cout << to_string(space_venn_ranks(
  //     mapped(
  //       normalize_space_remove_consecutive(CGrL3_Dim3_test_space(points), dimension, num_points),
  //       DISAMBIGUATE(ncomultiply)
  //     ),
  //     std::vector{equation},
  //     identity_function
  //   )) << "\n";
  //   std::cout << to_string(space_venn_ranks(
  //     mapped(
  //       normalize_space_remove_consecutive(CGrL3_Dim3_Depth1_test_space(points), dimension, num_points),
  //       DISAMBIGUATE(ncomultiply)
  //     ),
  //     std::vector{equation},
  //     identity_function
  //   )) << "\n";
  //   // std::cout << dump_to_string(kernel_vector) << "\n";
  // }



  // const int weight = 4;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 10)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   const auto space = co_space(weight, 2, [&](const int weight) {
  //     switch (weight) {
  //       case 1: return normalize_space_remove_consecutive(GrFx(dimension, points), dimension, num_points);
  //       case 2: return normalize_space_remove_consecutive(GrL2(dimension, points), dimension, num_points);
  //       case 3: return normalize_space_remove_consecutive(CGrL3_Dim3_test_space(points), dimension, num_points);
  //       default: FATAL("Unsupported weight");
  //     }
  //   });
  //   profiler.finish("space");
  //   const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   });
  //   profiler.finish("ranks");
  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << "\n";
  // }


  // const int weight = 5;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   const auto space = co_space(weight, 2, [&](const int w) {
  //     const auto sp = w == 1
  //       ? GrFx(dimension, points)
  //       : CGrL_test_space(w, dimension, points);
  //     const auto spl = mapped(sp, DISAMBIGUATE(to_lyndon_basis));
  //     // return normalize_space_remove_consecutive(spl, dimension, num_points);
  //     return space_basis(normalize_space_remove_consecutive(spl, dimension, num_points), identity_function);
  //   });
  //   profiler.finish("space");
  //
  //   // const auto base_name = absl::StrCat("cgrl", weight, "_d", dimension, "_p", num_points);
  //   // const auto dir_path = "/mnt/c/Andrei/polylog/matrices/";
  //   // const auto full_matrix = space_matrix(space, identity_function);
  //   // const auto mapping_matrix = space_matrix(space, [](const auto& expr) {
  //   //   return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   // });
  //   // profiler.finish("matrix");
  //   // // save_triplets(absl::StrCat(dir_path, base_name, "_all.txt"), full_matrix);
  //   // save_triplets(absl::StrCat(dir_path, base_name, ".txt"), mapping_matrix);
  //   // profiler.finish("dump");
  //   // std::cout << dump_to_string(full_matrix) << "\n";
  //   // std::cout << dump_to_string(mapping_matrix) << "\n";
  //   // std::cout << "Done: d=" << dimension << ", w=" << weight << ", p=" << num_points << "\n";
  //
  //   const int rank = space_rank(space, [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   });
  //   profiler.finish("rank");
  //   const SpaceMappingRanks ranks(space.size(), rank);
  //   std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   std::cout << to_string(ranks) << "\n";
  // }


  // const int weight = 5;
  // const int dimension = 3;
  // for (const int num_points : range_incl(5, 10)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   Profiler profiler;
  //   const auto full_space = co_space(weight, 2, [&](const int w) {
  //     const auto sp = w == 1
  //       ? GrFx(dimension, points)
  //       : CGrL_test_space(w, dimension, points);
  //     const auto spl = mapped(sp, DISAMBIGUATE(to_lyndon_basis));
  //     return normalize_space_remove_consecutive(spl, dimension, num_points);
  //   });
  //   const auto space = filtered(full_space, [](const auto& expr) {
  //     return is_totally_weakly_separated(expr);
  //   });
  //   profiler.finish("space");
  //
  //   const auto base_name = absl::StrCat("alt_cgrl", weight, "_d", dimension, "_p", num_points);
  //   const auto dir_path = "/mnt/c/Andrei/polylog/matrices/";
  //   const auto full_matrix = space_matrix(space, identity_function);
  //   const auto mapping_matrix = space_matrix(space, DISAMBIGUATE(ncomultiply));
  //   profiler.finish("matrix");
  //   save_triplets(absl::StrCat(dir_path, base_name, ".txt"), mapping_matrix);
  //   profiler.finish("dump");
  //   std::cout << dump_to_string(full_matrix) << "\n";
  //   std::cout << dump_to_string(mapping_matrix) << "\n";
  //   std::cout << "Done: d=" << dimension << ", w=" << weight << ", p=" << num_points << "\n";
  //
  //   // const auto ranks = space_mapping_ranks(space, identity_function, DISAMBIGUATE(ncomultiply));
  //   // profiler.finish("ranks");
  //   // std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
  //   // std::cout << to_string(ranks) << "\n";
  // }



  // const int weight = 5;
  // const int dimension = 3;
  // const int num_points = 6;
  // const auto points = to_vector(range_incl(1, num_points));
  // const auto space = co_space(weight, 2, [&](const int w) {
  //   const auto sp = w == 1
  //     ? GrFx(dimension, points)
  //     : CGrL_test_space(w, dimension, points);
  //   const auto spl = mapped(sp, DISAMBIGUATE(to_lyndon_basis));
  //   return space_basis(normalize_space_remove_consecutive(spl, dimension, num_points), identity_function);
  // });
  // const auto dir_path = "/mnt/c/Andrei/polylog/cgrl5_solutions/";
  // const auto file_path = absl::StrCat(dir_path, "dp3_p", num_points, "_ns.txt");
  // const auto& solutions = load_bracketed_vectors(file_path);
  // // std::cout << solutions.size() << "\n";
  // for (const auto& solution : solutions) {
  //   const auto expr = sum(mapped(zip(solution, space), [](const auto& pair) {
  //     const auto [coeff, expr] = pair;
  //     return coeff * expr;
  //   }));
  //   std::cout << expr.annotations() << "\n";
  //   CHECK(ncomultiply(expr).is_zero());
  //   CHECK(keep_non_weakly_separated(expr).is_zero());
  //   // std::cout << expr;
  //   // std::cout << ncomultiply(expr);
  //   // std::cout << keep_non_weakly_separated(expr);
  // }


  // const int weight = 5;
  // const int dimension = 3;
  // const int num_points = 8;
  // const auto points = to_vector(range_incl(1, num_points));
  // Profiler profiler;
  // const auto file_path = absl::StrCat("../../cgrl5_solutions/alt_dp3_p", num_points, "_ns.txt");
  // const auto& solutions = load_bracketed_vectors(file_path);
  // profiler.finish("read");
  // const auto full_space = co_space(weight, 2, [&](const int w) {
  //   Gr_Space sp;
  //   switch (w) {
  //     case 1: sp = GrFx(dimension, points); break;
  //     case 2: sp = GrL2_unreduced(dimension, points); break;
  //     case 3: sp = CGrL3_Dim3_test_space_unreduced(points); break;
  //     default: sp = CGrL_test_space(w, dimension, points); break;
  //   }
  //   const auto spl = mapped(sp, DISAMBIGUATE(to_lyndon_basis));
  //   return normalize_space_remove_consecutive(spl, dimension, num_points);
  // });
  // const auto space = filtered(full_space, [](const auto& expr) {
  //   return is_totally_weakly_separated(expr);
  // });
  // profiler.finish("space");

  // const auto cglr5_space = normalize_space_remove_consecutive(
  //   CGrL_test_space(weight, dimension, points),
  //   dimension, num_points
  // );
  // profiler.finish("cgrl5 space");
  // const auto cglr5_cospace = mapped(cglr5_space, DISAMBIGUATE(ncomultiply));
  // const auto cglr5_cospace_1_4 = filtered(
  //   mapped(cglr5_cospace, [](const auto& expr) { return keep_coexpr_form(expr, {1,4}); }),
  //   [](const auto& expr) { return !expr.is_zero(); }
  // );
  // profiler.finish("cgrl5 cospace");
  // for (const int i_solution : range(10)) {
  //   std::cout << "---------------\n";
  //   // const auto& solution = solutions.back();
  //   const auto& solution = solutions[solutions.size() - i_solution - 1];
  //   GammaNCoExpr expr;
  //   for (int i : range(solution.size())) {
  //     if (solution[i] != 0) {
  //       expr += solution[i] * space[i];
  //     }
  //   }
  //   expr = expr.without_annotations();
  //   const auto expr_1_4 = keep_coexpr_form(expr, {1,4});
  //   profiler.finish("expr");
  //   const auto ranks = space_venn_ranks(cglr5_cospace_1_4, {expr_1_4}, std::identity{});
  //   profiler.finish("ranks");
  //   std::cout << to_string(ranks) << "\n";
  //   const auto expr_1_4_sum = expr_1_4 + substitute_variables_1_based(expr_1_4, rotated_vector(points, 1));
  //   const auto expr_1_4_diff = expr_1_4 - substitute_variables_1_based(expr_1_4, rotated_vector(points, 1));
  //   const auto ranks_sum = space_venn_ranks(cglr5_cospace_1_4, {expr_1_4_sum}, std::identity{});
  //   const auto ranks_diff = space_venn_ranks(cglr5_cospace_1_4, {expr_1_4_diff}, std::identity{});
  //   profiler.finish("ranks");
  //   std::cout << to_string(ranks_sum) << "\n";
  //   std::cout << to_string(ranks_diff) << "\n";
  // }

  // const auto cglr5_space = normalize_space_remove_consecutive(
  //   CGrL_test_space(weight, dimension, {1,2,3,4,5,6,7,8,9}),
  //   dimension, 9
  // );
  // profiler.finish("cgrl5 space");
  // const auto cglr5_cospace = mapped(cglr5_space, DISAMBIGUATE(ncomultiply));
  // const auto cglr5_cospace_1_4 = filtered(
  //   mapped(cglr5_cospace, [](const auto& expr) { return keep_coexpr_form(expr, {1,4}); }),
  //   [](const auto& expr) { return !expr.is_zero(); }
  // );
  // profiler.finish("cgrl5 cospace");
  // for (const int i_solution : range(10)) {
  //   std::cout << "---------------\n";
  //   // const auto& solution = solutions.back();
  //   const auto& solution = solutions[solutions.size() - i_solution - 1];
  //   GammaNCoExpr expr;
  //   for (int i : range(solution.size())) {
  //     if (solution[i] != 0) {
  //       expr += solution[i] * space[i];
  //     }
  //   }
  //   expr = expr.without_annotations();
  //   const auto expr_1_4 = keep_coexpr_form(expr, {1,4});
  //   profiler.finish("expr");
  //   // const auto expr_1_4_alt_sum = sum_looped_vec([&](const auto& points) {
  //   //   return substitute_variables_1_based(expr_1_4, points);
  //   // }, 9, {1,2,3,4,5,6,7,8}, SumSign::alternating);
  //   const auto expr_1_4_alt_sum =
  //     + substitute_variables_1_based(expr_1_4, {1,2,3,4,5,6,7,8})
  //     - substitute_variables_1_based(expr_1_4, {1,2,3,4,5,6,7,9})
  //     + substitute_variables_1_based(expr_1_4, {1,2,3,4,5,6,8,9})
  //     - substitute_variables_1_based(expr_1_4, {1,2,3,4,5,7,8,9})
  //     + substitute_variables_1_based(expr_1_4, {1,2,3,4,6,7,8,9})
  //     - substitute_variables_1_based(expr_1_4, {1,2,3,5,6,7,8,9})
  //     + substitute_variables_1_based(expr_1_4, {1,2,4,5,6,7,8,9})
  //     - substitute_variables_1_based(expr_1_4, {1,3,4,5,6,7,8,9})
  //     + substitute_variables_1_based(expr_1_4, {2,3,4,5,6,7,8,9})
  //   ;
  //   const auto alt_sum_ranks = space_venn_ranks(cglr5_cospace_1_4, {expr_1_4_alt_sum}, std::identity{});
  //   profiler.finish("ranks");
  //   std::cout << to_string(alt_sum_ranks) << "\n";
  // }

  // const auto proto_expr = expr_1_4.mapped_expanding([](const auto& term) {
  //   return GammaExpr::single(flatten(term));
  // });
  // profiler.finish("protoexpr");
  // const auto co_proto_expr = ncomultiply(proto_expr);
  // profiler.finish("comultiply");
  // std::cout << expr;
  // std::cout << proto_expr;
  // std::cout << co_proto_expr;
  // // std::cout << co_proto_expr - expr;

  // std::cout << "Loaded " << solutions.size() << " solutions.\n";
  // std::regex re_6_points(R"(\([0-9]+,[0-9]+,[0-9]+,[0-9]+,[0-9]+,[0-9]+\))");
  // for (const auto& solution : solutions) {
  //   GammaNCoExpr expr;
  //   for (int i : range(solution.size())) {
  //     if (solution[i] != 0) {
  //       expr += solution[i] * space[i];
  //     }
  //   }
  //   // std::cout << expr;
  //   CHECK(ncomultiply(expr).is_zero());
  //   CHECK(keep_non_weakly_separated(expr).is_zero());
  //   // CHECK_EQ(solution.size(), space.size());
  //   // BasicLinearAnnotation annotations;
  //   // bool contain_6_points = false;
  //   // for (int i : range(solution.size())) {
  //   //   if (solution[i] != 0) {
  //   //     if (space[i].annotations().expression.contains([&](const auto& term) {
  //   //       return std::regex_search(term, re_6_points);
  //   //     })) {
  //   //       contain_6_points = true;
  //   //     }
  //   //     annotations += solution[i] * space[i].annotations().expression;
  //   //   }
  //   // }
  //   // if (contain_6_points && annotations.num_terms() >= 2) {
  //   //   std::cout << LinearAnnotation{annotations, {}} << "\n";
  //   // }
  // }
  // profiler.finish("check");


  const int weight = 5;
  const int dimension = 3;
  for (const int num_points : range_incl(5, 10)) {
    const auto points = to_vector(range_incl(1, num_points));
    Profiler profiler;
    const auto full_space = co_space(weight, 2, [&](const int w) {
      Gr_Space sp;
      switch (w) {
        case 1: sp = GrFx(dimension, points); break;
        case 2: sp = GrL2_unreduced(dimension, points); break;
        case 3: sp = CGrL3_Dim3_test_space_unreduced(points); break;
        default: sp = CGrL_test_space(w, dimension, points); break;
      }
      const auto spl = mapped(sp, DISAMBIGUATE(to_lyndon_basis));
      return normalize_space_remove_consecutive(spl, dimension, num_points);
    });
    const auto space = filtered(full_space, [](const auto& expr) {
      return is_totally_weakly_separated(expr);
    });
    profiler.finish("space");
    const auto ranks = space_mapping_ranks(space, identity_function, DISAMBIGUATE(ncomultiply));
    profiler.finish("ranks");
    std::cout << "d=" << dimension << ", w=" << weight << ", p=" << num_points << ": ";
    std::cout << to_string(ranks) << "\n";
  }
}
