#include <iostream>
#include <sstream>

#include "lib/shuffle.h"


std::string shuffle_unrolled(int n, int m) {
  constexpr int kThreshold = 100;
  Word u, v;
  for (int i = 0; i < n; ++i) {
    u.push_back(i);
  }
  for (int i = 0; i < m; ++i) {
    v.push_back(i + kThreshold);
  }
  auto prod = shuffle_product(u, v);
  auto unrolled_expr = prod.mapped<StringExpr>([&](const Word& w) {
    std::vector<std::string> terms;
    for (const int ch : w) {
      if (ch < kThreshold) {
        const int idx = ch;
        terms.push_back(absl::StrCat("u[", idx, "]"));
      } else {
        const int idx = ch - kThreshold;
        terms.push_back(absl::StrCat("v[", idx, "]"));
      }
    }
    return absl::StrCat("WordExpr::single({", str_join(terms, ", "), "})");
  });
  std::string function_name = absl::StrCat("shuffle_product_unrolled_", n, "_", m);
  std::cout << "WordExpr " << function_name << "(const Word& u, const Word& v) {\n";
  std::cout << "  return (\n" << unrolled_expr.main() << "  );\n";
  std::cout << "}\n";
  std::cout << "\n";
  return function_name;
}

void generate_shuffle_unrolled() {
  std::cout << R"(#include "shuffle_unrolled.h")" "\n\n\n";
  constexpr int max_len = 8;
  std::map<int, std::map<int, std::string>> function_names;
  for (int n = 1; n <= max_len; ++n) {
    function_names[n] = {};
    for (int m = n; m <= max_len; ++m) {
      if (n + m <= max_len) {
        function_names[n][m] = shuffle_unrolled(n, m);
      }
    }
  }
  std::cout << "WordExpr shuffle_product_unrolled(Word u, Word v) {\n";
  std::cout << "  if (u.size() > v.size()) {\n";
  std::cout << "    std::swap(u, v);\n";
  std::cout << "  }\n";
  std::cout << "  switch (u.size()) {\n";
  for (const auto& [n, functions_outer] : function_names) {
    if (functions_outer.empty()) {
      continue;
    }
    std::cout << "    case " << n << ":\n";
    std::cout << "    switch (v.size()) {\n";
    for (const auto& [m, func] : functions_outer) {
      std::cout << "      case " << pad_left(std::to_string(m), 2) << ": return " << func << "(u, v);\n";
    }
    std::cout << "    }\n";
    std::cout << "    break;\n";
  }
  std::cout << "  }\n";
  std::cout << "  return {};\n";
  std::cout << "}\n";
}

std::string shuffle_power_unrolled(int len, int rep) {
  constexpr int kThreshold = 100;
  Word v;
  for (int i = 0; i < len; ++i) {
    v.push_back(i);
  }
  auto prod = shuffle_product(std::vector<Word>(rep, v));
  auto unrolled_expr = prod.mapped<StringExpr>([&](const Word& w) {
    std::vector<std::string> terms;
    for (const int ch : w) {
      if (ch < kThreshold) {
        const int idx = ch;
        terms.push_back(absl::StrCat("w[", idx, "]"));
      }
    }
    return absl::StrCat("WordExpr::single({", str_join(terms, ", "), "})");
  });
  std::string function_name = absl::StrCat("shuffle_power_unrolled_len_", len, "_pow_", rep);
  std::cout << "WordExpr " << function_name << "(const Word& w) {\n";
  std::cout << "  return (\n" << unrolled_expr.main() << "  );\n";
  std::cout << "}\n";
  std::cout << "\n";
  return function_name;
}

void generate_shuffle_power_unrolled() {
  std::cout << R"(#include "shuffle_unrolled.h")" "\n\n";
  std::cout << R"(#include "absl/strings/str_cat.h")" "\n\n\n";
  constexpr int max_len = 10;
  std::map<int, std::map<int, std::string>> function_names;
  for (int len = 1; len <= max_len; ++len) {
    function_names[len] = {};
    for (int rep = 2; rep <= max_len; ++rep) {
      if (len * rep <= max_len) {
        function_names[len][rep] = shuffle_power_unrolled(len, rep);
      }
    }
  }
  std::cout << "WordExpr shuffle_power_unrolled(Word word, int pow) {\n";
  std::cout << "  switch (word.size()) {\n";
  for (const auto& [len, functions_outer] : function_names) {
    if (functions_outer.empty()) {
      continue;
    }
    std::cout << "    case " << len << ":\n";
    std::cout << "    switch (pow) {\n";
    for (const auto& [rep, func] : functions_outer) {
      std::cout << "      case " << pad_left(std::to_string(rep), 2) << ": return " << func << "(word);\n";
    }
    std::cout << "    }\n";
    std::cout << "    break;\n";
  }
  std::cout << "  }";
  std::cout << R"(
  FAIL(absl::StrCat(
      "Shuffle power unroll doesn't exit for word = ",
      to_string(word), "; power = ", pow));
  )";
  std::cout << "}\n";
}


int main(int argc, char *argv[]) {
  generate_shuffle_unrolled();
}
