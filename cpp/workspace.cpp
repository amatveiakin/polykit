#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/polylog.h"
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
  // auto expr = Li(3, {1,2,3,4});

  // std::cout << tensor_product(D(1, 2) + 2*D(3, 4), -D(5, 6) + 3*D(7, 8)) << "\n";
  // std::cout << str_join(shuffle_product({
  //   IntWord{0,1,0},
  //   IntWord{1,0},
  //   IntWord{1},
  // }), "\n") << "\n";

  // Profiler profiler;
  // // auto expr = Li6(1,2,3,4,5,6,7,8);
  // auto expr = Li6(1,2,3,4,5,6);
  // profiler.finish("expr");
  // std::cout << "Num terms before Lyndon = " << expr.size() << "\n";
  // auto lyndon = to_lyndon_basis(expr);
  // profiler.finish("lyndon");
  // std::cout << "Num terms after Lyndon = " << lyndon.size() << "\n";

  // auto expr = I(1,2,3,4);
  // auto expr = project_on_x1(I({1,2,3,4,5}));
  // auto expr = project_on_x1(Li4(1,2,3,4));
  // auto expr = project_on_x2(Li4(2,3,4,5));
  // auto expr = Li4(4,2,3,4);
  // expr = expr.without_annotations();
  // std::cout << "Before Lyndon " << expr << "\n";
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << "After Lyndon " << lyndon << "\n";

  // auto expr = Li4(1,2,3,4);
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
  //   to_lyndon_basis(project_on_x1(Li(w, seq_incl(1, n)))),
  //   distinct) << "\n";
  // profiler.finish("expr");
  // return 0;

  auto lhs = comultiply(I(1,2,3,4,5,6), {2,2});
  auto rhs = (
      + coproduct(I(1,2,3,4), I(1,4,5,6))
      + coproduct(I(2,3,4,5), I(1,2,5,6))
      + coproduct(I(3,4,5,6), I(1,2,3,6))
  );

  std::cout << "LHS " << lhs << "\n";
  std::cout << "RHS " << rhs << "\n";
  std::cout << "Diff " << (lhs - rhs) << "\n";
}
