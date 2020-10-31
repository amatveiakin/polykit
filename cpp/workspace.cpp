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
#include "lib/shuffle.h"
#include "lib/theta.h"


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;

  profiler.finish("expr");

  std::cout << "\n";
  // std::cout << "Expr " << expr << "\n";
}
