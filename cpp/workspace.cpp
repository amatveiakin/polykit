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
  // TODO: move; fix
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;

  // auto expr =
  //   + CoLi(1,1)({1},{2})
  //   + CoLi(1,1)({2},{1})
  //   + CoLi(2)  ({1,2})
  //   - mystic_product(
  //     CoLi(1)({1}),
  //     CoLi(1)({2})
  //   );

  // auto expr = CoLi(1,1)({1},{2});

  // TODO: Add to tests
  // auto cr_expr =
  //   + epsilon_coexpr_to_theta_coexpr(CoLi(1,1)({1},{2}), {
  //     {CR(1,2,3,6)},
  //     {CR(3,4,5,6)},
  //   })
  //   - epsilon_coexpr_to_theta_coexpr(CoLi(1,1)({1},{2}), {
  //     {CR(1,2,5,6)},
  //     {CR(3,4,5,2)},
  //   })
  //   + epsilon_coexpr_to_theta_coexpr(CoLi(1,1)({1},{2}), {
  //     {CR(1,4,5,6)},
  //     {CR(1,2,3,4)},
  //   });

  // TODO: Add to tests
  // auto cr_expr =
  //   + epsilon_coexpr_to_theta_coexpr(CoLi(1,1)({1},{2}), {
  //     {CR(3,4,7,2)},
  //     {CR(5,6,7,4)},
  //   })
  //   - epsilon_coexpr_to_theta_coexpr(CoLi(1,1)({1},{2}), {
  //     {CR(3,6,7,2)},
  //     {CR(3,4,5,6)},
  //   })
  //   + epsilon_coexpr_to_theta_coexpr(CoLi(1,1)({1},{2}), {
  //     {CR(5,6,7,2)},
  //     {CR(3,4,5,2)},
  //   })
  //   + epsilon_coexpr_to_theta_coexpr(CoLi(2)({1,2}), {
  //     {CR(5,6,7,2)},
  //     {CR(3,4,5,2)},
  //   });

  // const int fw = 4;
  // auto cr_expr =
  //   + epsilon_expr_to_theta_expr(LiVec(fw, {1,1}, {{1},{2}}), {
  //     {CR(3,4,7,2)},
  //     {CR(5,6,7,4)},
  //   })
  //   - epsilon_expr_to_theta_expr(LiVec(fw, {1,1}, {{1},{2}}), {
  //     {CR(3,6,7,2)},
  //     {CR(3,4,5,6)},
  //   })
  //   + epsilon_expr_to_theta_expr(LiVec(fw, {1,1}, {{1},{2}}), {
  //     {CR(5,6,7,2)},
  //     {CR(3,4,5,2)},
  //   })
  //   + epsilon_expr_to_theta_expr(LiVec(fw, {2}, {{1,2}}), {
  //     {CR(5,6,7,2)},
  //     {CR(3,4,5,2)},
  //   });

  // auto cr_expr = epsilon_expr_to_theta_expr(Li(1)({1}), {
  //   {CR(1,2,3,4)},
  // });

  auto cr_expr = LiQuad(2, {1,2,3,4,5,6}, LiFirstPoint::odd);
  auto eval_expr = eval_formal_symbols(cr_expr);
  auto d_expr = theta_expr_to_delta_expr(eval_expr);
  auto legacy_expr = Lido4(1,2,3,4,5,6);
  auto diff = d_expr - legacy_expr;

  // auto expr = LiVec(2, {1}, {{1}});
  // auto cr_expr = epsilon_expr_to_theta_expr(expr, {
  //   {CR(1,2,3,4)},
  // });

  // auto cr_expr =
  //   + epsilon_coexpr_to_theta_coexpr(CoLi(1,1,1)({1},{2},{3}), {
  //     {CR(1,2,3,8)},
  //     {CR(3,4,5,8)},
  //     {CR(5,6,7,8)},
  //   });

  // auto expr = CoLi(1,5)({1},{2});
  // // auto expr = CoLi(1,1,1,1,1)({1},{2},{3},{4},{5});
  // auto filtered = filter_coexpr_predicate(expr, 0, [](const EpsilonPack& pack) {
  //   return std::visit(overloaded{
  //     [](const std::vector<Epsilon>& product) {
  //       return false;
  //     },
  //     [](const LiParam& formal_symbol) {
  //       return formal_symbol.points().size() == 1 &&
  //         // formal_symbol.points().front().size() == 3 &&
  //         formal_symbol.weights().front() >= 5;
  //     },
  //   }, pack);
  // });

  // auto a = CoLi(1,2)({1},{2});
  // auto b = CoLi(2,1)({2},{1});
  // auto c = CoLi(3)  ({1,2});
  // auto d1 = CoLi(1)({1});
  // auto d2 = CoLi(2)({2});
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
  // std::cout << "Expr " << expr << "\n";
  std::cout << "Cross-ratio expr " << cr_expr << "\n";
  // std::cout << "Evaluated expr " << eval_expr << "\n";
  // std::cout << "Converted " << d_expr << "\n";
  // std::cout << "Legacy " << legacy_expr << "\n";
  std::cout << "Diff " << diff << "\n";
  // std::cout << "Filtered " << filtered << "\n";
  // auto filtered = filter_coexpr(expr, 0, LiParam({1},{{1,2}}));
  // std::cout << "Expr filtered " << filtered << "\n";
  // std::cout << "After Lyndon " << lyndon << "\n";
  // std::cout << "After Lyndon: " << lyndon.size() << " terms, |coeff| = " << lyndon.l1_norm() << "\n";
}
