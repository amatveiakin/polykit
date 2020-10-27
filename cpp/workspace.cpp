#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog.h"
#include "lib/iterated_integral.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/shuffle.h"


int main(int argc, char *argv[]) {
  // TODO: move; fix
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;

  auto expr =
    + CoLi(1,1)({1},{2})
    + CoLi(1,1)({2},{1})
    + CoLi(2)  ({1,2})
    - mystic_product(
      CoLi(1)({1}),
      CoLi(1)({2})
    );

  // auto a = CoLi(1,3)({1},{2});
  // auto b = CoLi(3,1)({2},{1});
  // auto c = CoLi(4)  ({1,2});
  // auto d1 = CoLi(1)({1});
  // auto d2 = CoLi(3)({2});
  // auto d = mystic_product(d1, d2);
  // auto expr = a + b + c - d;

  std::cout << "\n";
  profiler.finish("expr");
  // auto lyndon = to_lyndon_basis(expr);
  // profiler.finish("lyndon");
  std::cout << "\n";
  // std::cout << "A " << a << "\n";
  // std::cout << "B " << b << "\n";
  // std::cout << "C " << c << "\n";
  // std::cout << "D " << d << "\n";
  std::cout << "Expr " << expr << "\n";
  // auto filtered = filter_coexpr(expr, 0, LiParam({1},{{1,2}}));
  // std::cout << "Expr filtered " << filtered << "\n";
  // std::cout << "After Lyndon " << lyndon << "\n";
  // std::cout << "After Lyndon: " << lyndon.size() << " terms, |coeff| = " << lyndon.l1_norm() << "\n";
}
