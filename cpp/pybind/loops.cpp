#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "lib/loops.h"
#include "lib/polylog_liquad.h"


namespace py = pybind11;

// TODO: Move to lyndon.h
// TODO: Test
std::vector<std::vector<int>> gen_lyndon_words(int alphabet_size, int max_length) {
  std::vector<int> last_word{0};
  std::vector<std::vector<int>> words{last_word};
  while (true) {
    std::vector<int> word;
    for (int i : range(max_length)) {
      word.push_back(last_word[i % last_word.size()]);
    }
    while (!word.empty() && word.back() == alphabet_size - 1) {
      word.pop_back();
    }
    if (word.empty()) {
      break;
    }
    word.back() += 1;
    words.push_back(word);
    last_word = word;
  }
  return words;
}

std::vector<int> substitute(const std::vector<int>& points, const std::vector<int>& new_indices) {
  return mapped(points, [&](const int p) { return new_indices.at(p - 1); });
}

// TODO: Replace with:
//   - proper Python functional interface, or
//   - binary matrix interface, or
//   - C++ linear algebra (via Eigen?)
std::vector<std::vector<int>> loops_matrix() {
  static constexpr int num_points = 8;
  static constexpr int weight = num_points / 2 - 1;
  auto lyndon_words = gen_lyndon_words(4, weight);
  auto basis = filtered(lyndon_words, [](const auto& w) { return w.size() == weight; });
  CHECK_EQ(basis.size(), 20) << dump_to_string(basis);
  absl::flat_hash_map<std::vector<int>, int> to_basis;
  for (int i : range(basis.size())) {
    to_basis[basis[i]] = i;
  }

  std::vector<std::vector<int>> mat;
  std::vector args{1,2,3,4,5};
  do {
    const std::vector points = substitute({1,2,1,3,1,4,5,4}, args);
    // const std::vector points = substitute({1,2,1,3,1,2,4,5}, args);
    CHECK_EQ(num_points, points.size());
    auto source = LiQuad(weight, points);
    auto lira_expr = without_unities(theta_expr_to_lira_expr_without_products(source.without_annotations()));
    auto expr = lira_expr_to_loop_expr(lira_expr);
    expr = fully_normalize_loops(expr);
    expr = loops_var5_shuffle_internally(expr);
    expr = to_lyndon_basis(expr);
    std::vector<int> column(basis.size(), 0);
    for (const auto& [term, coeff] : expr) {
      auto term_indexed = mapped(term, [](const std::vector<int> loop) {
        int ret = sum(loop) - (1+2+3+4);
        CHECK_LE(0, ret);
        CHECK_LT(ret, 4);
        return ret;
      });
      CHECK(to_basis.contains(term_indexed)) << dump_to_string(term_indexed);
      column.at(to_basis.at(term_indexed)) = coeff;
    }
    mat.push_back(column);
  } while (absl::c_next_permutation(args));
  return mat;
}

PYBIND11_MODULE(loops, m) {
  m.def("loops_matrix", &loops_matrix);
}
