#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_quadrangle.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/theta.h"


DeltaExpr I6(int a, int b) {
  const int weight = 6;
  CHECK_EQ(a + b, weight);
  std::vector<X> points;
  points.push_back(1);
  points.push_back(2);
  for (int i = 0; i < a-1; ++i) {
    points.push_back(1);
  }
  points.push_back(3);
  for (int i = 0; i < b-1; ++i) {
    points.push_back(1);
  }
  points.push_back(4);
  CHECK_EQ(points.size(), weight + 2);
  return I(points);
}


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});


  // std::cout << Lido6(1,2,3,4,5,6) << "\n";
  // std::cout << Lido6(1,2,3,4,5,Inf) << "\n";
  // std::cout << Lido6(1,2,3,Inf,5,Inf) << "\n";
  // return 0;


  Profiler profiler;

  // const auto expr_asc_points = LidoVec(6, seq_incl(1, 6));

  // int num_zero = 0;
  // int num_nonzero = 0;
  // int i = 0;
  // for (const auto q : all_squences(4, 6)) {
  //   // const auto expr = project_on(axis,
  //   //   Lido6(mapped(q, [](int x) { return x + 1; }))
  //   // );
  //   const auto args = mapped(q, [](int x) { return X(x + 1); });
  //   const auto expr = project_on(axis,
  //     delta_expr_substitute(expr_asc_points, args)
  //   );
  //   if (expr.zero()) {
  //     ++num_zero;
  //   } else {
  //     ++num_nonzero;
  //     // const auto distinct = terms_with_min_distinct_elements(expr, 3);
  //     const auto distinct = terms_with_min_distinct_elements(to_lyndon_basis(expr), 3);
  //     // if (!distinct.zero() && distinct.size() <= 10) {
  //     if (!distinct.zero()) {
  //       std::cout << "Found: " << list_to_string(args) << " => " << distinct << std::endl;
  //     }
  //   }

  //   ++i;
  //   if (i % 100 == 0) {
  //     std::cout << "Zero: " << num_zero << "\n";
  //     std::cout << "Non-zero: " << num_nonzero << std::endl;
  //   }
  // }
  // std::cout << "DONE\n";
  // std::cout << "Zero: " << num_zero << "\n";
  // std::cout << "Non-zero: " << num_nonzero << std::endl;
  // return 0;



  for (int pr_axis = 1; pr_axis <= 6; ++pr_axis) {
    for (int a = 0; a < 6; ++a) {
      for (int b = a+1; b < 6; ++b) {
        std::vector<X> args{1, 2, 3, 4, 5, 6};
        args[a] = Inf;
        args[b] = Inf;
        auto expr = to_lyndon_basis(
          project_on(pr_axis,
          LidoVec(6, args))
        );
        auto filtered = terms_by_template(
          expr,
          Word{1, 1, 1, 3, 1, 2}
        );
        if (filtered.size() > 0) {
          std::cout << "Found: " << list_to_string(args) << " / " << pr_axis << " => " << filtered <<  "\n";
          // return 0;
        }
      }
    }
  }
  return 0;


  int axis = 4;

  auto lhs_raw = I6(2, 4);
  auto lhs_projected = to_lyndon_basis(project_on(axis, lhs_raw));
  auto lhs = terms_with_min_distinct_elements(lhs_projected, 3);

  // auto rhs_raw = Lido6(1,2,3,4,5,6);
  // auto rhs_raw = Lido6(4,Inf,3,Inf,2,1);
  // auto rhs_raw = Lido6(1,2,3,Inf,5,6);
  auto rhs_raw = Lido6(Inf,2,Inf,4,5,6);
  auto rhs_projected = to_lyndon_basis(project_on(5, rhs_raw));

  // auto rhs = terms_with_min_distinct_elements(
  //   rhs_projected,
  //   3
  // );
  auto rhs = terms_by_template(
    rhs_projected,
    Word{1, 1, 1, 3, 1, 2}
  );

  auto diff = lhs - rhs;
  // auto diff_lyndon = to_lyndon_basis(diff);

  profiler.finish("program");

  std::cout << "\n";
  std::cout << "LHS " << lhs << "\n";
  std::cout << "RHS " << rhs << "\n";
  std::cout << "Diff before Lyndon " << diff << "\n";
  // std::cout << "Diff after Lyndon " << diff_lyndon << "\n";

  // auto raw_diff = lhs_raw - rhs_raw;
  // auto raw_diff_lyndon = to_lyndon_basis(raw_diff);
  // std::cout << "RAW Diff before Lyndon " << raw_diff << "\n";
  // std::cout << "RAW Diff after Lyndon " << raw_diff_lyndon << "\n";

  // std::cout << "\n";
  // std::cout << "LHS " << project_on(axis, lhs) << "\n";
  // std::cout << "RHS " << project_on(axis, rhs) << "\n";
  // std::cout << "Diff before Lyndon " << project_on(axis, diff) << "\n";
  // std::cout << "Diff after Lyndon " << project_on(axis, diff_lyndon) << "\n";
}
