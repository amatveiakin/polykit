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


enum class Sign {
  plus,
  alternating,
};

enum class Loop {
  half,
  full,
};

template<typename F>
auto sum_looped(
    const F& func,
    int max_arg_value,
    const std::vector<int>& starting_args,
    Sign summation_sign,
    Loop loop = Loop::full) {
  int shift_step = 0;
  switch (loop) {
    case Loop::full:
      shift_step = 1;
      break;
    case Loop::half:
      CHECK(max_arg_value % 2 == 0);
      shift_step = 2;
      break;
  }
  std::invoke_result_t<F, std::vector<X>> ret;
  for (int shift = 0; shift < max_arg_value; shift += shift_step) {
    std::vector<X> args;
    for (const int a : starting_args) {
      CHECK_LE(1, a);
      CHECK_LE(a, max_arg_value);
      args.push_back((a - 1 + shift) % max_arg_value + 1);
    }
    int sign = 0;
    switch (summation_sign) {
      case Sign::plus: sign = 1; break;
      case Sign::alternating: sign = neg_one_pow(shift); break;
    }
    ret += sign * func(args);
  }
  return ret;
}

DeltaExpr LiSon(int x1, int x2, int x3, int x4, int x5, int x6) {
  return
    +2*Corr(x1,x1,x1,x2,x4,x6)
     + Corr(x1,x1,x2,x2,x3,x6)
     + Corr(x1,x1,x2,x2,x4,x5)
    +2*Corr(x1,x1,x2,x3,x4,x6)
     + Corr(x1,x1,x2,x3,x6,x6)
     + Corr(x1,x1,x2,x4,x4,x5)
    +2*Corr(x1,x1,x2,x4,x5,x6)
     + Corr(x1,x1,x3,x4,x4,x6)
     + Corr(x1,x1,x3,x4,x6,x6)
     + Corr(x1,x1,x4,x4,x5,x6)
     + Corr(x1,x1,x4,x5,x6,x6)
    +2*Corr(x1,x2,x2,x2,x3,x5)
     + Corr(x1,x2,x2,x3,x3,x6)
    +2*Corr(x1,x2,x2,x3,x4,x5)
    +2*Corr(x1,x2,x2,x3,x5,x6)
     + Corr(x1,x2,x2,x4,x5,x5)
    +2*Corr(x1,x2,x3,x3,x4,x6)
     + Corr(x1,x2,x3,x3,x6,x6)
    +2*Corr(x1,x2,x3,x4,x4,x5)
    +2*Corr(x1,x2,x3,x4,x5,x6)
    +2*Corr(x1,x2,x3,x5,x6,x6)
     + Corr(x1,x2,x4,x4,x5,x5)
    +2*Corr(x1,x2,x4,x5,x5,x6)
     + Corr(x1,x3,x3,x4,x4,x6)
     + Corr(x1,x3,x3,x4,x6,x6)
    +2*Corr(x1,x3,x4,x4,x4,x5)
    +2*Corr(x1,x3,x4,x4,x5,x6)
    +2*Corr(x1,x3,x4,x5,x6,x6)
    +2*Corr(x1,x3,x5,x6,x6,x6)
     + Corr(x1,x4,x4,x5,x5,x6)
     + Corr(x1,x4,x5,x5,x6,x6)
     + Corr(x2,x2,x3,x3,x4,x5)
     + Corr(x2,x2,x3,x3,x5,x6)
     + Corr(x2,x2,x3,x4,x5,x5)
     + Corr(x2,x2,x3,x5,x5,x6)
    +2*Corr(x2,x3,x3,x3,x4,x6)
     + Corr(x2,x3,x3,x4,x4,x5)
    +2*Corr(x2,x3,x3,x4,x5,x6)
     + Corr(x2,x3,x3,x5,x6,x6)
     + Corr(x2,x3,x4,x4,x5,x5)
    +2*Corr(x2,x3,x4,x5,x5,x6)
     + Corr(x2,x3,x5,x5,x6,x6)
    +2*Corr(x2,x4,x5,x5,x5,x6)
  ;
}

DeltaExpr LydoSymm7(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8) {
  return
    + Lido7(x1,x2,x3,x4,x5,x6,x7,x8)
    - Lido7(x1,x2,x3,x4,x5,x6)
    - Lido7(x3,x4,x5,x6,x7,x8)
    - Lido7(x5,x6,x7,x8,x1,x2)
    - Lido7(x7,x8,x1,x2,x3,x4)
  ;
}

int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;

  // std::cout << "Corr " << to_lyndon_basis(Corr(1,1,2,3,5,6)) << "\n";
  // auto expr = Lido5(1,2,3,4,5,6);
  // auto expr = Lido5(1,2,3,4,5,6) - Corr(1,2,3,4,5,6);
  // auto expr = (
  //   + LidoSymm4(1,2,3,4,5,6)
  //   + Corr(1,2,3,4,5)
  //   - Corr(1,2,3,4,6)
  //   + Corr(1,2,3,5,6)
  //   - Corr(1,2,4,5,6)
  //   + Corr(1,3,4,5,6)
  //   - Corr(2,3,4,5,6)
  //   - CorrLoop(1,2,2,3,4)
  //   - CorrLoop(1,2,2,5,6)
  // ).without_annotations();

  // auto expr = (
  //   + Corr(1,1,2,3,4,5)
  //   + Corr(1,2,2,3,4,5)
  //   + Corr(1,2,3,3,4,5)
  //   + Corr(1,2,3,4,4,5)
  //   + Corr(1,2,3,4,5,5)
  // );

  // auto expr = (
  //   + Corr(1,1,2,3,4)
  //   + Corr(1,2,2,3,4)
  //   + Corr(1,2,3,3,4)
  //   + Corr(1,2,3,4,4)
  // );

  // auto expr = (
  //   + Corr(1,1,1,2,3)
  //   + Corr(1,1,2,2,3)
  //   + Corr(1,1,2,3,3)
  //   + Corr(1,2,2,2,3)
  //   + Corr(1,2,2,3,3)
  //   + Corr(1,2,3,3,3)
  // );

  // auto expr = (
  //   + Corr(1,1,1,2,3)
  //   - Corr(1,2,2,2,3)
  //   - Corr(1,2,2,3,3)
  //   - Corr(1,2,3,3,3)
  // );

  // auto expr = (
  //   + Corr(1,2,3,4,4,4)
  //   - Corr(1,1,1,2,3,4)
  //   - Corr(1,1,2,2,3,4)
  //   - Corr(1,1,2,3,3,4)
  //   - Corr(1,2,2,2,3,4)
  //   - Corr(1,2,2,3,3,4)
  //   - Corr(1,2,3,3,3,4)
  // );

  // auto expr =
  //   - Lido5(1,2,3,4,5,6)
  //   + Corr(1,2,3,4,5,6)
  //   + sum_looped(&CorrVec, 6, {1,1,2,4,5,6}, Sign::plus)
  //   + sum_looped(&CorrVec, 6, {1,1,2,3,4,6}, Sign::plus)
  // ;

  // std::cout << to_lyndon_basis(project_on_x1(Corr(1,1,2,2,4,5))) << "\n";
  // std::cout << to_lyndon_basis(project_on_x1(Corr(1,1,2,4,4,5))) << "\n";
  // std::cout << to_lyndon_basis(project_on_x1(Corr(1,2,2,4,5,5))) << "\n";
  // std::cout << to_lyndon_basis(project_on_x1(Corr(1,2,4,4,5,5))) << "\n";
  // return 0;

  // auto expr =
  //   -2 * Lido5(1,2,3,4,5,6)
  //   +2 * Corr(1,2,3,4,5,6)
  //   +2 * sum_looped(&CorrVec, 6, {1,1,2,4,5,6}, Sign::plus)
  //   +2 * sum_looped(&CorrVec, 6, {1,1,2,3,4,6}, Sign::plus)
  //   +2 * sum_looped(&CorrVec, 6, {1,2,2,2,3,5}, Sign::plus)
  //   + sum_looped(&CorrVec, 6, {1,1,2,2,4,5}, Sign::plus)
  //   + Corr(1,1,2,4,4,5)
  //   + Corr(2,2,3,5,5,6)
  //   + Corr(3,3,4,6,6,1)
  //   + Corr(1,2,2,4,5,5)
  //   + Corr(2,3,3,5,6,6)
  //   + Corr(3,4,4,6,1,1)
  //   + sum_looped(&CorrVec, 6, {1,1,2,2,3,6}, Sign::plus, Loop::half)
  //   + sum_looped(&CorrVec, 6, {1,1,2,3,6,6}, Sign::plus, Loop::half)
  //   + sum_looped(&CorrVec, 6, {1,2,2,3,3,6}, Sign::plus, Loop::half)
  //   + sum_looped(&CorrVec, 6, {1,2,3,3,6,6}, Sign::plus, Loop::half)
  // ;

  auto expr =
    -2 * Lido5(1,2,3,4,5,6)
    +2 * Corr(1,2,3,4,5,6)
    +2 * sum_looped(&CorrVec, 6, {1,1,2,4,5,6}, Sign::plus)
    +2 * sum_looped(&CorrVec, 6, {1,1,2,3,4,6}, Sign::plus)
    +2 * sum_looped(&CorrVec, 6, {1,2,2,2,3,5}, Sign::plus)
    + sum_looped(&CorrVec, 6, {1,1,2,2,4,5}, Sign::plus)
    + sum_looped(&CorrVec, 6, {1,1,2,4,4,5}, Sign::plus, Loop::half)
    + sum_looped(&CorrVec, 6, {1,2,2,4,5,5}, Sign::plus, Loop::half)
    + sum_looped(&CorrVec, 6, {1,1,2,2,3,6}, Sign::plus, Loop::half)
    + sum_looped(&CorrVec, 6, {1,1,2,3,6,6}, Sign::plus, Loop::half)
    + sum_looped(&CorrVec, 6, {1,2,2,3,3,6}, Sign::plus, Loop::half)
    + sum_looped(&CorrVec, 6, {1,2,3,3,6,6}, Sign::plus, Loop::half)
  ;  // Has at most 3 vars per term

  // auto expr =
  //   -2 * LidoSymm5(1,2,3,4,5,6)
  //   + LiSon(1,2,3,4,5,6)
  //   + Corr(1,1,2,2,3,4)
  //   + Corr(1,1,2,2,5,6)
  //   + Corr(1,1,2,3,4,4)
  //   + Corr(1,1,2,5,6,6)
  //   + Corr(1,2,2,3,3,4)
  //   + Corr(1,2,2,5,5,6)
  //   + Corr(1,2,3,3,4,4)
  //   + Corr(1,2,5,5,6,6)
  //   + Corr(3,3,4,4,5,6)
  //   + Corr(3,3,4,5,6,6)
  //   + Corr(3,4,4,5,5,6)
  //   + Corr(3,4,5,5,6,6)
  // ;

  // std::cout << to_lyndon_basis(project_on_x1(
  //   + LydoSymm7(1,2,3,4,5,6,7,8)
  //   - LydoSymm7(2,3,4,5,6,7,8,1)
  // ));
  // return 0;

  // auto expr =
  //   // -2*LidoVecPr(7, {1,2,3,4,5,6,7,8}, project_on_x1)
  //   + project_on_x1 (
  //     -2*LydoSymm7(1,2,3,4,5,6,7,8)

  //     +2*Corr(1,2,3,4,5,6,7,8)

  //     +2*Corr(1,1,2,3,4,5,6,8)
  //     +2*Corr(1,1,2,3,4,6,7,8)
  //     +2*Corr(1,1,2,4,5,6,7,8)
  //     +2*Corr(1,2,2,3,4,5,6,7)
  //     +2*Corr(1,2,2,3,4,5,7,8)
  //     +2*Corr(1,2,2,3,5,6,7,8)
  //     +2*Corr(1,2,3,3,4,5,6,8)
  //     +2*Corr(1,2,3,3,4,6,7,8)
  //     +2*Corr(1,2,3,4,4,5,6,7)
  //     +2*Corr(1,2,3,4,4,5,7,8)
  //     +2*Corr(1,2,3,4,5,5,6,8)
  //     +2*Corr(1,2,3,4,5,6,6,7)
  //     +2*Corr(1,2,3,4,5,7,8,8)
  //     +2*Corr(1,2,3,4,6,7,7,8)
  //     +2*Corr(1,2,3,5,6,6,7,8)
  //     +2*Corr(1,2,3,5,6,7,8,8)
  //     +2*Corr(1,2,4,5,5,6,7,8)
  //     +2*Corr(1,2,4,5,6,7,7,8)
  //     +2*Corr(1,3,4,4,5,6,7,8)
  //     +2*Corr(1,3,4,5,6,6,7,8)
  //     +2*Corr(1,3,4,5,6,7,8,8)
  //     +2*Corr(2,3,3,4,5,6,7,8)
  //     +2*Corr(2,3,4,5,5,6,7,8)
  //     +2*Corr(2,3,4,5,6,7,7,8)

  //     +2*Corr(1,1,1,2,3,4,6,8)
  //     +2*Corr(1,1,1,2,4,5,6,8)
  //     +2*Corr(1,1,1,2,4,6,7,8)
  //     +  Corr(1,1,2,2,3,4,5,6)
  //     +  Corr(1,1,2,2,3,4,5,8)
  //     +  Corr(1,1,2,2,3,4,6,7)
  //     +  Corr(1,1,2,2,3,4,7,8)
  //     +  Corr(1,1,2,2,3,5,6,8)
  //     +  Corr(1,1,2,2,3,6,7,8)
  //     +  Corr(1,1,2,2,4,5,6,7)
  //     +  Corr(1,1,2,2,4,5,7,8)
  //     +  Corr(1,1,2,2,5,6,7,8)
  //     +  Corr(1,1,2,3,3,4,6,8)
  //     +  Corr(1,1,2,3,4,4,5,6)
  //     +  Corr(1,1,2,3,4,4,5,8)
  //     +  Corr(1,1,2,3,4,4,6,7)
  //     +  Corr(1,1,2,3,4,4,7,8)
  //     +  Corr(1,1,2,3,4,5,6,6)
  //     +  Corr(1,1,2,3,4,5,8,8)
  //     +  Corr(1,1,2,3,4,6,6,7)
  //     +  Corr(1,1,2,3,4,7,8,8)
  //     +  Corr(1,1,2,3,5,6,6,8)
  //     +  Corr(1,1,2,3,5,6,8,8)
  //     +  Corr(1,1,2,3,6,6,7,8)
  //     +  Corr(1,1,2,3,6,7,8,8)
  //     +  Corr(1,1,2,4,4,5,6,7)
  //     +  Corr(1,1,2,4,4,5,7,8)
  //     +  Corr(1,1,2,4,5,5,6,8)
  //     +  Corr(1,1,2,4,5,6,6,7)
  //     +  Corr(1,1,2,4,5,7,8,8)
  //     +  Corr(1,1,2,4,6,7,7,8)
  //     +  Corr(1,1,2,5,6,6,7,8)
  //     +  Corr(1,1,2,5,6,7,8,8)
  //     +  Corr(1,1,3,4,4,5,6,8)
  //     +  Corr(1,1,3,4,4,6,7,8)
  //     +  Corr(1,1,3,4,5,6,6,8)
  //     +  Corr(1,1,3,4,5,6,8,8)
  //     +  Corr(1,1,3,4,6,6,7,8)
  //     +  Corr(1,1,3,4,6,7,8,8)
  //     +  Corr(1,1,4,4,5,6,7,8)
  //     +  Corr(1,1,4,5,6,6,7,8)
  //     +  Corr(1,1,4,5,6,7,8,8)
  //     +2*Corr(1,2,2,2,3,4,5,7)
  //     +2*Corr(1,2,2,2,3,5,6,7)
  //     +2*Corr(1,2,2,2,3,5,7,8)
  //     +  Corr(1,2,2,3,3,4,5,6)
  //     +  Corr(1,2,2,3,3,4,5,8)
  //     +  Corr(1,2,2,3,3,4,6,7)
  //     +  Corr(1,2,2,3,3,4,7,8)
  //     +  Corr(1,2,2,3,3,5,6,8)
  //     +  Corr(1,2,2,3,3,6,7,8)
  //     +  Corr(1,2,2,3,4,4,5,7)
  //     +  Corr(1,2,2,3,4,5,5,6)
  //     +  Corr(1,2,2,3,4,5,5,8)
  //     +  Corr(1,2,2,3,4,6,7,7)
  //     +  Corr(1,2,2,3,4,7,7,8)
  //     +  Corr(1,2,2,3,5,5,6,8)
  //     +  Corr(1,2,2,3,5,6,6,7)
  //     +  Corr(1,2,2,3,5,7,8,8)
  //     +  Corr(1,2,2,3,6,7,7,8)
  //     +  Corr(1,2,2,4,5,5,6,7)
  //     +  Corr(1,2,2,4,5,5,7,8)
  //     +  Corr(1,2,2,4,5,6,7,7)
  //     +  Corr(1,2,2,4,5,7,7,8)
  //     +  Corr(1,2,2,5,5,6,7,8)
  //     +  Corr(1,2,2,5,6,7,7,8)
  //     +2*Corr(1,2,3,3,3,4,6,8)
  //     +  Corr(1,2,3,3,4,4,5,6)
  //     +  Corr(1,2,3,3,4,4,5,8)
  //     +  Corr(1,2,3,3,4,4,6,7)
  //     +  Corr(1,2,3,3,4,4,7,8)
  //     +  Corr(1,2,3,3,4,5,6,6)
  //     +  Corr(1,2,3,3,4,5,8,8)
  //     +  Corr(1,2,3,3,4,6,6,7)
  //     +  Corr(1,2,3,3,4,7,8,8)
  //     +  Corr(1,2,3,3,5,6,6,8)
  //     +  Corr(1,2,3,3,5,6,8,8)
  //     +  Corr(1,2,3,3,6,6,7,8)
  //     +  Corr(1,2,3,3,6,7,8,8)
  //     +2*Corr(1,2,3,4,4,4,5,7)
  //     +  Corr(1,2,3,4,4,5,5,6)
  //     +  Corr(1,2,3,4,4,5,5,8)
  //     +  Corr(1,2,3,4,4,6,7,7)
  //     +  Corr(1,2,3,4,4,7,7,8)
  //     +  Corr(1,2,3,4,5,5,6,6)
  //     +  Corr(1,2,3,4,5,5,8,8)
  //     +  Corr(1,2,3,4,6,6,7,7)
  //     +  Corr(1,2,3,4,7,7,8,8)
  //     +  Corr(1,2,3,5,5,6,6,8)
  //     +  Corr(1,2,3,5,5,6,8,8)
  //     +2*Corr(1,2,3,5,6,6,6,7)
  //     +2*Corr(1,2,3,5,7,8,8,8)
  //     +  Corr(1,2,3,6,6,7,7,8)
  //     +  Corr(1,2,3,6,7,7,8,8)
  //     +  Corr(1,2,4,4,5,5,6,7)
  //     +  Corr(1,2,4,4,5,5,7,8)
  //     +  Corr(1,2,4,4,5,6,7,7)
  //     +  Corr(1,2,4,4,5,7,7,8)
  //     +2*Corr(1,2,4,5,5,5,6,8)
  //     +  Corr(1,2,4,5,5,6,6,7)
  //     +  Corr(1,2,4,5,5,7,8,8)
  //     +  Corr(1,2,4,5,6,6,7,7)
  //     +  Corr(1,2,4,5,7,7,8,8)
  //     +2*Corr(1,2,4,6,7,7,7,8)
  //     +  Corr(1,2,5,5,6,6,7,8)
  //     +  Corr(1,2,5,5,6,7,8,8)
  //     +  Corr(1,2,5,6,6,7,7,8)
  //     +  Corr(1,2,5,6,7,7,8,8)
  //     +  Corr(1,3,3,4,4,5,6,8)
  //     +  Corr(1,3,3,4,4,6,7,8)
  //     +  Corr(1,3,3,4,5,6,6,8)
  //     +  Corr(1,3,3,4,5,6,8,8)
  //     +  Corr(1,3,3,4,6,6,7,8)
  //     +  Corr(1,3,3,4,6,7,8,8)
  //     +2*Corr(1,3,4,4,4,5,6,7)
  //     +2*Corr(1,3,4,4,4,5,7,8)
  //     +  Corr(1,3,4,4,5,5,6,8)
  //     +  Corr(1,3,4,4,5,6,6,7)
  //     +  Corr(1,3,4,4,5,7,8,8)
  //     +  Corr(1,3,4,4,6,7,7,8)
  //     +  Corr(1,3,4,5,5,6,6,8)
  //     +  Corr(1,3,4,5,5,6,8,8)
  //     +2*Corr(1,3,4,5,6,6,6,7)
  //     +2*Corr(1,3,4,5,7,8,8,8)
  //     +  Corr(1,3,4,6,6,7,7,8)
  //     +  Corr(1,3,4,6,7,7,8,8)
  //     +2*Corr(1,3,5,6,6,6,7,8)
  //     +  Corr(1,3,5,6,6,7,8,8)
  //     +2*Corr(1,3,5,6,7,8,8,8)
  //     +  Corr(1,4,4,5,5,6,7,8)
  //     +  Corr(1,4,4,5,6,7,7,8)
  //     +  Corr(1,4,5,5,6,6,7,8)
  //     +  Corr(1,4,5,5,6,7,8,8)
  //     +  Corr(1,4,5,6,6,7,7,8)
  //     +  Corr(1,4,5,6,7,7,8,8)
  //     +  Corr(2,2,3,3,4,5,6,7)
  //     +  Corr(2,2,3,3,4,5,7,8)
  //     +  Corr(2,2,3,3,5,6,7,8)
  //     +  Corr(2,2,3,4,5,5,6,7)
  //     +  Corr(2,2,3,4,5,5,7,8)
  //     +  Corr(2,2,3,4,5,6,7,7)
  //     +  Corr(2,2,3,4,5,7,7,8)
  //     +  Corr(2,2,3,5,5,6,7,8)
  //     +  Corr(2,2,3,5,6,7,7,8)
  //     +2*Corr(2,3,3,3,4,5,6,8)
  //     +2*Corr(2,3,3,3,4,6,7,8)
  //     +  Corr(2,3,3,4,4,5,6,7)
  //     +  Corr(2,3,3,4,4,5,7,8)
  //     +  Corr(2,3,3,4,5,5,6,8)
  //     +  Corr(2,3,3,4,5,6,6,7)
  //     +  Corr(2,3,3,4,5,7,8,8)
  //     +  Corr(2,3,3,4,6,7,7,8)
  //     +  Corr(2,3,3,5,6,6,7,8)
  //     +  Corr(2,3,3,5,6,7,8,8)
  //     +  Corr(2,3,4,4,5,5,6,7)
  //     +  Corr(2,3,4,4,5,5,7,8)
  //     +  Corr(2,3,4,4,5,6,7,7)
  //     +  Corr(2,3,4,4,5,7,7,8)
  //     +2*Corr(2,3,4,5,5,5,6,8)
  //     +  Corr(2,3,4,5,5,6,6,7)
  //     +  Corr(2,3,4,5,5,7,8,8)
  //     +  Corr(2,3,4,5,6,6,7,7)
  //     +  Corr(2,3,4,5,7,7,8,8)
  //     +2*Corr(2,3,4,6,7,7,7,8)
  //     +  Corr(2,3,5,5,6,6,7,8)
  //     +  Corr(2,3,5,5,6,7,8,8)
  //     +  Corr(2,3,5,6,6,7,7,8)
  //     +  Corr(2,3,5,6,7,7,8,8)
  //     +2*Corr(2,4,5,5,5,6,7,8)
  //     +  Corr(2,4,5,5,6,7,7,8)
  //     +2*Corr(2,4,5,6,7,7,7,8)
  //     +  Corr(3,3,4,4,5,6,7,8)
  //     +  Corr(3,3,4,5,6,6,7,8)
  //     +  Corr(3,3,4,5,6,7,8,8)
  //     +  Corr(3,4,4,5,5,6,7,8)
  //     +  Corr(3,4,4,5,6,7,7,8)
  //     +  Corr(3,4,5,5,6,6,7,8)
  //     +  Corr(3,4,5,5,6,7,8,8)
  //     +  Corr(3,4,5,6,6,7,7,8)
  //     +  Corr(3,4,5,6,7,7,8,8)
  //   );

  // for (int i = 0; i < 3; ++i) {
  //   Profiler prof;
  //   const auto expr = Lido7(1,2,3,4,5,6,7,8);
  //   prof.finish("expr");
  //   std::cout << "Norm: " << expr.l1_norm() << "\n";
  //   std::cout << "Size: " << expr.size() << "\n";
  //   std::vector<size_t> hashes;
  //   expr.foreach_key([&](const Word& w, int coeff) {
  //     CHECK(coeff != 0);
  //     hashes.push_back(std::hash<Word>()(w));
  //   });
  //   std::cout << "Unique hashes: " << num_distinct_elements(hashes) << "\n";
  // }
  // return 0;

  std::set<std::string> existing_args;
  expr.annotations().foreach([&](const std::string& annotation, int) {
    existing_args.insert(annotation);
  });

  std::set<std::string> missing_args;
  for (const auto& w : nondecreasing_sequences(8, 8)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    int num_odd = 0;
    int num_even = 0;
    for (const int a : args) {
      if (a % 2 == 0) {
        num_even++;
      } else {
        num_odd++;
      }
    }

    const int num_distinct = num_distinct_elements(args);

    bool has_three_in_row = false;
    int last_arg = -1;
    int same_in_row = 0;
    for (const int a : args) {
      if (a == last_arg) {
        ++same_in_row;
        if (same_in_row == 3) {
          has_three_in_row = true;
        }
      } else {
        last_arg = a;
        same_in_row = 1;
      }
    }

    if (num_even == num_odd && num_distinct == 6) {
      const std::string annotation = "Corr(" + str_join(args, ",") + ")";
      if (existing_args.count(annotation) == 0) {
        missing_args.insert(annotation);
        // std::cout << (has_three_in_row ? "+2*" : "+  ") << annotation << "\n";
      }
    }
  }
  std::cout << "\n";

  // auto expr =
  //   - Lido7(1,2,3,4,5,6,7,8)
  //   + Corr(1,2,3,4,5,6,7,8)
  //   + CorrLoop(1,1,2,4,5,6,7,8)
  //   + CorrLoop(1,1,2,3,4,6,7,8)
  //   + CorrLoop(1,1,2,3,4,5,6,8)
  // ;

  // auto expr =
  //   +6 * Lido5(1,2,3,4)
  //   +3 * sum_looped(&CorrVec, 4, {1,1,2,2,3,4}, Sign::plus)
  //   +2 * sum_looped(&CorrVec, 4, {1,2,2,2,3,3}, Sign::plus)
  //   +2 * sum_looped(&CorrVec, 4, {1,1,2,2,2,3}, Sign::plus)
  // ;   // Total zero

  // auto expr =
  //   +2 * Lido4(1,2,3,4)
  //   +1 * sum_looped(&CorrVec, 4, {1,2,3,4,4}, Sign::alternating)
  //   +2 * sum_looped(&CorrVec, 4, {1,2,2,2,3}, Sign::alternating)
  // ;   // Total zero

  // auto expr =
  //   + Lido4(1,2,3,4)
  //   + Corr(1,2,3,4,4)
  //   + Corr(1,2,2,3,4)
  // ;

  profiler.finish("expr");

  // auto lyndon = to_lyndon_basis(expr);
  // auto projected = project_on_x1(lyndon);
  auto projected = to_lyndon_basis(project_on_x1(expr));
  profiler.finish("lyndon");
  std::cout << "\n";

  // auto src = lyndon;
  auto src = projected;
  // std::cout << "After Lyndon" << lyndon << "\n";
  std::cout << "Projected " << projected << "\n";
  // auto filtered = terms_containing_only_variables(lyndon, {1,2,3});
  // std::cout << terms_with_min_distinct_elements(lyndon, 3) << "\n";
  // std::cout << filtered << "\n";
  // for (int i = 3; i <= 6; ++i) {
  //   std::cout << i << " vars " << terms_containing_num_variables(src, i).without_annotations() << "\n";
  // }
  // for (int i = 3; i <= 8; ++i) {
  //   std::cout << i << " vars " << terms_with_exact_distinct_elements(src, i).without_annotations() << "\n";
  // }
  // std::cout << contains_only_variables(src, {1,2,3,4,5}) << "\n";
  // std::cout << keep_connected_graphs(src) << "\n";

  // std::cout << "Missing: \n";
  // for (const std::string& s : missing_args) {
  //   std::cout << " + " << s << "\n";
  // }
  // std::cout << "\n";
}
