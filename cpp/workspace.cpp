#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"

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

std::vector<int> odd_elements(std::vector<int> v) {
  return filtered(std::move(v), [](int x) { return x % 2 == 1; });
}
std::vector<int> even_elements(std::vector<int> v) {
  return filtered(std::move(v), [](int x) { return x % 2 == 0; });
}

DeltaExpr CorrBundle(int num_args, int num_vars, int num_distinct_vars, std::pair<int, int> coeffs = {1, 1}) {
  CHECK_LE(num_distinct_vars, num_args);
  CHECK_LE(num_distinct_vars, num_vars);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto args_odd = odd_elements(args);
    const auto args_even = even_elements(args);
    const int num_odd = args_odd.size();
    const int num_even = args_even.size();
    const int num_distinct = num_distinct_elements(args);
    CHECK(num_vars % 2 == 0);
    const bool has_all_odd  = num_distinct_elements(args_odd)  == num_vars / 2;
    const bool has_all_even = num_distinct_elements(args_even) == num_vars / 2;
    if (num_odd == num_even && num_distinct == num_distinct_vars) {
      const int coeff = has_all_odd || has_all_even ? coeffs.first : coeffs.second;
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

DeltaExpr CorrBundleAlt(int num_args, int num_vars, int num_distinct_vars) {
  CHECK_LE(num_distinct_vars, num_args);
  CHECK_LE(num_distinct_vars, num_vars);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto args_odd = odd_elements(args);
    const auto args_even = even_elements(args);
    const int num_odd = args_odd.size();
    const int num_even = args_even.size();
    const int num_distinct = num_distinct_elements(args);
    if (num_distinct == num_distinct_vars) {
      const int coeff = (num_odd == num_even) ? 1 : -1;
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

DeltaExpr CorrBundleEven(int num_args, int num_vars, int num_distinct_vars, std::tuple<int, int, int, int> coeffs = {1, -1, 1, -1}) {
  CHECK_LE(num_distinct_vars, num_args);
  CHECK_LE(num_distinct_vars, num_vars);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto args_odd = odd_elements(args);
    const auto args_even = even_elements(args);
    const int num_odd = args_odd.size();
    const int num_even = args_even.size();
    const int num_distinct = num_distinct_elements(args);
    CHECK(num_vars % 2 == 0);
    const bool has_all_odd  = num_distinct_elements(args_odd)  == num_vars / 2;
    const bool has_all_even = num_distinct_elements(args_even) == num_vars / 2;
    if (num_distinct == num_distinct_vars && std::abs(num_odd - num_even) == 1) {
      const int coeff =
        (has_all_odd || has_all_even)
        ? ((num_odd > num_even) ? std::get<0>(coeffs) : std::get<1>(coeffs))
        : ((num_odd > num_even) ? std::get<2>(coeffs) : std::get<3>(coeffs));
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

DeltaExpr DreamCorrBundle(int num_args, int num_vars) {
  DeltaExpr ret;
  const int total_odd_vars = (num_vars + 1) / 2;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + num_args);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

bool contains_var_from_each_pair(const std::vector<int>& args, int num_vars) {
  absl::flat_hash_set<int> args_set(args.begin(), args.end());
  CHECK(num_vars % 2 == 0);
  for (int var = 1; var <= num_vars; var += 2) {
    if (!(args_set.contains(var) || args_set.contains(var+1))) {
      return false;
    }
  }
  return true;
}

DeltaExpr SupremeCorrBundle(int num_args, int num_vars) {
  DeltaExpr ret;
  const int total_odd_vars = (num_vars + 1) / 2;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair(args, num_vars)) {
      continue;
    }
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + num_args);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

// Doesn't work :(
// DeltaExpr LidoSymm4_10(int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, int x10) {
//   return
//     + 2 * Lido4(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)
//     + (
//       - LidoSymm4(x1,x2,x3,x4,x5,x6,x7,x8)
//       - LidoSymm4(x3,x4,x5,x6,x7,x8,x9,x10)
//       - LidoSymm4(x5,x6,x7,x8,x9,x10,x1,x2)
//       - LidoSymm4(x7,x8,x9,x10,x1,x2,x3,x4)
//       - LidoSymm4(x9,x10,x1,x2,x3,x4,x5,x6)
//       - Lido4(x1,x2,x3,x4,x5,x6,x7,x8)
//       - Lido4(x3,x4,x5,x6,x7,x8,x9,x10)
//       - Lido4(x5,x6,x7,x8,x9,x10,x1,x2)
//       - Lido4(x7,x8,x9,x10,x1,x2,x3,x4)
//       - Lido4(x9,x10,x1,x2,x3,x4,x5,x6)
//     )
//   ;
// }

int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;

  // for (int weight = 1; weight <= 6; weight++) {
  //   for (int num_args = 4; num_args <= 12; num_args += 2) {
  //     auto large_args = seq_incl(1, num_args);
  //     --large_args.back();
  //     auto lhs = word_expr_substitute(
  //       project_on_x1(DreamCorrBundle(weight + 1, num_args)),
  //       large_args
  //     );
  //     auto rhs = project_on_x1(DreamCorrBundle(weight + 1, num_args - 2));
  //     std::cout << "weight = " << weight << ", num_args = " << num_args << ": ";
  //     std::cout << (lhs + rhs).without_annotations();
  //   }
  // }

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
  // auto corhs =
  //   + coproduct(Lido3(1,2,3,4), Lido2(1,4,5,6))
  //   + coproduct(Lido3(2,3,4,5), Lido2(2,5,6,1))
  //   + coproduct(Lido3(3,4,5,6), Lido2(3,6,1,2))
  //   + coproduct(Lido3(4,5,6,1), Lido2(4,1,2,3))
  //   + coproduct(Lido3(5,6,1,2), Lido2(5,2,3,4))
  //   + coproduct(Lido3(6,1,2,3), Lido2(6,3,4,5))
  // ;
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
  // profiler.finish("lhs");
  // auto colhs = comultiply(lhs, {2,3});
  // profiler.finish("colhs");
  // auto corhs =
  //   + coproduct(Lido2(1,2,3,4), LidoSymm3(1,4,5,6,7,8))
  //   + coproduct(Lido2(2,3,4,5), LidoSymm3(2,5,6,7,8,1))
  //   + coproduct(Lido2(3,4,5,6), LidoSymm3(3,6,7,8,1,2))
  //   + coproduct(Lido2(4,5,6,7), LidoSymm3(4,7,8,1,2,3))
  //   + coproduct(Lido2(5,6,7,8), LidoSymm3(5,8,1,2,3,4))
  //   + coproduct(Lido2(6,7,8,1), LidoSymm3(6,1,2,3,4,5))
  //   + coproduct(Lido2(7,8,1,2), LidoSymm3(7,2,3,4,5,6))
  //   + coproduct(Lido2(8,1,2,3), LidoSymm3(8,3,4,5,6,7))
  //   + coproduct(Lido3(1,2,3,4), LidoSymm2(1,4,5,6,7,8))
  //   + coproduct(Lido3(2,3,4,5), LidoSymm2(2,5,6,7,8,1))
  //   + coproduct(Lido3(3,4,5,6), LidoSymm2(3,6,7,8,1,2))
  //   + coproduct(Lido3(4,5,6,7), LidoSymm2(4,7,8,1,2,3))
  //   + coproduct(Lido3(5,6,7,8), LidoSymm2(5,8,1,2,3,4))
  //   + coproduct(Lido3(6,7,8,1), LidoSymm2(6,1,2,3,4,5))
  //   + coproduct(Lido3(7,8,1,2), LidoSymm2(7,2,3,4,5,6))
  //   + coproduct(Lido3(8,1,2,3), LidoSymm2(8,3,4,5,6,7))
  // ;
  // profiler.finish("corhs");
  // auto diff = colhs - corhs;
  // profiler.finish("diff");

  // // StringExpr forms;  // sanity check
  // // diff.foreach_key([&](MultiWord w, int) {
  // //   CHECK_EQ(w.num_segments(), 2);
  // //   forms += StringExpr::single(absl::StrCat("<", w.segment(0).size(), ", ", w.segment(1).size(), ">"));
  // // });

  // std::cout << "\n";
  // std::cout << diff << "\n";





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

  // auto expr =
  //   + sum_looped(&CorrVec, 4, {1,1,1,2,3,4}, Sign::plus)
  //   + sum_looped(&CorrVec, 4, {1,1,2,2,3,4}, Sign::plus)
  //   + Corr(1,1,2,3,3,4)
  //   + Corr(2,2,3,4,4,1)
  // ;  // Total zero


  // auto expr =
  //   +2*Lido5(1,2,3,4)
  //   +  Corr(1,1,2,2,3,4)
  //   +  Corr(1,1,2,3,4,4)
  //   +  Corr(1,2,2,3,3,4)
  //   +  Corr(1,2,3,3,4,4)
  // ;   // only 2 vars in projection on x1


  // auto expr =
  //   - Lido3(1,2,3,4)
  //   + Corr(1,2,3,4)
  //   + Corr(1,2,2,3)
  //   + Corr(2,3,3,4)
  //   + Corr(3,4,4,1)
  //   + Corr(4,1,1,2)
  // ;  // Total zero


  // auto expr =
  //   +6*Lido5(1,2,3,4)
  //   +3*Corr(1,1,2,2,3,4)
  //   +3*Corr(1,1,2,3,4,4)
  //   +3*Corr(1,2,2,3,3,4)
  //   +3*Corr(1,2,3,3,4,4)
  //   +2*Corr(1,1,1,2,2,4)
  //   +2*Corr(1,1,1,2,4,4)
  //   +2*Corr(1,1,2,2,2,3)
  //   +2*Corr(1,1,3,4,4,4)
  //   +2*Corr(1,2,2,2,3,3)
  //   +2*Corr(1,3,3,4,4,4)
  //   +2*Corr(2,2,3,3,3,4)
  //   +2*Corr(2,3,3,3,4,4)
  // ;  // Total zero


  // auto expr =
  //   -30*Lido7(1,2,3,4)
  //   +5*Corr(1,1,1,2,2,3,4,4)
  //   +5*Corr(1,1,1,2,2,2,3,4)
  //   +5*Corr(1,1,1,2,3,4,4,4)
  //   +5*Corr(1,1,2,2,2,3,3,4)
  //   +5*Corr(1,1,2,3,3,4,4,4)
  //   +5*Corr(1,1,2,2,3,3,4,4)
  //   +5*Corr(1,2,2,2,3,3,3,4)
  //   +5*Corr(1,2,2,3,3,3,4,4)
  //   +5*Corr(1,2,3,3,3,4,4,4)
  //   +3*Corr(1,1,1,1,2,2,2,4)
  //   +3*Corr(1,1,1,1,2,2,4,4)
  //   +3*Corr(1,1,1,1,2,4,4,4)
  //   +3*Corr(1,1,1,2,2,2,2,3)
  //   +3*Corr(1,1,1,3,4,4,4,4)
  //   +3*Corr(1,1,2,2,2,2,3,3)
  //   +3*Corr(1,1,3,3,4,4,4,4)
  //   +3*Corr(1,2,2,2,2,3,3,3)
  //   +3*Corr(1,3,3,3,4,4,4,4)
  //   +3*Corr(2,2,2,3,3,3,3,4)
  //   +3*Corr(2,2,3,3,3,3,4,4)
  //   +3*Corr(2,3,3,3,3,4,4,4)
  // ;  // Total zero


  // auto expr =
  //   - Lido5(1,2,3,4)
  //   + Corr(1,3,4,4,4,4)
  //   + Corr(1,2,3,4,4,4)
  //   + Corr(1,2,2,3,4,4)
  //   + Corr(1,2,2,2,3,4)
  //   + Corr(1,2,2,2,2,3)
  // ;  // Kills everything that contains 1 and 3 simultaneously


  // auto expr =
  //   - Lido5(1,2,3,4)
  //   + Corr(1,3,4,4,4,4)
  //   + Corr(1,2,3,4,4,4)
  //   + Corr(1,2,2,3,4,4)
  //   + Corr(1,2,2,2,3,4)
  //   + Corr(1,2,2,2,2,3)
  //   - Corr(1,2,4,4,4,4)
  //   - Corr(1,2,2,4,4,4)
  //   - Corr(1,2,2,2,4,4)
  //   - Corr(1,2,2,2,2,4)
  //   - Corr(2,3,4,4,4,4)
  //   - Corr(2,2,3,4,4,4)
  //   - Corr(2,2,2,3,4,4)
  //   - Corr(2,2,2,2,3,4)
  // ;  // Total zero


/*
  auto expr =
    - LidoSymm5(1,2,3,4,5,6)

    + DreamCorrBundle(6, 6)

    - Corr(1,2,2,2,2,3)
    - Corr(1,2,2,2,3,4)
    - Corr(1,2,2,2,3,6)
    - Corr(1,2,2,3,4,4)
    - Corr(1,2,2,3,4,6)
    - Corr(1,2,2,3,6,6)
    - Corr(1,2,3,4,4,4)
    - Corr(1,2,3,4,4,6)
    - Corr(1,2,3,4,6,6)
    - Corr(1,2,3,6,6,6)
    - Corr(1,3,4,4,4,4)
    - Corr(1,3,4,4,4,6)
    - Corr(1,3,4,4,6,6)
    - Corr(1,3,4,6,6,6)
    - Corr(1,3,6,6,6,6)

    - Corr(1,2,2,2,2,5)
    - Corr(1,2,2,2,4,5)
    - Corr(1,2,2,4,4,5)
    - Corr(1,2,4,4,4,5)
    - Corr(1,4,4,4,4,5)
    - Corr(1,2,2,2,5,6)
    - Corr(1,2,2,4,5,6)
    - Corr(1,2,4,4,5,6)
    - Corr(1,4,4,4,5,6)
    - Corr(1,2,2,5,6,6)
    - Corr(1,2,4,5,6,6)
    - Corr(1,4,4,5,6,6)
    - Corr(1,2,5,6,6,6)
    - Corr(1,4,5,6,6,6)
    - Corr(1,5,6,6,6,6)

    + Corr(1,2,2,2,4,6)
    + Corr(1,2,2,4,4,6)
    + Corr(1,2,2,4,6,6)
    + Corr(1,2,4,4,4,6)
    + Corr(1,2,4,4,6,6)
    + Corr(1,2,4,6,6,6)

    + Corr(1,2,2,2,2,4)
    + Corr(1,2,2,2,2,6)
    + Corr(1,2,2,2,4,4)
    + Corr(1,2,2,2,6,6)
    + Corr(1,2,2,4,4,4)
    + Corr(1,2,2,6,6,6)
    + Corr(1,2,4,4,4,4)
    + Corr(1,2,6,6,6,6)
    + Corr(1,4,4,4,4,6)
    + Corr(1,4,4,4,6,6)
    + Corr(1,4,4,6,6,6)
    + Corr(1,4,6,6,6,6)
  ;
*/



  auto expr =
    - Lido3(1,2,3,4)
    + SupremeCorrBundle(4, 4)
  ;

  // auto expr =
  //   - Lido5(1,2,3,4,5,6)
  //   + SupremeCorrBundle(6, 6)
  // ;

  // auto expr =
  //   - Lido6(1,2,3,4,5,6)
  //   + SupremeCorrBundle(7, 6)
  // ;

  // auto expr =
  //   - Lido5(1,2,3,4,5,6,7,8)
  //   + SupremeCorrBundle(6, 8)
  // ;

  // auto expr =
  //   - Lido5(1,2,3,4,5,6,7,8,9,10)
  //   + SupremeCorrBundle(6, 10)
  // ;



  // auto expr =
  //   - Lido1(1,2,3,4)
  //   + DreamCorrBundle(2, 4)
  // ;  // NOT zero

  // auto expr =
  //   - Lido3(1,2,3,4)
  //   + DreamCorrBundle(4, 4)
  // ;

  // auto expr =
  //   - Lido4(1,2,3,4)
  //   + DreamCorrBundle(5, 4)
  // ;

  // auto expr =
  //   - LidoSymm2(1,2,3,4,5,6)
  //   + DreamCorrBundle(3, 6)
  // ;

  // auto expr =
  //   - LidoSymm3(1,2,3,4,5,6)
  //   + DreamCorrBundle(4, 6)
  // ;

  // auto expr =
  //   - LidoSymm4(1,2,3,4,5,6)
  //   + DreamCorrBundle(5, 6)
  // ;

  // auto expr =
  //   - LidoSymm5(1,2,3,4,5,6)
  //   + DreamCorrBundle(6, 6)
  // ;

  // auto expr =
  //   - LidoSymm7(1,2,3,4,5,6)
  //   + DreamCorrBundle(8, 6)
  // ;

  // auto expr =
  //   - LidoSymm5(1,2,3,4,5,6,7,8)
  //   + DreamCorrBundle(6, 8)
  // ;

  // auto expr =
  //   - LidoSymm6(1,2,3,4,5,6,7,8)
  //   + DreamCorrBundle(7, 8)
  // ;

  // auto expr =
  //   - LidoSymm7(1,2,3,4,5,6,7,8)
  //   + DreamCorrBundle(8, 8)
  // ;


  // auto expr =
  //   + Lido4(1,2,3,4,5,6)
  //   - I(1,2,3,4,5,6)
  //   - I(3,4,5,6,1,2)
  //   - I(5,6,1,2,3,4)
  //   + I(2,2,2,3,5,1)
  //   + I(4,4,4,5,1,3)
  //   + I(6,6,6,1,3,5)
  //   - I(1,1,2,4,4,5)
  //   - I(3,3,4,6,6,1)
  //   - I(5,5,6,2,2,3)
  //   + I(1,2,2,4,5,5)
  //   + I(3,4,4,6,1,1)
  //   + I(5,6,6,2,3,3)
  // ;  // Kills 4 vars (after projecting on x1) and half of 3 vars


  // auto expr =
  //   +6*LidoSymm4(1,2,3,4,5,6)
  //   +6*CorrBundleEven(5, 6, 5)
  //   +3*CorrBundleEven(5, 6, 4, {2,-2,1,-1})
  //   +2*CorrBundleEven(5, 6, 3)
  // ;  // Total zero


  // auto expr =
  //   -6*LidoSymm5(1,2,3,4,5,6)
  //   +6*CorrBundle(6, 6, 6)
  //   +6*CorrBundle(6, 6, 5)
  //   +3*CorrBundle(6, 6, 4, {2,1})
  //   +2*CorrBundle(6, 6, 3)
  // ;  // Total zero


  // auto expr =
  //   +60*LidoSymm7(1,2,3,4,5,6)
  //   +30*CorrBundle(8, 6, 6)
  //   +20*CorrBundle(8, 6, 5)
  //   +5 *CorrBundle(8, 6, 4, {3,2})
  //   +6 *CorrBundle(8, 6, 3)
  // ;


  // auto expr =
  //   + LidoSymm3(1,2,3,4,5,6)
  //   + CorrBundleAlt(4, 6, 4)
  //   + CorrBundle(4, 6, 3)
  // ;


  // auto expr =
  //   +2*LidoSymm5(1,2,3,4,5,6,7,8)
  //   +2*CorrBundleAlt(6, 8, 6)
  //   +2*CorrBundleAlt(6, 8, 5)
  // ;  // <= 4 vars when projected on x1


  // auto expr =
  //   -2*LidoSymm7(1,2,3,4,5,6,7,8)
  //   +2*Corr(1,2,3,4,5,6,7,8)
  //   +2*CorrBundle(8, 8, 7)
  //   +  CorrBundle(8, 8, 6, {2,1})
  // ;

  // auto expr =
  //   -2*LidoSymmVecPr(7, {1,2,3,4,5,6,7,8}, project_on_x1)
  //   + project_on_x1(
  //     +2*Corr(1,2,3,4,5,6,7,8)
  //     +2*CorrBundle(8, 8, 7)
  //     +  CorrBundle(8, 8, 6, {2,1})
  //   )
  // ;

/*
  auto Corr = [](auto... args) {
    return CorrVecPr({args...}, project_on_x1);
  };

  auto expr =
    // -2*LidoSymmVecPr(7, {1,2,3,4,5,6,7,8}, project_on_x1)
    -2*LidoVecPr(7, {1,2,3,4,5,6,7,8}, project_on_x1)
    // -2*LidoSymmPr7(1,2,3,4,5,6,7,8)

    +2*Corr(1,2,3,4,5,6,7,8)

    +2*Corr(1,1,2,3,4,5,6,8)
    +2*Corr(1,1,2,3,4,6,7,8)
    +2*Corr(1,1,2,4,5,6,7,8)
    +2*Corr(1,2,2,3,4,5,6,7)
    +2*Corr(1,2,2,3,4,5,7,8)
    +2*Corr(1,2,2,3,5,6,7,8)
    +2*Corr(1,2,3,3,4,5,6,8)
    +2*Corr(1,2,3,3,4,6,7,8)
    +2*Corr(1,2,3,4,4,5,6,7)
    +2*Corr(1,2,3,4,4,5,7,8)
    +2*Corr(1,2,3,4,5,5,6,8)
    +2*Corr(1,2,3,4,5,6,6,7)
    +2*Corr(1,2,3,4,5,7,8,8)
    +2*Corr(1,2,3,4,6,7,7,8)
    +2*Corr(1,2,3,5,6,6,7,8)
    +2*Corr(1,2,3,5,6,7,8,8)
    +2*Corr(1,2,4,5,5,6,7,8)
    +2*Corr(1,2,4,5,6,7,7,8)
    +2*Corr(1,3,4,4,5,6,7,8)
    +2*Corr(1,3,4,5,6,6,7,8)
    +2*Corr(1,3,4,5,6,7,8,8)
    +2*Corr(2,3,3,4,5,6,7,8)
    +2*Corr(2,3,4,5,5,6,7,8)
    +2*Corr(2,3,4,5,6,7,7,8)

    +2*Corr(1,1,1,2,3,4,6,8)
    +2*Corr(1,1,1,2,4,5,6,8)
    +2*Corr(1,1,1,2,4,6,7,8)
    +  Corr(1,1,2,2,3,4,5,6)
    +  Corr(1,1,2,2,3,4,5,8)
    +  Corr(1,1,2,2,3,4,6,7)
    +  Corr(1,1,2,2,3,4,7,8)
    +  Corr(1,1,2,2,3,5,6,8)
    +  Corr(1,1,2,2,3,6,7,8)
    +  Corr(1,1,2,2,4,5,6,7)
    +  Corr(1,1,2,2,4,5,7,8)
    +  Corr(1,1,2,2,5,6,7,8)
    +2*Corr(1,1,2,3,3,4,6,8)
    +  Corr(1,1,2,3,4,4,5,6)
    +  Corr(1,1,2,3,4,4,5,8)
    +  Corr(1,1,2,3,4,4,6,7)
    +  Corr(1,1,2,3,4,4,7,8)
    +  Corr(1,1,2,3,4,5,6,6)
    +  Corr(1,1,2,3,4,5,8,8)
    +  Corr(1,1,2,3,4,6,6,7)
    +  Corr(1,1,2,3,4,7,8,8)
    +  Corr(1,1,2,3,5,6,6,8)
    +  Corr(1,1,2,3,5,6,8,8)
    +  Corr(1,1,2,3,6,6,7,8)
    +  Corr(1,1,2,3,6,7,8,8)
    +  Corr(1,1,2,4,4,5,6,7)
    +  Corr(1,1,2,4,4,5,7,8)
    +2*Corr(1,1,2,4,5,5,6,8)
    +  Corr(1,1,2,4,5,6,6,7)
    +  Corr(1,1,2,4,5,7,8,8)
    +2*Corr(1,1,2,4,6,7,7,8)
    +  Corr(1,1,2,5,6,6,7,8)
    +  Corr(1,1,2,5,6,7,8,8)
    +  Corr(1,1,3,4,4,5,6,8)
    +  Corr(1,1,3,4,4,6,7,8)
    +  Corr(1,1,3,4,5,6,6,8)
    +  Corr(1,1,3,4,5,6,8,8)
    +  Corr(1,1,3,4,6,6,7,8)
    +  Corr(1,1,3,4,6,7,8,8)
    +  Corr(1,1,4,4,5,6,7,8)
    +  Corr(1,1,4,5,6,6,7,8)
    +  Corr(1,1,4,5,6,7,8,8)
    +2*Corr(1,2,2,2,3,4,5,7)
    +2*Corr(1,2,2,2,3,5,6,7)
    +2*Corr(1,2,2,2,3,5,7,8)
    +  Corr(1,2,2,3,3,4,5,6)
    +  Corr(1,2,2,3,3,4,5,8)
    +  Corr(1,2,2,3,3,4,6,7)
    +  Corr(1,2,2,3,3,4,7,8)
    +  Corr(1,2,2,3,3,5,6,8)
    +  Corr(1,2,2,3,3,6,7,8)
    +2*Corr(1,2,2,3,4,4,5,7)
    +  Corr(1,2,2,3,4,5,5,6)
    +  Corr(1,2,2,3,4,5,5,8)
    +  Corr(1,2,2,3,4,6,7,7)
    +  Corr(1,2,2,3,4,7,7,8)
    +  Corr(1,2,2,3,5,5,6,8)
    +2*Corr(1,2,2,3,5,6,6,7)
    +2*Corr(1,2,2,3,5,7,8,8)
    +  Corr(1,2,2,3,6,7,7,8)
    +  Corr(1,2,2,4,5,5,6,7)
    +  Corr(1,2,2,4,5,5,7,8)
    +  Corr(1,2,2,4,5,6,7,7)
    +  Corr(1,2,2,4,5,7,7,8)
    +  Corr(1,2,2,5,5,6,7,8)
    +  Corr(1,2,2,5,6,7,7,8)
    +2*Corr(1,2,3,3,3,4,6,8)
    +  Corr(1,2,3,3,4,4,5,6)
    +  Corr(1,2,3,3,4,4,5,8)
    +  Corr(1,2,3,3,4,4,6,7)
    +  Corr(1,2,3,3,4,4,7,8)
    +  Corr(1,2,3,3,4,5,6,6)
    +  Corr(1,2,3,3,4,5,8,8)
    +  Corr(1,2,3,3,4,6,6,7)
    +  Corr(1,2,3,3,4,7,8,8)
    +  Corr(1,2,3,3,5,6,6,8)
    +  Corr(1,2,3,3,5,6,8,8)
    +  Corr(1,2,3,3,6,6,7,8)
    +  Corr(1,2,3,3,6,7,8,8)
    +2*Corr(1,2,3,4,4,4,5,7)
    +  Corr(1,2,3,4,4,5,5,6)
    +  Corr(1,2,3,4,4,5,5,8)
    +  Corr(1,2,3,4,4,6,7,7)
    +  Corr(1,2,3,4,4,7,7,8)
    +  Corr(1,2,3,4,5,5,6,6)
    +  Corr(1,2,3,4,5,5,8,8)
    +  Corr(1,2,3,4,6,6,7,7)
    +  Corr(1,2,3,4,7,7,8,8)
    +  Corr(1,2,3,5,5,6,6,8)
    +  Corr(1,2,3,5,5,6,8,8)
    +2*Corr(1,2,3,5,6,6,6,7)
    +2*Corr(1,2,3,5,7,8,8,8)
    +  Corr(1,2,3,6,6,7,7,8)
    +  Corr(1,2,3,6,7,7,8,8)
    +  Corr(1,2,4,4,5,5,6,7)
    +  Corr(1,2,4,4,5,5,7,8)
    +  Corr(1,2,4,4,5,6,7,7)
    +  Corr(1,2,4,4,5,7,7,8)
    +2*Corr(1,2,4,5,5,5,6,8)
    +  Corr(1,2,4,5,5,6,6,7)
    +  Corr(1,2,4,5,5,7,8,8)
    +  Corr(1,2,4,5,6,6,7,7)
    +  Corr(1,2,4,5,7,7,8,8)
    +2*Corr(1,2,4,6,7,7,7,8)
    +  Corr(1,2,5,5,6,6,7,8)
    +  Corr(1,2,5,5,6,7,8,8)
    +  Corr(1,2,5,6,6,7,7,8)
    +  Corr(1,2,5,6,7,7,8,8)
    +  Corr(1,3,3,4,4,5,6,8)
    +  Corr(1,3,3,4,4,6,7,8)
    +  Corr(1,3,3,4,5,6,6,8)
    +  Corr(1,3,3,4,5,6,8,8)
    +  Corr(1,3,3,4,6,6,7,8)
    +  Corr(1,3,3,4,6,7,8,8)
    +2*Corr(1,3,4,4,4,5,6,7)
    +2*Corr(1,3,4,4,4,5,7,8)
    +  Corr(1,3,4,4,5,5,6,8)
    +2*Corr(1,3,4,4,5,6,6,7)
    +2*Corr(1,3,4,4,5,7,8,8)
    +  Corr(1,3,4,4,6,7,7,8)
    +  Corr(1,3,4,5,5,6,6,8)
    +  Corr(1,3,4,5,5,6,8,8)
    +2*Corr(1,3,4,5,6,6,6,7)
    +2*Corr(1,3,4,5,7,8,8,8)
    +  Corr(1,3,4,6,6,7,7,8)
    +  Corr(1,3,4,6,7,7,8,8)
    +2*Corr(1,3,5,6,6,6,7,8)
    +2*Corr(1,3,5,6,6,7,8,8)
    +2*Corr(1,3,5,6,7,8,8,8)
    +  Corr(1,4,4,5,5,6,7,8)
    +  Corr(1,4,4,5,6,7,7,8)
    +  Corr(1,4,5,5,6,6,7,8)
    +  Corr(1,4,5,5,6,7,8,8)
    +  Corr(1,4,5,6,6,7,7,8)
    +  Corr(1,4,5,6,7,7,8,8)
    +  Corr(2,2,3,3,4,5,6,7)
    +  Corr(2,2,3,3,4,5,7,8)
    +  Corr(2,2,3,3,5,6,7,8)
    +  Corr(2,2,3,4,5,5,6,7)
    +  Corr(2,2,3,4,5,5,7,8)
    +  Corr(2,2,3,4,5,6,7,7)
    +  Corr(2,2,3,4,5,7,7,8)
    +  Corr(2,2,3,5,5,6,7,8)
    +  Corr(2,2,3,5,6,7,7,8)
    +2*Corr(2,3,3,3,4,5,6,8)
    +2*Corr(2,3,3,3,4,6,7,8)
    +  Corr(2,3,3,4,4,5,6,7)
    +  Corr(2,3,3,4,4,5,7,8)
    +2*Corr(2,3,3,4,5,5,6,8)
    +  Corr(2,3,3,4,5,6,6,7)
    +  Corr(2,3,3,4,5,7,8,8)
    +2*Corr(2,3,3,4,6,7,7,8)
    +  Corr(2,3,3,5,6,6,7,8)
    +  Corr(2,3,3,5,6,7,8,8)
    +  Corr(2,3,4,4,5,5,6,7)
    +  Corr(2,3,4,4,5,5,7,8)
    +  Corr(2,3,4,4,5,6,7,7)
    +  Corr(2,3,4,4,5,7,7,8)
    +2*Corr(2,3,4,5,5,5,6,8)
    +  Corr(2,3,4,5,5,6,6,7)
    +  Corr(2,3,4,5,5,7,8,8)
    +  Corr(2,3,4,5,6,6,7,7)
    +  Corr(2,3,4,5,7,7,8,8)
    +2*Corr(2,3,4,6,7,7,7,8)
    +  Corr(2,3,5,5,6,6,7,8)
    +  Corr(2,3,5,5,6,7,8,8)
    +  Corr(2,3,5,6,6,7,7,8)
    +  Corr(2,3,5,6,7,7,8,8)
    +2*Corr(2,4,5,5,5,6,7,8)
    +2*Corr(2,4,5,5,6,7,7,8)
    +2*Corr(2,4,5,6,7,7,7,8)
    +  Corr(3,3,4,4,5,6,7,8)
    +  Corr(3,3,4,5,6,6,7,8)
    +  Corr(3,3,4,5,6,7,8,8)
    +  Corr(3,4,4,5,5,6,7,8)
    +  Corr(3,4,4,5,6,7,7,8)
    +  Corr(3,4,5,5,6,6,7,8)
    +  Corr(3,4,5,5,6,7,8,8)
    +  Corr(3,4,5,6,6,7,7,8)
    +  Corr(3,4,5,6,7,7,8,8)
  ;
*/

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
  // auto expr = DeltaExpr{};



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
  // for (int i = 2; i <= 8; ++i) {
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
