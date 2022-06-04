#include <iostream>
#include <sstream>

#include "absl/strings/substitute.h"

#include "cpp/lib/shuffle.h"


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

std::string shuffle_unrolled_rust(int n, int m) {
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
    lines.push_back(absl::Substitute(
      "    ret.add_to(vec_like_cloned![MonomT; $0], $1);",
      str_join(term, ", "), coeff
    ));
  }
  absl::c_sort(lines);
  std::string function_name = absl::StrCat("shuffle_product_unrolled_", n, "_", m);
  std::cout << "fn " << function_name << "<T, MonomT>(u: &MonomT, v: &MonomT) -> Linear<MonomT>\n";
  std::cout << "where\n";
  std::cout << "    T: Clone,\n";
  std::cout << "    MonomT: LinearMonom + VectorLike<T>,\n";
  std::cout << "{\n";
  std::cout << "    let mut ret = Linear::<MonomT>::zero();\n";
  for (const auto& line : lines) {
    std::cout << line << "\n";
  }
  std::cout << "    ret\n";
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
    for (int m : range_incl(n, max_len - n)) {
      function_names[n][m] = shuffle_unrolled(n, m);
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

void generate_shuffle_unrolled_rust() {
  constexpr int max_len = 6;
  std::map<int, std::map<int, std::string>> function_names;
  for (int n : range_incl(1, max_len)) {
    function_names[n] = {};
    for (int m : range_incl(n, max_len - n)) {
      function_names[n][m] = shuffle_unrolled_rust(n, m);
    }
  }
  std::cout << "fn shuffle_product_unrolled<T, MonomT>(u: &MonomT, v: &MonomT) -> Option<Linear<MonomT>>\n";
  std::cout << "where\n";
  std::cout << "    T: Clone,\n";
  std::cout << "    MonomT: LinearMonom + VectorLike<T>,\n";
  std::cout << "{\n";
  std::cout << "    match (u.len(), v.len()) {\n";
  for (int n : range_incl(1, max_len)) {
    for (int m : range_incl(1, max_len - n)) {
      if (n <= m) {
        std::cout << "        (" << n << ", " << m << ") => Some(" << function_names[n][m] << "(u, v)),\n";
      } else {
        std::cout << "        (" << n << ", " << m << ") => Some(" << function_names[m][n] << "(v, u)),\n";
      }
    }
  }
  std::cout << "        (_, _) => None,\n";
  std::cout << "    }\n";
  std::cout << "}\n";
}



int main(int argc, char *argv[]) {
  if (argc == 2 && argv[1] == std::string("rust")) {
    generate_shuffle_unrolled_rust();
  } else {
    generate_shuffle_unrolled();
  }
}
