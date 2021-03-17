#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/packed.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"
#include "lib/zip.h"


// TODO: Consider adding base_0_to_1 / base_1_to_0 utils.
// TODO: Move this to delta.h / projection.h
int count_var(const std::vector<Delta>& term, int var) {
  return absl::c_count_if(term, [&](const Delta& d) { return d.a() == var || d.b() == var; });
};
int count_var(const std::vector<int>& term, int var) {
  return absl::c_count(term, var);
};

void renumerate(std::vector<int>& vec) {
  absl::flat_hash_map<int, int> new_index;
  int next_index = 0;
  for (int& x : vec) {
    if (!new_index.contains(x)) {
      new_index[x] = next_index;
      ++next_index;
    }
    x = new_index.at(x);
  }
}

std::vector<int> substitute(const std::vector<int>& points, const std::vector<int>& new_indices) {
  return mapped(points, [&](const int p) { return new_indices.at(p - 1) + 1; });
}

ProjectionExpr inv_pr(const DeltaExpr& expr) {
  return involute_projected(expr, {1,2,3,4,5,6}, 3);
}


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(10)
    .set_annotation_sorting(AnnotationSorting::length)
  );



#if 0
  // auto qli6 =
  //   + Lira(1,1)(CR(1,4,5,6), CR(1,2,3,4))
  //   - Lira(1,1)(CR(1,2,5,6), CR(3,4,5,2))
  //   + Lira(1,1)(CR(1,2,3,6), CR(3,4,5,6))
  // ;



  // static const auto preshow = [](const DeltaExpr& expr) {
  //   return terms_with_min_distinct_variables(to_lyndon_basis(project_on_x5(expr)), 4);
  // };

  // static const auto lira = [](int x1, int x2, int x3, int x4, int x5, int x6) {
  //   return Lira4(1,1,1)(CR(x1,x2,x3,x4), CR(x1,x4,x3,x5), CR(x1,x5,x3,x6));
  // };

  // auto t_expr =
  //   + lira(1,2,3,4,5,6)
  // ;
  // auto expr = theta_expr_to_delta_expr(t_expr);
  // auto rhs =
  //   + QLi6(1,2,3,4,3,5,3,6)
  // ;
  // auto diff = expr + rhs;
  // auto pr = project_on_x5(diff);
  // pr = to_lyndon_basis(pr);
  // // pr = pr.filtered([](const auto& term) {
  // //   return count_var(term, 6) == 0;
  // // });
  // print_sorted_by_num_distinct_variables(std::cout, pr);


  // TODO: Fix 1 variable in place !!!
  // Profiler profiler;
  // static const int n = 6;
  // std::vector<int> seq_tmpl(n);
  // absl::c_iota(seq_tmpl, 0);
  // do {
  //   auto seq = seq_tmpl;
  //   for (int i : range(n)) {
  //     for (int j : range(i+1, n)) {
  //       seq[i] = seq[j];
  //       renumerate(seq);
  //       seq = mapped(seq, [](int x) { return x + 1; });
  //       auto expr = to_lyndon_basis(project_on_x5(QLiVec(6, seq)));
  //       expr = terms_with_min_distinct_variables(expr, 4);
  //       if (!expr.is_zero()) {
  //         std::cout << expr;
  //       }
  //     }
  //   }
  // } while (absl::c_next_permutation(seq_tmpl));
  // profiler.finish("Sequences");


  Profiler profiler;
  std::vector<int> arg_order(5);
  int checksum = 0;
  absl::c_iota(arg_order, 0);
  do {
    auto expr = to_lyndon_basis(project_on_x5(QLiVec(6, substitute({1,2,1,3,4,5}, arg_order))));
    expr = terms_with_min_distinct_variables(expr, 4);
    if (!expr.is_zero()) {
      // std::cout << expr;
      checksum += expr.l1_norm();
    }
  } while (absl::c_next_permutation(arg_order));
  profiler.finish("Sequences");
  std::cout << "Checksum = " << checksum << "\n";

  // auto t_expr =
  //   + lira(1,2,3,2,4,5)
  // ;
  // // std::cout << t_expr;
  // // std::cout << keep_monsters(t_expr);
  // auto expr = theta_expr_to_delta_expr(t_expr);
  // // auto rhs =
  // //   QLi6()
  // // ;
  // // auto diff = expr - rhs;
  // // auto pr = project_on_x5(diff);
  // auto pr = project_on_x5(expr);
  // pr = to_lyndon_basis(pr);
  // print_sorted_by_num_distinct_variables(std::cout, pr);

  // std::cout << "===\n";

  // std::cout << preshow(QLi6(1,2,3,4,5,4));
  // std::cout << preshow(QLi6(1,2,3,4,5,3));
  // std::cout << preshow(QLi6(1,2,3,4,5,2));
  // std::cout << preshow(QLi6(1,2,1,3,5,4));
  // std::cout << preshow(QLi6(1,2,3,2,5,4));
  // std::cout << preshow(QLi6(1,2,3,1,5,4));



  // const std::string expr_str = get_file_content(R"(C:\Users\amatv\Downloads\Sharmanka)");
  // auto expr_parsed = parse_delta_expression(expr_str);
  // std::cout << "Mathematica " << to_lyndon_basis(expr_parsed);
  // auto expr_computed = theta_expr_to_delta_expr(
  //   Lira3(1,1)(CR(1,2,3,4), CR(1,5,3,2))
  // );
  // std::cout << "PolyKit " << to_lyndon_basis(expr_computed);
  // std::cout << "Diff " << to_lyndon_basis(expr_parsed - expr_computed);
  // return 0;
#endif


#if 1
  static auto countains_var = [](const auto& expr, int var) {
    bool contains = false;
    expr.foreach([&](const auto& term, int) {
      if (count_var(term, var) > 0) {
        contains = true;
      }
    });
    return contains;
  };

  // static auto preshow = [](const DeltaExpr& expr) {
  //   return terms_with_min_distinct_variables(to_lyndon_basis(project_on_x3(expr)), 2);
  // };
  // static auto preshow2 = [](const DeltaExpr& expr) {
  //   return preshow(expr).filtered([](const auto& term) {
  //     return count_var(term, 1) == 0;
  //   });
  // };

  // for (const auto seq : all_sequences(6, 4)) {
  //   const bool include = absl::c_min_element(seq) == seq.begin() && seq[1] < seq[3];
  //   if (!include) {
  //     continue;
  //   }
  //   const auto args = mapped(seq, [](int x) { return x + 1; });
  //   // const auto expr = preshow(involute(QLiVec(4, args), {1,2,3,4,5,6}));
  //   // if (!expr.is_zero() && !countains_var(expr, 1)) {
  //   //   std::cout << fmt::brackets(str_join(args, ",")) << " " << expr;
  //   // }
  //   const auto expr = preshow(involute(QLiVec(4, args), {1,2,3,4,5,6}));
  //   if (!expr.is_zero()) {
  //     std::cout << ">>> " << expr;
  //   }
  // }

  static auto preshow = [](const DeltaExpr& expr) {
    return terms_with_min_distinct_variables(to_lyndon_basis(inv_pr(expr)), 4);
  };


  // auto expr = inv_pr(
  //   +2*QLi4(1,6,1,5,3,2)
  //   -2*QLi4(6,5,1,3,4,5)

  //   -2*QLi4(1,3,2,6)
  //   -  QLi4(2,3,5,6)
  //   +  QLi4(1,3,4,6)

  //   -2*QLi4(1,5,4,6)
  //   +2*QLi4(3,4,5,6)
  //   +2*QLi4(2,4,3,6)
  //   +2*QLi4(2,3,5,4)
  //   -  QLi4(1,2,4,5)
  // );


  auto expr = inv_pr(
    +2*QLi4(1,6,1,5,3,2)
    -2*QLi4(6,5,1,3,4,5)

    -2*QLi4(1,3,2,6)
    +2*QLi4(3,4,5,6)
    +2*QLi4(2,4,3,6)
  );

  expr = to_lyndon_basis(expr);
  // std::cout << expr;
  // std::cout << terms_with_num_distinct_variables(expr, 4);
  print_sorted_by_num_distinct_variables(std::cout, expr);

  // std::cout << preshow(QLi4(1,3,2,6));
  // std::cout << preshow(QLi4(2,3,5,6));
  // std::cout << preshow(QLi4(1,3,4,6));
  // std::cout << preshow(QLi4(1,5,4,6));
  // std::cout << preshow(QLi4(3,4,5,6));
  // std::cout << preshow(QLi4(2,4,3,6));
  // std::cout << preshow(QLi4(2,3,5,4));
  // std::cout << preshow(QLi4(1,2,4,5));
#endif


#if 0
  auto expr = involute(
    +2*QLi4(1,6,1,5,3,2)
    -2*QLi4(6,5,1,3,4,5)

    -2*QLi4(1,3,2,6)
    -  QLi4(2,3,5,6)
    +  QLi4(1,3,4,6)

    -2*QLi4(1,5,4,6)
    +2*QLi4(3,4,5,6)
    +2*QLi4(2,4,3,6)
    +2*QLi4(2,3,5,4)
    -  QLi4(1,2,4,5)

    // - QLi4(2,3,6,5)
    // + QLi4(6,1,2,3)
    ,
    {1,2,3,4,5,6}
  );
  expr = to_lyndon_basis(expr);
  // std::cout << comultiply(expr, {2,2});
  // print_sorted_by_num_distinct_variables(std::cout, to_lyndon_basis(expr));
  // std::cout << expr;
  // std::cout << "TwoSixesPlus " << expr.filtered([](const auto& term) {
  //   return
  //     count_var(term, 6) >= 2 &&
  //     count_var(term, 1) >= 1 && count_var(term, 2) >= 1 && count_var(term, 3) >= 1 &&
  //     count_var(term, 4) == 0 && count_var(term, 5) == 0
  //   ;
  // });
  // std::cout << "Threes " << expr.filtered([](const auto& term) {
  //   return
  //     count_var(term, 3) >= 4
  //   ;
  // });

  // std::cout << project_on_x3(expr);
  // std::cout << preshow(expr);
  std::cout << to_lyndon_basis(expr);

  // std::cout << "TwoSixes " << expr.filtered([](const auto& term) {
  //   return absl::c_count_if(term, [](const Delta& d) { return d.a() == 6 || d.b() == 6; }) >= 2;
  // });
  // std::cout << "ThreeSixes " << expr.filtered([](const auto& term) {
  //   return absl::c_count_if(term, [](const Delta& d) { return d.a() == 6 || d.b() == 6; }) >= 3;
  // });
  // std::cout << "FourSixes " <<expr.filtered([](const auto& term) {
  //   return absl::c_count_if(term, [](const Delta& d) { return d.a() == 6 || d.b() == 6; }) >= 4;
  // });

  // std::cout << "A " << preshow(involute(QLi4(2,3,5,6), {1,2,3,4,5,6}));
  // std::cout << "B " << preshow(involute(QLi4(2,3,6,5), {1,2,3,4,5,6}));
  // std::cout << "C " << preshow(involute(QLi4(2,5,3,6), {1,2,3,4,5,6}));
  // std::cout << "D " << preshow(involute(QLi4(2,5,6,3), {1,2,3,4,5,6}));
  // std::cout << "E " << preshow(involute(QLi4(2,6,3,5), {1,2,3,4,5,6}));
  // std::cout << "F " << preshow(involute(QLi4(2,6,5,3), {1,2,3,4,5,6}));
#endif
}
