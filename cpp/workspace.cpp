#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_multiarg.h"
#include "lib/iterated_integral.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/shuffle.h"


int main(int argc, char *argv[]) {
  // TODO: move; fix
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  // std::cout << list_to_string(shuffle_product(IntWord{1,2}, IntWord{3,4})) << "\n";
  // std::cout << "lynd fact:\n" << list_to_string(lyndon_factorize(IntWord{1,0,1,1,0,0,1})) << "\n";
  // auto expr = Lira(3, {1,2,3,4});

  // std::cout << tensor_product(D(1, 2) + 2*D(3, 4), -D(5, 6) + 3*D(7, 8)) << "\n";
  // std::cout << str_join(shuffle_product({
  //   IntWord{0,1,0},
  //   IntWord{1,0},
  //   IntWord{1},
  // }), "\n") << "\n";

  // Profiler profiler;
  // // auto expr = Lira6(1,2,3,4,5,6,7,8);
  // auto expr = Lira6(1,2,3,4,5,6);
  // profiler.finish("expr");
  // std::cout << "Num terms before Lyndon = " << expr.size() << "\n";
  // auto lyndon = to_lyndon_basis(expr);
  // profiler.finish("lyndon");
  // std::cout << "Num terms after Lyndon = " << lyndon.size() << "\n";

  // auto expr = I(1,2,3,4);
  // auto expr = project_on_x1(I({1,2,3,4,5}));
  // auto expr = project_on_x1(Lira4(1,2,3,4));
  // auto expr = project_on_x2(Lira4(2,3,4,5));
  // auto expr = Lira4(4,2,3,4);
  // expr = expr.without_annotations();
  // std::cout << "Before Lyndon " << expr << "\n";
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << "After Lyndon " << lyndon << "\n";

  // auto expr = Lira4(1,2,3,4);
  // std::cout << "Before Lyndon " << expr << "\n";
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << "After Lyndon " << lyndon << "\n";
  // auto coprod = comultiply<DeltaCoExpr>(expr, {1,3});
  // std::cout << "Comult " << coprod << "\n";

  // int w = 8;
  // int n = 10;
  // int distinct = std::min(w, n-1);
  // Profiler profiler;
  // std::cout << "Expr " << terms_with_min_distinct_elements(
  //   to_lyndon_basis(project_on_x1(Lira(w, seq_incl(1, n)))),
  //   distinct) << "\n";
  // profiler.finish("expr");
  // return 0;

  // auto lhs = comultiply(I(1,2,3,4,5,6), {2,2});
  // auto rhs = (
  //     + coproduct(I(1,2,3,4), I(1,4,5,6))
  //     + coproduct(I(2,3,4,5), I(1,2,5,6))
  //     + coproduct(I(3,4,5,6), I(1,2,3,6))
  // );

  // std::cout << "LHS " << lhs << "\n";
  // std::cout << "RHS " << rhs << "\n";
  // std::cout << "Diff " << (lhs - rhs) << "\n";

  Profiler profiler;

  auto expr = (
    + Lily({1,1,1}, {{1},{2},{3}})
    + Lily({1,1,1}, {{1},{3},{2}})
    + Lily({1,1,1}, {{3},{1},{2}})
    - Lily({1,2},   {{1},{2,3}})
    - Lily({2,1},   {{1,3},{2}})
  )
  -
  shuffle_product_expr(
    Lily({1,1}, {{1},{2}}),
    Lily({1},   {{3}})
  );

  // auto expr = (
  //   + Lily({1,1,2}, {{1},{2},{3}})
  //   + Lily({1,2,1}, {{1},{3},{2}})
  //   + Lily({2,1,1}, {{3},{1},{2}})
  //   - Lily({1,3},   {{1},{2,3}})
  //   - Lily({3,1},   {{1,3},{2}})
  // );

  // auto expr = (
  //   + Lily({2,2,2}, {{1},{2},{3}})
  //   + Lily({2,2,2}, {{1},{3},{2}})
  //   + Lily({2,2,2}, {{3},{1},{2}})
  //   - Lily({2,4},   {{1},{2,3}})
  //   - Lily({4,2},   {{1,3},{2}})
  // );

  // auto expr = (
  //   + Lily({2,2,2,2}, {{1},{2},{3},{4}})
  //   + Lily({2,2,2,2}, {{1},{2},{4},{3}})
  //   + Lily({2,2,2,2}, {{1},{4},{2},{3}})
  //   + Lily({2,2,2,2}, {{4},{1},{2},{3}})
  //   - Lily({2,2,4},   {{1},{2},{3,4}})
  //   - Lily({2,4,2},   {{1},{2,4},{3}})
  //   - Lily({4,2,2},   {{1,4},{2},{3}})
  // );

  // auto expr = (
  //   + Lily({2,2,2,2}, {{1},{2},{3},{4}})
  //   + Lily({2,2,2,2}, {{1},{2},{4},{3}})
  //   + Lily({2,2,2,2}, {{1},{4},{2},{3}})
  //   + Lily({2,2,2,2}, {{4},{1},{2},{3}})
  //   - Lily({2,2,4},   {{1},{2},{3,4}})
  //   - Lily({2,4,2},   {{1},{2,4},{3}})
  //   - Lily({4,2,2},   {{1,4},{2},{3}})
  // )
  // -
  // shuffle_product_expr(
  //   Lily({2,2,2}, {{1},{2},{3}}),
  //   Lily({2},     {{4}})
  // );

  // auto expr = (
  //   + Lily({1,2}, {{1},{2}})
  //   + Lily({2,1}, {{2},{1}})
  //   - Lily({3},   {{1,2}})
  // );

  std::cout << "\n";
  profiler.finish("expr");
  auto lyndon = to_lyndon_basis(expr);
  profiler.finish("lyndon");
  std::cout << "\n";
  std::cout << "Before Lyndon " << expr << "\n";
  std::cout << "After Lyndon " << lyndon << "\n";
  std::cout << "Without monsters " << epsilon_expr_without_monsters(lyndon) << "\n";
  // std::cout << "After Lyndon: " << lyndon.size() << " terms, |coeff| = " << lyndon.l1_norm() << "\n";

  // auto lhs = to_lyndon_basis(Lira2(1,2,3,4));
  // auto rhs = to_lyndon_basis(  // TODO: Find expression
  //     I(1,2,3,4)
  // );
  // std::cout << "LHS " << lhs << "\n";
  // std::cout << "RHS " << rhs << "\n";
  // std::cout << "Diff " << (lhs - rhs) << "\n";
}
