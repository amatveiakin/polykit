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
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/pvector.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"
#include "lib/zip.h"


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_annotation_sorting(AnnotationSorting::length)
  );

  // auto q = QLi1(x1,x2,x3,x4);
  // // auto q = QLi1(x1s,x2s,x3s,x4s);
  // // auto q = D(x1,x2);
  // std::cout << q << "\n";
  // // std::cout << project_on_x1(q) << "\n";
  // // std::cout << to_lyndon_basis(q) << "\n";



  // const int total_points = 6;
  // const int weight = 3;
  // DeltaExpr expr;
  // for (int num_args = 4; num_args <= total_points; num_args += 2) {
  //   for (const auto& seq : increasing_sequences(total_points, num_args)) {
  //     const auto args = mapped(seq, [](int x) { return x + 1; });
  //     const int sign = neg_one_pow(sum(args) + num_args / 2);
  //     expr += sign * QLiSymmVec(weight, args);
  //   }
  // }

  // auto expr =
  //   + QLiSymm3(x1,x2,x3,-x2,-x1,-x3)
  //   + QLiSymm3(x1,x2,x3,-x2)
  //   - QLiSymm3(x1,x2,x3,-x1)
  //   + QLiSymm3(x1,x2,x3,-x3)
  //   + QLiSymm3(x1,x2,-x2,-x1)
  //   - QLiSymm3(x1,x2,-x2,-x3)
  //   + QLiSymm3(x1,x2,-x1,-x3)
  //   - QLiSymm3(x1,x3,-x2,-x1)
  //   + QLiSymm3(x1,x3,-x2,-x3)
  //   - QLiSymm3(x1,x3,-x1,-x3)
  //   + QLiSymm3(x1,-x2,-x1,-x3)
  //   + QLiSymm3(x2,x3,-x2,-x1)
  //   - QLiSymm3(x2,x3,-x2,-x3)
  //   + QLiSymm3(x2,x3,-x1,-x3)
  //   - QLiSymm3(x2,-x2,-x1,-x3)
  //   + QLiSymm3(x3,-x2,-x1,-x3)
  // ;



  // auto expr =
  //   +2*QLi4(x1,-x3,x1,-x2,x3,x2)
  //   -2*QLi4(-x3,-x2,x1,x3,-x1,-x2)
  //   -  QLi4(x1,x2,-x1,-x2)
  //   -2*QLi4(x1,x3,x2,-x3)
  //   +  QLi4(x1,x3,-x1,-x3)
  //   -2*QLi4(x1,-x2,-x1,-x3)
  //   +2*QLi4(x2,x3,-x2,-x1)
  //   -  QLi4(x2,x3,-x2,-x3)
  //   +2*QLi4(x2,-x1,x3,-x3)
  //   +2*QLi4(x3,-x1,-x2,-x3)
  // ;
  // std::cout << to_lyndon_basis(expr);

  // auto lhs = comultiply(
  //   + QLi4(x1,-x3,x1,-x2,x3,x2),
  //   {1,3}
  // );
  // auto rhs = coproduct(
  //   DeltaExpr::single({Delta(x1,x3)}),
  //   + QLi3(-x3,x1,-x2,x3)
  //   - QLi3(x1,-x3,x3,x2)
  // );
  // std::cout << filter_coexpr(lhs - rhs, 0, {Delta(x1,x3)});

  // auto lhs = comultiply(
  //   QLi4(x1,x3,-x1,-x3),
  //   // + QLi4(-x3,-x2,x1,x3,-x1,-x2),
  //   {1,3}
  // );
  // auto rhs = coproduct(
  //   DeltaExpr::single({Delta(x1,x3)}),
  //   // + QLi3(-x3,-x2,x1,x3)
  //   // - QLi3(x3,-x1,-x2,-x3)
  //   + QLi3(x1,x3,-x1,-x3)
  // );
  // std::cout << filter_coexpr(lhs - 2*rhs, 0, {Delta(x1,x3)});



  // auto expr =
  //   -2*QLi3(x1,x3,x2,-x3)
  //   +2*QLi3(x1,x3,-x1,-x3)
  //   -2*QLi3(x1,-x2,-x1,-x3)
  //   +  QLi3(-x3,x1,-x2,x3)
  //   -  QLi3(x1,-x3,x3,x2)
  // ;
  // // std::cout << to_lyndon_basis(project_on_x(expr));  // NOT ready
  // std::cout << to_lyndon_basis(expr);


  // Kummer's equation
  auto lhs =
    + QLiSymm3(x1,x2,x3,-x1,-x2,-x3)
  ;
  auto rhs =
    + QLiSymm3( x1, x2,-x3, x3)
    + QLiSymm3( x2, x3, x1,-x1)
    + QLiSymm3( x3,-x1, x2,-x2)
    + QLiSymm3(-x1,-x2, x3,-x3)
    + QLiSymm3(-x2,-x3,-x1, x1)
    + QLiSymm3(-x3, x1,-x2, x2)
  ;
  std::cout << to_lyndon_basis(lhs - rhs);  // ZERO



  // const int total_points = 6;
  // DeltaExpr lhs;
  // for (int num_args = 4; num_args <= total_points; num_args += 2) {
  //   for (const auto& seq : increasing_sequences(total_points, num_args)) {
  //     const auto args = mapped(seq, [](int x) { return x + 1; });
  //     const int sign = neg_one_pow(sum(args) + num_args / 2);
  //     lhs += sign * QLiSymmVec(3, args);
  //   }
  // }
  // auto lhs =
  //   + QLiSymm3( x1, x2, x3,-x1)
  //   - QLiSymm3( x1, x2, x3,-x2)
  //   + QLiSymm3( x1, x2, x3,-x3)
  //   + QLiSymm3( x1, x2,-x1,-x2)
  //   - QLiSymm3( x1, x2,-x1,-x3)
  //   + QLiSymm3( x1, x2,-x2,-x3)
  //   - QLiSymm3( x1, x3,-x1,-x2)
  //   + QLiSymm3( x1, x3,-x1,-x3)
  //   - QLiSymm3( x1, x3,-x2,-x3)
  //   + QLiSymm3( x1,-x1,-x2,-x3)
  //   + QLiSymm3( x2, x3,-x1,-x2)
  //   - QLiSymm3( x2, x3,-x1,-x3)
  //   + QLiSymm3( x2, x3,-x2,-x3)
  //   - QLiSymm3( x2,-x1,-x2,-x3)
  //   + QLiSymm3( x3,-x1,-x2,-x3)
  // ;
  // auto rhs =
  //   + QLiSymm3( x1, x2,-x3, x3)
  //   + QLiSymm3( x2, x3, x1,-x1)
  //   + QLiSymm3( x3,-x1, x2,-x2)
  //   + QLiSymm3(-x1,-x2, x3,-x3)
  //   + QLiSymm3(-x2,-x3,-x1, x1)
  //   + QLiSymm3(-x3, x1,-x2, x2)
  // ;

  // auto expr =
  //   +2*QLiSymm3( x1, x2, x3,-x1)
  //   +2*QLiSymm3(-x1, x2, x3, x1)
  //   -2*QLiSymm3( x1, x2, x3,-x2)
  //   +2*QLiSymm3( x1, x2, x3,-x3)
  //   +2*QLiSymm3( x1, x2,-x3, x3)
  //   +  QLiSymm3( x1, x2,-x1,-x2)
  //   -2*QLiSymm3( x1, x2,-x1,-x3)
  //   +2*QLiSymm3( x1, x2,-x2,-x3)
  //   +2*QLiSymm3( x3,-x1, x2,-x2)
  //   +  QLiSymm3( x1, x3,-x1,-x3)
  //   -2*QLiSymm3( x1, x3,-x2,-x3)
  //   +  QLiSymm3( x2, x3,-x2,-x3)
  // ;
  // std::cout << to_lyndon_basis(expr);

  // auto expr =
  //   -2*QLiSymm3( x1, x2,-x1, x3)
  //   -2*QLiSymm3( x1, x2, x3,-x2)
  //   -2*QLiSymm3( x3, x1,-x3, x2)
  //   -2*QLiSymm3( x1, x2,-x1,-x3)
  //   -2*QLiSymm3( x2, x3,-x2,-x1)
  //   -2*QLiSymm3( x1, x3,-x2,-x3)
  //   +  QLiSymm3( x1, x2,-x1,-x2)
  //   +  QLiSymm3( x1, x3,-x1,-x3)
  //   +  QLiSymm3( x2, x3,-x2,-x3)
  // ;
  // std::cout << to_lyndon_basis(expr);



  // auto expr =
  //   +  QLiSymm5( x1, x2, x3,-x1,-x2,-x3)
  //   +  QLiSymm5( x1, x2, x4,-x1,-x2,-x4)
  //   +  QLiSymm5( x1, x3, x4,-x1,-x3,-x4)
  //   +  QLiSymm5( x2, x3, x4,-x2,-x3,-x4)
  // ;
  // std::cout << to_lyndon_basis(expr);



  // auto lhs =
  //   + QLiSymm5(x1,x2,x3,x4,-x1,-x2,-x3,-x4)
  //   // + QLiSymm5(x1,x2,x3,x4,x5,x6,x7,x8)
  // ;
  // // auto rhs =
  // //   + QLiSymm5( x1, x2,-x3, x3)
  // //   + QLiSymm5( x2, x3, x1,-x1)
  // //   + QLiSymm5( x3,-x1, x2,-x2)
  // //   + QLiSymm5(-x1,-x2, x3,-x3)
  // //   + QLiSymm5(-x2,-x3,-x1, x1)
  // //   + QLiSymm5(-x3, x1,-x2, x2)
  // // ;
  // auto expr = to_lyndon_basis(lhs);
  // // std::cout << expr.filtered([](const std::vector<Delta>& d) {
  // //   return absl::c_find(d, Delta{x3,Zero}) != d.end();
  // // });
  // print_sorted_by_num_distinct_variables(std::cout, expr);
  // // std::cout << expr;



  // auto expr =
  //   + QLiSymm6(x1,x2,x3,x4,-x1,-x2,-x3,-x4)
  //   + QLiSymm6(-x1,x2,x3,x4,x1,-x2,-x3,-x4)
  // ;
  // std::cout << comultiply(expr, {2,2,2});

  // // auto expr =
  // //   + QLiSymm3( x1, x2, x3,-x1,-x2,-x3)
  // //   - QLiSymm3(-x1, x2, x3, x1,-x2,-x3)
  // // ;
  // // std::cout << to_lyndon_basis(expr);






  // auto lhs =
  //   + QLi4(-x1,x3,x1,x2,-x1,Inf)
  //   - QLi4(x1,x3,-x1,x2,x1,Inf)
  // ;
  // auto rhs =
  //   QLi4(x1s,x2s,x3s,Inf)
  // ;
  // auto prepare = [](auto&& e) {
  //   return terms_with_min_distinct_variables(to_lyndon_basis(project_on_x3(e)), 3);
  // };
  // // std::cout << comultiply(expr, {2,2});
  // std::cout << prepare(lhs);
  // std::cout << prepare(QLi4(x1s,x2s,x3s,Inf));
  // std::cout << prepare(QLi4(x2s,x3s,x1s,Inf));
  // std::cout << prepare(QLi4(x3s,x1s,x2s,Inf));



  // std::cout << to_lyndon_basis(project_on_x1(QLi3(x1,x2,-x1,-x2)));
  // std::cout << to_lyndon_basis(project_on_x1(QLi3(-x3,x1,x3,-x1)));
  // std::cout << to_lyndon_basis(project_on_x1(QLi3(x1,x2,x3,-x1)));
  // std::cout << to_lyndon_basis(project_on_x1(QLi3(x1,x2,x1,x3)));
  // std::cout << to_lyndon_basis(project_on_x1(QLi3(x1,x2,-x1,x3)));
  // std::cout << to_lyndon_basis(project_on_x1(QLi3(x1,x2,-x2,x3)));


  // auto expr =
  //   + 2*QLi2(1,2,3,4)
  //   - 2*QLi2(1,2,3,5)
  //   + 2*QLi2(1,2,4,5)
  //   - 2*QLi2(1,3,4,5)
  //   + 2*QLi2(2,3,4,5)
  // ;
  // expr +=
  //   + 2*shuffle_product_expr(QLi1(1,2,3,5), QLi1(1,3,4,5))
  //   -   shuffle_product_expr(QLi1(1,2,4,5), QLi1(1,2,4,5))
  // ;
  // std::cout << expr;


  // auto a =
  //   + 2*QLi2(1,2,3,4)
  //   - 2*QLi2(1,2,3,5)
  //   + 2*QLi2(1,2,4,5)
  //   - 2*QLi2(1,3,4,5)
  //   + 2*QLi2(2,3,4,5)
  // ;
  // auto b =
  //   + shuffle_product_expr(QLi1(1,2,3,4), Log(1,2,3,4))
  //   - shuffle_product_expr(QLi1(1,2,3,5), Log(1,2,3,5))
  //   + shuffle_product_expr(QLi1(1,2,4,5), Log(1,2,4,5))
  //   - shuffle_product_expr(QLi1(1,3,4,5), Log(1,3,4,5))
  //   + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
  // ;
  // std::cout << a;
  // std::cout << b;
  // std::cout << a - b;



  // auto qli2symm = [](const XArgs& args) {
  //   auto [x1, x2, x3, x4, x5, x6] = to_array<6>(args.as_x());
  //   return
  //     + QLi2(x1,x2,x3,x4,x5,x6)
  //     - QLi2(x1,x2,x3,x4)
  //     - QLi2(x1,x2,x5,x6)
  //     - QLi2(x3,x4,x5,x6)
  //   ;
  // };
  // auto a = qli2symm({1,2,3,4,5,6}) + qli2symm({2,3,4,5,6,1});
  // auto b =
  //   // + shuffle_product_expr(QLi1(1,2,3,4), QLi1(1,4,5,6))
  //   // - shuffle_product_expr(QLi1(2,3,4,5), QLi1(2,5,6,1))
  //   // + shuffle_product_expr(QLi1(3,4,5,6), QLi1(3,6,1,2))
  //   // - shuffle_product_expr(QLi1(4,5,6,1), QLi1(4,1,2,3))
  //   // + shuffle_product_expr(QLi1(5,6,1,2), QLi1(5,2,3,4))
  //   // - shuffle_product_expr(QLi1(6,1,2,3), QLi1(6,3,4,5))
  //   // - shuffle_product_expr(QLi1(1,3,4,6), Log(1,3,4,6))

  //   + shuffle_product_expr(QLi1(1,2,3,4), Log(1,2,3,4))
  //   + shuffle_product_expr(QLi1(1,2,5,6), Log(1,2,5,6))
  //   + shuffle_product_expr(QLi1(3,4,5,6), Log(3,4,5,6))
  //   + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
  //   + shuffle_product_expr(QLi1(2,3,6,1), Log(2,3,6,1))
  //   + shuffle_product_expr(QLi1(4,5,6,1), Log(4,5,6,1))
  // ;
  // std::cout << a;
  // std::cout << b;
  // std::cout << a + b;


  // auto a = QLi2(1,2,3,4,5,6);
  // auto b =
  //   - QLi2(1,2,3,5)
  //   + QLi2(1,2,3,6)
  //   + QLi2(1,2,4,5)
  //   - QLi2(1,2,4,6)
  //   - QLi2(1,3,4,5)
  //   + QLi2(1,3,4,6)
  //   - QLi2(1,3,5,6)
  //   + QLi2(1,4,5,6)
  //   + QLi2(2,3,4,5)
  //   - QLi2(2,3,4,6)
  //   + QLi2(2,3,5,6)
  //   - QLi2(2,4,5,6)
  // ;
  // a *= 2;
  // b *= 2;
  // // std::cout << to_lyndon_basis(a + b);
  // auto c =
  //   - shuffle_product_expr(QLi1(1,2,3,5), Log(1,2,3,5))
  //   + shuffle_product_expr(QLi1(1,2,3,6), Log(1,2,3,6))
  //   + shuffle_product_expr(QLi1(1,2,4,5), Log(1,2,4,5))
  //   - shuffle_product_expr(QLi1(1,2,4,6), Log(1,2,4,6))
  //   - shuffle_product_expr(QLi1(1,3,4,5), Log(1,3,4,5))
  //   + shuffle_product_expr(QLi1(1,3,4,6), Log(1,3,4,6))
  //   - shuffle_product_expr(QLi1(1,3,5,6), Log(1,3,5,6))
  //   + shuffle_product_expr(QLi1(1,4,5,6), Log(1,4,5,6))
  //   + shuffle_product_expr(QLi1(2,3,4,5), Log(2,3,4,5))
  //   - shuffle_product_expr(QLi1(2,3,4,6), Log(2,3,4,6))
  //   + shuffle_product_expr(QLi1(2,3,5,6), Log(2,3,5,6))
  //   - shuffle_product_expr(QLi1(2,4,5,6), Log(2,4,5,6))
  // ;
  // c +=
  //   + shuffle_product_expr(QLi1(1,2,3,4), QLi1(1,4,5,6))
  //   - shuffle_product_expr(QLi1(1,2,5,6), QLi1(3,4,5,2))
  //   + shuffle_product_expr(QLi1(3,4,5,6), QLi1(1,2,3,6))
  // ;
  // std::cout << a + b;
  // std::cout << c;
  // std::cout << a + b - c;
}
