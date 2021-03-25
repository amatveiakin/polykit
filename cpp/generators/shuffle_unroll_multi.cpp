#include <iostream>
#include <sstream>

#include "absl/strings/substitute.h"

#include "lib/shuffle.h"


std::string shuffle_unrolled_multi(const std::vector<int>& lengths) {
  std::vector<std::vector<std::string>> v;
  std::vector<std::string> arg_names;
  for (int i : range(lengths.size())) {
    arg_names.push_back(absl::StrCat("const MonomT& v", i));
    v.push_back({});
    for (int j : range(lengths[i])) {
      v.back().push_back(absl::Substitute("v$0[$1]", i, j));
    }
  }
  std::vector<std::string> lines;
  for (const auto& [term, coeff] : shuffle_product(v)) {
    CHECK_EQ(coeff, 1);
    lines.push_back(absl::Substitute("  ret.add_to_key({$0}, $1);", str_join(term, ", "), coeff));
  }
  absl::c_sort(lines);
  std::string function_name = absl::StrCat("shuffle_product_unrolled_", str_join(lengths, "_"));
  std::cout << "template<typename MonomT>\n";
  std::cout << "Linear<SimpleLinearParam<MonomT>> " << function_name << "(" << str_join(arg_names, ", ") << ") {\n";
  std::cout << "  Linear<SimpleLinearParam<MonomT>> ret;\n";
  for (const auto& line : lines) {
    std::cout << line << "\n";
  }
  std::cout << "  return ret;\n";
  std::cout << "}\n";
  std::cout << "\n";
  return function_name;
}

void generate_shuffle_unrolled_3_words() {
  std::cout << R"(#include "linear.h")" "\n\n\n";
  constexpr int max_len = 6;
  std::map<int, std::map<int, std::map<int, std::string>>> function_names;
  for (int n : range_incl(1, max_len)) {
    function_names[n] = {};
    for (int m : range_incl(n, max_len)) {
      function_names[n][m] = {};
      for (int k : range_incl(m, max_len)) {
        if (n + m + k <= max_len) {
          function_names[n][m][k] = shuffle_unrolled_multi({n, m, k});
        }
      }
    }
  }
  std::cout << "template<typename MonomT>\n";
  std::cout << "Linear<SimpleLinearParam<MonomT>> shuffle_product_unrolled_multi(std::vector<MonomT>& v) {\n";
  std::cout << "  if (v.size() != 3) {\n";
  std::cout << "    return {};\n";
  std::cout << "  }\n";
  std::cout << "  absl::c_sort(v, [](const auto& a, const auto& b) { return a.size() < b.size(); });\n";
  std::cout << "  switch (v[0].size()) {\n";
  for (const auto& [n, func1] : function_names) {
    if (absl::c_all_of(func1, [](const auto& e) { return e.second.empty(); })) {
      continue;
    }
    std::cout << "    case " << n << ":\n";
    std::cout << "      switch (v[1].size()) {\n";
    for (const auto& [m, func2] : func1) {
      if (func2.empty()) {
        continue;
      }
      std::cout << "        case " << m << ":\n";
      std::cout << "          switch (v[2].size()) {\n";
      for (const auto& [k, func] : func2) {
        std::cout << "            case " << k << ": return " << func << "(v[0], v[1], v[2]);\n";
      }
      std::cout << "          }\n";
      std::cout << "          break;\n";
    }
    std::cout << "      }\n";
    std::cout << "      break;\n";
  }
  std::cout << "  }\n";
  std::cout << "  return {};\n";
  std::cout << "}\n";
}


int main(int argc, char *argv[]) {
  generate_shuffle_unrolled_3_words();
}
