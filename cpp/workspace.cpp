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
