#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/summation.h"
#include "lib/theta.h"


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;


  // auto lhs = LidoSymm4(1,2,3,4,5,6);
  // auto colhs = comultiply(lhs, {2,2});
  // auto corhs =
  //   + coproduct(Lido2(1,2,3,4), Lido2(1,4,5,6))
  //   - coproduct(Lido2(2,3,4,5), Lido2(2,5,6,1))
  //   + coproduct(Lido2(3,4,5,6), Lido2(3,6,1,2))
  // ;
  // std::cout << colhs - corhs << "\n";

  // auto lhs = LidoSymm5(1,2,3,4,5,6);
  // auto colhs = comultiply(lhs, {2,3});
  // auto corhs = sum_looped(
  //   [](X x1, X x2, X x3, X x4, X x5, X x6) {
  //     return coproduct(Lido3(x1,x2,x3,x4), Lido2(x1,x4,x5,x6));
  //   },
  //   6);
  // std::cout << colhs - corhs << "\n";

  // auto lhs = LidoSymm6(1,2,3,4,5,6);
  // auto colhs = comultiply(lhs, {3,3});
  // auto corhs =
  //   + coproduct(Lido3(1,2,3,4), Lido3(1,4,5,6))
  //   - coproduct(Lido3(2,3,4,5), Lido3(2,5,6,1))
  //   + coproduct(Lido3(3,4,5,6), Lido3(3,6,1,2))
  // ;
  // std::cout << colhs - corhs << "\n";

  // auto lhs = LidoSymm5(1,2,3,4,5,6,7,8);
  // auto colhs = comultiply(lhs, {2,3});
  // // auto colhs =
  // //   + coproduct(Lido2(1,2,3,4), LidoSymm3(1,4,5,6,7,8))
  // //   + coproduct(Lido2(2,3,4,5), LidoSymm3(2,5,6,7,8,1))
  // //   + coproduct(Lido2(3,4,5,6), LidoSymm3(3,6,7,8,1,2))
  // //   + coproduct(Lido2(4,5,6,7), LidoSymm3(4,7,8,1,2,3))
  // //   + coproduct(Lido2(5,6,7,8), LidoSymm3(5,8,1,2,3,4))
  // //   + coproduct(Lido2(6,7,8,1), LidoSymm3(6,1,2,3,4,5))
  // //   + coproduct(Lido2(7,8,1,2), LidoSymm3(7,2,3,4,5,6))
  // //   + coproduct(Lido2(8,1,2,3), LidoSymm3(8,3,4,5,6,7))
  // //   + coproduct(Lido3(1,2,3,4), LidoSymm2(1,4,5,6,7,8))
  // //   + coproduct(Lido3(2,3,4,5), LidoSymm2(2,5,6,7,8,1))
  // //   + coproduct(Lido3(3,4,5,6), LidoSymm2(3,6,7,8,1,2))
  // //   + coproduct(Lido3(4,5,6,7), LidoSymm2(4,7,8,1,2,3))
  // //   + coproduct(Lido3(5,6,7,8), LidoSymm2(5,8,1,2,3,4))
  // //   + coproduct(Lido3(6,7,8,1), LidoSymm2(6,1,2,3,4,5))
  // //   + coproduct(Lido3(7,8,1,2), LidoSymm2(7,2,3,4,5,6))
  // //   + coproduct(Lido3(8,1,2,3), LidoSymm2(8,3,4,5,6,7))
  // // ;
  // auto corhs = sum_looped(
  //   [](X x1, X x2, X x3, X x4, X x5, X x6, X x7, X x8) {
  //     return
  //       + coproduct(Lido2(x1,x2,x3,x4), LidoSymm3(x1,x4,x5,x6,x7,x8))
  //       + coproduct(Lido3(x1,x2,x3,x4), LidoSymm2(x1,x4,x5,x6,x7,x8))
  //     ;
  //   },
  //   8);
  // auto diff = colhs - corhs;
  // std::cout << diff;  // NOT zero




  // auto lhs = LidoSymm2(1,2,3,4,5,6);
  // auto co_lhs = comultiply<DeltaCoExpr>(lhs, {1,1});
  // auto co_rhs =
  //   + coproduct(
  //     delta_expr_substitute(DreamCorrBundle(2, 4), {1,2,3,4}),
  //     delta_expr_substitute(DreamCorrBundle(2, 4), {1,4,5,6})
  //   )
  //   + coproduct(
  //     delta_expr_substitute(DreamCorrBundle(2, 4), {3,4,5,6}),
  //     delta_expr_substitute(DreamCorrBundle(2, 4), {3,6,1,2})
  //   )
  //   + coproduct(
  //     delta_expr_substitute(DreamCorrBundle(2, 4), {5,6,1,2}),
  //     delta_expr_substitute(DreamCorrBundle(2, 4), {5,2,3,4})
  //   )
  // ;
  // std::cout << co_lhs << "\n";
  // std::cout << co_rhs << "\n";
  // std::cout << co_lhs - co_rhs << "\n";


  auto lhs = to_lyndon_basis(
    theta_expr_to_delta_expr(eval_formal_symbols(
      LiQuad(1, {1,2,3,4,5,6}, LiFirstPoint::even))
    )
  );
  auto rhs = to_lyndon_basis(
    LidoNeg2(1,2,3,4,5,6)
  );
  auto diff = lhs - rhs;

  profiler.finish("all");
  std::cout << "\n";

  std::cout << lhs << "\n";
  std::cout << rhs << "\n";
  std::cout << diff << "\n";


  // profiler.finish("expr");

  // auto lyndon = to_lyndon_basis(diff);
  // profiler.finish("lyndon");
  // std::cout << "\n";
  // std::cout << lyndon << "\n";
  // print_sorted_by_num_distinct_variables(std::cout, lyndon);

  // auto lyndon = to_lyndon_basis(project_on_x1(expr));
  // profiler.finish("lyndon");
  // std::cout << "\n";
  // print_sorted_by_num_distinct_elements(std::cout, lyndon);
}
