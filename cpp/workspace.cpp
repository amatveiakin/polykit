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


DeltaExpr CorrLoop(int x1, int x2, int x3, int x4, int x5) {
  DeltaExpr ret;
  for (int i = 0; i < 6; ++i) {
    auto sh = [&](int idx){ return (idx - 1 + i) % 6 + 1; };
    ret += neg_one_pow(i) * Corr(sh(x1), sh(x2), sh(x3), sh(x4), sh(x5));
  }
  return ret;
}


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  // std::cout << "Corr " << to_lyndon_basis(Corr(1,1,2,3,5,6)) << "\n";
  // auto expr = Lido5(1,2,3,4,5,6);
  // auto expr = Lido5(1,2,3,4,5,6) - Corr(1,2,3,4,5,6);
  auto expr = (
    + LidoSymm4(1,2,3,4,5,6)

    + Corr(1,2,3,4,5)
    - Corr(1,2,3,4,6)
    + Corr(1,2,3,5,6)
    - Corr(1,2,4,5,6)
    + Corr(1,3,4,5,6)
    - Corr(2,3,4,5,6)

    - CorrLoop(1,2,2,3,4)
    - CorrLoop(1,2,2,5,6)
    // - CorrLoop(1,4,4,5,6)
  ).without_annotations();
  auto lyndon = to_lyndon_basis(expr);
  auto src = lyndon;
  // auto lyndon = to_lyndon_basis(project_on_x1(expr));
  auto filtered = terms_containing_only_variables(lyndon, {1,2,3});
  // std::cout << terms_with_min_distinct_elements(lyndon, 3) << "\n";
  std::cout << filtered << "\n";
  // for (int i = 3; i <= 6; ++i) {
  //   std::cout << i << " vars " << terms_containing_num_variables(src, i) << "\n";
  // }
  // std::cout << contains_only_variables(src, {1,2,3,4,5}) << "\n";
  // std::cout << keep_connected_graphs(src) << "\n";
}
