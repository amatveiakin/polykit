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
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/packed.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"


DeltaExpr LidoSymmViaCorr_New(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = div_int(num_vars, 2);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto special_args = filtered(args, [](int x) { return x == 1 || x == 4; });
    if (special_args.size() == num_distinct_elements(special_args)) {
      const int missing_special = total_odd_vars - special_args.size();
      const int coeff = neg_one_pow(missing_special + num_args);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

DeltaExpr LidoSymm1Fixed(X x1, X x2, X x3, X x4) {
  return Lido1(x1,x2,x3,x4) - Lido1(x1,x2) - Lido1(x3,x4);
}

DeltaExpr LidoSymm1Fixed(const std::vector<X>& points) {
  return LidoSymm1Fixed(points[0], points[1], points[2], points[3]);
}

WordExpr SuperLidoOriginalAnnotations(int weight, const std::vector<X>& points) {
  WordExpr expr;
  expr += 3 * LidoSymmVecPr(weight, points, project_on_x1);
  for (int num_args = 4; num_args <= points.size() - 1; num_args += 2) {
    for (const auto seq : increasing_sequences(points.size(), num_args)) {
      const auto args = choose_indices(points, seq);
      const int sign_proto =
        + absl::c_accumulate(mapped(seq, [](int x) { return x + 1; }), 0)
        + num_args / 2
      ;
      // const int sign_proto = num_args / 2;
      // const int sign_proto = 0;
      int sign = neg_one_pow(sign_proto);
      expr += sign * LidoSymmVecPr(weight, args, project_on_x1);
    }
  }
  return expr;
}

WordExpr SuperLido(int weight, const std::vector<X>& points) {
  return SuperLidoOriginalAnnotations(weight, points).without_annotations().annotate(
    fmt::function(
      fmt::sub_num("SuperLido", {weight}),
      mapped(points, [](X x) { return to_string(x); })
    )
  );
}


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    .set_formatter(Formatter::unicode)
    .set_rich_text_format(RichTextFormat::console)
    .set_annotation_sorting(AnnotationSorting::length)
    // .set_expression_line_limit(10000)
  );




  // const int weight = 4;
  // const int total_points = 9;
  // DeltaExpr expr;
  // for (int num_args = 4; num_args <= total_points; num_args += 2) {
  //   for (const auto seq : increasing_sequences(total_points, num_args)) {
  //     const auto args = mapped(seq, [](int x) { return x + 1; });
  //     const int sign_proto = absl::c_accumulate(args, 0) + num_args / 2;
  //     const int sign = neg_one_pow(sign_proto);
  //     expr += sign * LidoSymmVec(weight, args);
  //   }
  // }
  // std::cout << to_lyndon_basis(expr);

  static const int weight = 6;
  std::cout << SuperLidoOriginalAnnotations(weight, {1,2,3,4,5,6,7,8});
  std::cout << "\n\n";

  auto expr = sum_looped_vec([](const std::vector<X>& args) {
    return SuperLido(weight, args);
  }, 9, {1,2,3,4,5,6,7,8}, SumSign::plus);
  // WordExpr expr;
  // for (const auto seq : increasing_sequences(9, 8)) {
  //   auto args = mapped(seq, [](int x) { return X(x+1); });
  //   expr += SuperLido(weight, args);
  // }
  auto lyndon = to_lyndon_basis(expr);
  // std::cout << lyndon;
  print_sorted_by_num_distinct_elements(std::cout, lyndon);



  // std::cout << to_lyndon_basis(
  //   + LidoSymm3(1,2,3,4)
  //   - LidoSymmViaCorr_New(3, 4)
  // );

  // std::cout << to_lyndon_basis(LidoSymm1(1,2,3,4));
  // std::cout << to_lyndon_basis(LidoSymm2(1,2,3,4) + LidoSymm2(2,3,4,1));
  // std::cout << to_lyndon_basis(LidoSymm1Fixed(1,2,3,4) - LidoSymm1Fixed(2,3,4,1));



  // static const int wr = 3;
  // auto lhs = comultiply(LidoSymmVec(wr+1, {1,2,3,4,5,6}), {1,wr});
  // auto rhs =
  //   + sum_looped_vec([](const std::vector<X>& args) {
  //     return coproduct(
  //       LidoSymmVec(1,  choose_indices_one_based(args, {1,2,3,4})),
  //       LidoSymmVec(wr, choose_indices_one_based(args, {1,4,5,6}))
  //     );
  //   }, 6, {1,2,3,4,5,6}, SumSign::alternating)
  //   + coproduct(cross_ratio(std::vector{1,2,3,4,5,6}), LidoSymmVec(wr, {1,2,3,4,5,6}))
  // ;
  // std::cout << (lhs - rhs);



  // static const int wr = 3;
  // auto lhs = comultiply(LidoSymmVec(wr+1, {1,2,3,4,5,6,7,8}), {1,wr});
  // auto rhs =
  //   + sum_looped_vec([](const std::vector<X>& args) {
  //     return coproduct(
  //       LidoSymmVec(1,  choose_indices_one_based(args, {1,2,3,4})),
  //       LidoSymmVec(wr, choose_indices_one_based(args, {1,4,5,6,7,8}))
  //     );
  //   }, 8, {1,2,3,4,5,6,7,8}, SumSign::alternating)
  //   + coproduct(cross_ratio(std::vector{1,2,3,4,5,6,7,8}), LidoSymmVec(wr, {1,2,3,4,5,6,7,8}))
  // ;
  // std::cout << (lhs - rhs);

  // static const int w = 2;
  // Profiler profiler;
  // auto lhs_raw = LidoSymmVec(2*w, {1,2,3,4,5,6,7,8});
  // profiler.finish("LHS");
  // auto lhs = comultiply(lhs_raw, {w,w});
  // profiler.finish("Comultiply");
  // auto rhs =
  //   + sum_looped_vec([](const std::vector<X>& args) {
  //     return
  //       + coproduct(
  //         LidoSymmVec(w, choose_indices_one_based(args, {1,2,3,4})),
  //         LidoSymmVec(w, choose_indices_one_based(args, {1,4,5,6,7,8}))
  //       )
  //       // - coproduct(
  //       //   LidoSymmVec(w, choose_indices_one_based(args, {1,2,3,8})),
  //       //   LidoSymmVec(w, choose_indices_one_based(args, {4,5,6,7}))
  //       // )
  //     ;
  //   }, 8, {1,2,3,4,5,6,7,8}, SumSign::alternating)
  //   // + (
  //   //   + coproduct(LidoSymm2(1,2,3,8), LidoSymm2(4,5,6,7))
  //   //   - coproduct(LidoSymm2(2,3,4,1), LidoSymm2(5,6,7,8))
  //   //   + coproduct(LidoSymm2(3,4,5,2), LidoSymm2(6,7,8,1))
  //   //   - coproduct(LidoSymm2(4,5,6,3), LidoSymm2(7,8,1,2))
  //   // )
  // ;
  // profiler.finish("RHS");
  // std::cout << "\n";
  // std::cout << (lhs - rhs);
}
