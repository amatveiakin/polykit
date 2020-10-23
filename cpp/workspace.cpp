#include <iostream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/polylog.h"
#include "lib/profiler.h"
#include "lib/shuffle.h"


// using StringExpr = Linear<SimpleLinearParam<std::string>>;

// std::string shuffle_unrolled(int n, int m) {
//   constexpr int kThreshold = 100;
//   IntWord u, v;
//   for (int i = 0; i < n; ++i) {
//     u.push_back(i);
//   }
//   for (int i = 0; i < m; ++i) {
//     v.push_back(i + kThreshold);
//   }
//   auto prod = shuffle_product(u, v);
//   auto unrolled_expr = prod.mapped<StringExpr>([&](const IntWord& w) {
//     std::vector<std::string> terms;
//     for (const int ch : w) {
//       if (ch < kThreshold) {
//         const int idx = ch;
//         terms.push_back(absl::StrCat("u[", idx, "]"));
//       } else {
//         const int idx = ch - kThreshold;
//         terms.push_back(absl::StrCat("v[", idx, "]"));
//       }
//     }
//     return absl::StrCat("IntWordExpr::single({", str_join(terms, ", "), "})");
//   });
//   std::string function_name = absl::StrCat("shuffle_product_unrolled_", n, "_", m);
//   std::cout << "IntWordExpr " << function_name << "(const IntWord& u, const IntWord& v) {\n";
//   std::cout << "  return (\n" << unrolled_expr.main() << "  );\n";
//   std::cout << "}\n";
//   std::cout << "\n";
//   return function_name;
// }

// void generate_shuffle_unrolled() {
//   std::cout << R"(#include "shuffle_unrolled.h")" "\n\n";
//   std::cout << R"(#include "absl/strings/str_cat.h")" "\n\n\n";
//   constexpr int max_len = 8;
//   std::map<int, std::map<int, std::string>> function_names;
//   for (int n = 1; n <= max_len; ++n) {
//     function_names[n] = {};
//     for (int m = 1; m <= max_len; ++m) {
//       if (n + m <= max_len) {
//         function_names[n][m] = shuffle_unrolled(n, m);
//       }
//     }
//   }
//   std::cout << "IntWordExpr shuffle_power_unrolled(const IntWord& u, const IntWord& v) {\n";
//   std::cout << "  switch (u.size()) {\n";
//   for (const auto& [n, functions_outer] : function_names) {
//     if (functions_outer.empty()) {
//       continue;
//     }
//     std::cout << "    case " << n << ":\n";
//     std::cout << "    switch (v.size()) {\n";
//     for (const auto& [m, func] : functions_outer) {
//       std::cout << "      case " << pad_left(std::to_string(m), 2) << ": return " << func << "(u, v);\n";
//     }
//     std::cout << "    }\n";
//     std::cout << "    break;\n";
//   }
//   std::cout << "  }";
//   std::cout << R"(
// FAIL(absl::StrCat(
//     "Shuffle power unroll doesn't exit for words ",
//     to_string(u), ", ", to_string(v)));
// )";
//   std::cout << "}\n";
// }

// std::string shuffle_unrolled_same(int len, int rep) {
//   constexpr int kThreshold = 100;
//   IntWord v;
//   for (int i = 0; i < len; ++i) {
//     v.push_back(i);
//   }
//   auto prod = shuffle_product(std::vector<IntWord>(rep, v));
//   auto unrolled_expr = prod.mapped<StringExpr>([&](const IntWord& w) {
//     std::vector<std::string> terms;
//     for (const int ch : w) {
//       if (ch < kThreshold) {
//         const int idx = ch;
//         terms.push_back(absl::StrCat("w[", idx, "]"));
//       }
//     }
//     return absl::StrCat("IntWordExpr::single({", str_join(terms, ", "), "})");
//   });
//   std::string function_name = absl::StrCat("shuffle_power_unrolled_len_", len, "_pow_", rep);
//   std::cout << "IntWordExpr " << function_name << "(const IntWord& w) {\n";
//   std::cout << "  return (\n" << unrolled_expr.main() << "  );\n";
//   std::cout << "}\n";
//   std::cout << "\n";
//   return function_name;
// }

// void generate_shuffle_unrolled() {
//   std::cout << R"(#include "shuffle_unrolled.h")" "\n\n";
//   std::cout << R"(#include "absl/strings/str_cat.h")" "\n\n\n";
//   constexpr int max_len = 10;
//   std::map<int, std::map<int, std::string>> function_names;
//   for (int len = 1; len <= max_len; ++len) {
//     function_names[len] = {};
//     for (int rep = 2; rep <= max_len; ++rep) {
//       if (len * rep <= max_len) {
//         function_names[len][rep] = shuffle_unrolled_same(len, rep);
//       }
//     }
//   }
//   std::cout << "IntWordExpr shuffle_power_unrolled(IntWord word, int pow) {\n";
//   std::cout << "  switch (word.size()) {\n";
//   for (const auto& [len, functions_outer] : function_names) {
//     if (functions_outer.empty()) {
//       continue;
//     }
//     std::cout << "    case " << len << ":\n";
//     std::cout << "    switch (pow) {\n";
//     for (const auto& [rep, func] : functions_outer) {
//       std::cout << "      case " << pad_left(std::to_string(rep), 2) << ": return " << func << "(word);\n";
//     }
//     std::cout << "    }\n";
//     std::cout << "    break;\n";
//   }
//   std::cout << "  }";
//   std::cout << R"(
//   FAIL(absl::StrCat(
//       "Shuffle power unroll doesn't exit for word = ",
//       to_string(word), "; power = ", pow));
//   )";
//   std::cout << "}\n";
// }


int main(int argc, char *argv[]) {
  // TODO: move; fix
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  // generate_shuffle_unrolled();
  // return 0;

  // std::cout << list_to_string(shuffle_product(IntWord{1,2}, IntWord{3,4})) << "\n";
  // std::cout << "lynd fact:\n" << list_to_string(lyndon_factorize(IntWord{1,0,1,1,0,0,1})) << "\n";
  // auto expr = Li(3, {1,2,3,4});

  // std::cout << tensor_product(D(1, 2) + 2*D(3, 4), -D(5, 6) + 3*D(7, 8)) << "\n";
  // std::cout << str_join(shuffle_product({
  //   IntWord{0,1,0},
  //   IntWord{1,0},
  //   IntWord{1},
  // }), "\n") << "\n";

  Profiler profiler;
  // auto expr = Li6(1,2,3,4,5,6,7,8);
  auto expr = Li6(1,2,3,4,5,6);
  profiler.finish("expr");
  std::cout << "Num terms before Lyndon = " << expr.size() << "\n";
  auto lyndon = to_lyndon_basis(expr);
  profiler.finish("lyndon");
  std::cout << "Num terms after Lyndon = " << lyndon.size() << "\n";

  // auto expr = Li2(1,2,3,4,5,6);
  // auto expr = Li(2, {1,2,3,4,5,6});
  // auto expr =
  //     + Li2(1,2,3,4,5,6)
  //     - (Li2(1,3,5,6) - Li2(1,4,5,6) - Li2(2,3,5,6) + Li2(2,4,5,6));
  // expr = expr.without_annotations();
  // std::cout << "Before Lyndon " << expr << "\n";
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << "After Lyndon " << lyndon << "\n";
}
