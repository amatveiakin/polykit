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

  // int w = 8;
  // int n = 10;
  // int distinct = std::min(w, n-1);
  // Profiler profiler;
  // std::cout << "Expr " << terms_with_min_distinct_elements(
  //   to_lyndon_basis(project_on_x1(Lira(w, seq_incl(1, n)))),
  //   distinct) << "\n";
  // profiler.finish("expr");
  // return 0;

  Profiler profiler;

  // auto expr = (
  //   + Li(1,2)({1},{2})
  //   + Li(2,1)({2},{1})
  //   - Li(3)  ({1,2})
  // );

  // TODO: to tests
  // auto expr = (
  //   + CoLi({1,2}, {{1},{2}})
  //   + CoLi({2,1}, {{2},{1}})
  //   - CoLi({3},   {{1,2}})
  // );

  auto a = CoLi({1,2}, {{1},{2}});
  auto b = CoLi({2,1}, {{2},{1}});
  auto c = -CoLi({3},   {{1,2}});
  auto expr = a + b + c;

  std::cout << "\n";
  profiler.finish("expr");
  // auto lyndon = to_lyndon_basis(expr);
  // profiler.finish("lyndon");
  std::cout << "\n";
  std::cout << "A " << a << "\n";
  std::cout << "B " << b << "\n";
  std::cout << "C " << c << "\n";
  std::cout << "Expr " << expr << "\n";
  // std::cout << "After Lyndon " << lyndon << "\n";
  // std::cout << "Without monsters " << epsilon_expr_without_monsters(lyndon) << "\n";
  // std::cout << "After Lyndon: " << lyndon.size() << " terms, |coeff| = " << lyndon.l1_norm() << "\n";

  // auto lhs = to_lyndon_basis(Lira2(1,2,3,4));
  // auto rhs = to_lyndon_basis(  // TODO: Find expression
  //     I(1,2,3,4)
  // );
  // std::cout << "LHS " << lhs << "\n";
  // std::cout << "RHS " << rhs << "\n";
  // std::cout << "Diff " << (lhs - rhs) << "\n";
}
