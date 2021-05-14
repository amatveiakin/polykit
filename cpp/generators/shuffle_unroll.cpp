#include <iostream>
#include <sstream>

#include "absl/strings/substitute.h"

#include "lib/shuffle.h"


std::string shuffle_unrolled(int n, int m) {
  std::vector<std::string> u, v;
  for (int i : range(n)) {
    u.push_back(absl::Substitute("u[$0]", i));
  }
  for (int i : range(m)) {
    v.push_back(absl::Substitute("v[$0]", i));
  }
  std::vector<std::string> lines;
  for (const auto& [term, coeff] : shuffle_product(u, v)) {
    CHECK_EQ(coeff, 1);
    lines.push_back(absl::Substitute("  ret.add_to_key({$0}, $1);", str_join(term, ", "), coeff));
  }
  absl::c_sort(lines);
  std::string function_name = absl::StrCat("shuffle_product_unrolled_", n, "_", m);
  std::cout << "template<typename MonomT>\n";
  std::cout << "Linear<SimpleLinearParam<MonomT>> " << function_name << "(const MonomT& u, const MonomT& v) {\n";
  std::cout << "  Linear<SimpleLinearParam<MonomT>> ret;\n";
  for (const auto& line : lines) {
    std::cout << line << "\n";
  }
  std::cout << "  return ret;\n";
  std::cout << "}\n";
  std::cout << "\n";
  return function_name;
}

void generate_shuffle_unrolled() {
  std::cout << R"(#include "linear.h")" "\n\n\n";
  constexpr int max_len = 6;
  std::map<int, std::map<int, std::string>> function_names;
  for (int n : range_incl(1, max_len)) {
    function_names[n] = {};
    for (int m : range_incl(n, max_len)) {
      if (n + m <= max_len) {
        function_names[n][m] = shuffle_unrolled(n, m);
      }
    }
  }
  std::cout << "template<typename MonomT>\n";
  std::cout << "Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled(MonomT u, MonomT v) {\n";
  std::cout << "  if (u.size() > v.size()) {\n";
  std::cout << "    std::swap(u, v);\n";
  std::cout << "  }\n";
  std::cout << "  switch (u.size()) {\n";
  for (const auto& [n, functions_outer] : function_names) {
    if (functions_outer.empty()) {
      continue;
    }
    std::cout << "    case " << n << ":\n";
    std::cout << "      switch (v.size()) {\n";
    for (const auto& [m, func] : functions_outer) {
      std::cout << "        case " << pad_left(std::to_string(m), 1) << ": return " << func << "(u, v);\n";
    }
    std::cout << "      }\n";
    std::cout << "      break;\n";
  }
  std::cout << "  }\n";
  std::cout << "  return {};\n";
  std::cout << "}\n";
}


int main(int argc, char *argv[]) {
  generate_shuffle_unrolled();
}
