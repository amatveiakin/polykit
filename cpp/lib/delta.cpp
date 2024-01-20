#include "delta.h"

#include "absl/container/flat_hash_set.h"

#include "util.h"


std::string dump_to_string_impl(const Delta& d) {
  return fmt::brackets(absl::StrCat(to_string(d.a()), ",", to_string(d.b())));
}

std::string to_string(const Delta& d) {
  HSpacing hspacing = *current_formatting_config().compact_x ? HSpacing::dense : HSpacing::sparse;
  SWITCH_ENUM_OR_DIE(d.b().form(), {
    case XForm::var:
    case XForm::sq_var:
    case XForm::infinity:
      return fmt::parens(fmt::diff(to_string(d.a()), to_string(d.b()), hspacing));
    case XForm::neg_var:
      return fmt::parens(fmt::sum(to_string(d.a()), to_string(-d.b()), hspacing));
    case XForm::zero: {
      // Add padding to preserve columns in a typical case
      const std::string a_str = to_string(d.a());
      const int reference_width = strlen_utf8(fmt::parens(fmt::diff(a_str, a_str, hspacing)));
      return pad_right(fmt::parens(a_str), reference_width);
    }
    case XForm::undefined:
      break;
  });
}


DeltaAlphabetMapping delta_alphabet_mapping;

X DeltaAlphabetMapping::alphabet_to_x(int ch) {
  CHECK_LE(0, ch);
  if (ch < kVarCodeEnd) {
    return X(XForm::var, ch - kVarCodeStart + X::kMinIndex);
  } else if (ch < kNegVarCodeEnd) {
    return X(XForm::neg_var, ch - kNegVarCodeStart + X::kMinIndex);
  } else if (ch == kZeroCode) {
    return Zero;
  } else {
    FATAL(absl::StrCat("Unexpected character: ", ch));
  }
}

DeltaAlphabetMapping::DeltaAlphabetMapping() {
  static constexpr int kAlphabetSize = kMaxDimension * (kMaxDimension - 1) / 2;
  static_assert(kAlphabetSize <= std::numeric_limits<internal::DeltaDiffT>::max() + 1);
  deltas_.resize(kAlphabetSize);
  for (int b : range(0, kMaxDimension)) {
    for (int a : range(0, b)) {
      CHECK_EQ(x_to_alphabet(alphabet_to_x(b)), b);
      Delta d(alphabet_to_x(a), alphabet_to_x(b));
      deltas_.at(to_alphabet(d)) = d;
    }
  }
}


Delta monom_substitute_variables(Delta d, const std::vector<X>& new_points) {
  return Delta(
    d.a().substitution_result_0_based(new_points),
    d.b().substitution_result_0_based(new_points)
  );
}

DeltaExpr substitute_variables_0_based(const DeltaExpr& expr, const XArgs& new_points) {
  const auto& new_points_v = new_points.as_x();
  return expr.mapped_expanding([&](const DeltaExpr::ObjectT& term_old) -> DeltaExpr {
    std::vector<Delta> term_new;
    for (const Delta& d_old : term_old) {
      const Delta d_new = monom_substitute_variables(d_old, new_points_v);
      if (d_new.is_nil()) {
        return {};
      }
      term_new.push_back(d_new);
    }
    return DeltaExpr::single(term_new);
  }).without_annotations();
}

// TODO: Adopt optimized implementation to support X forms.
#if 0
#if DISABLE_PACKING
// ...
#else
DeltaExpr substitute_variables_1_based(const DeltaExpr& expr, const XArgs& new_points_arg) {
  constexpr int kMaxChar = std::numeric_limits<unsigned char>::max();
  constexpr int kNoReplacement = kMaxChar;
  constexpr int kNil = kMaxChar - 1;
  const auto& new_points = new_points_arg.as_x();
  std::array<unsigned char, kMaxChar> replacements;
  replacements.fill(kNoReplacement);
  const int num_src_vars = new_points.size();
  for (int a : range_incl(1, num_src_vars)) {
    for (int b : range_incl(a+1, num_src_vars)) {
      const Delta before = Delta(a, b);
      const Delta after = Delta(new_points[a-1], new_points[b-1]);
      const unsigned char key_before = delta_alphabet_mapping.to_alphabet(before);
      if (after.is_nil()) {
        replacements.at(key_before) = kNil;
      } else {
        const unsigned char key_after = delta_alphabet_mapping.to_alphabet(after);
        CHECK(key_after != kNoReplacement);
        CHECK(key_after != kNil);
        replacements.at(key_before) = key_after;
      }
    }
  }
  DeltaExpr ret;
  expr.foreach_key([&](const DeltaExpr::StorageT& term_old, int coeff) {
    DeltaExpr::StorageT term_new;
    for (unsigned char ch : term_old) {
      unsigned char ch_new = replacements.at(ch);
      CHECK(ch_new != kNoReplacement);
      if (ch_new == kNil) {
        return;
      }
      term_new.push_back(ch_new);
    }
    ret.add_to_key(term_new, coeff);
  });
  return ret;
}
#endif
#endif

DeltaExpr substitute_variables_1_based(const DeltaExpr& expr, const XArgs& new_points) {
  return substitute_variables_0_based(expr, concat({X::Undefined()}, new_points.as_x()));
}

DeltaNCoExpr substitute_variables_0_based(const DeltaNCoExpr& expr, const XArgs& new_points) {
  const auto& new_points_v = new_points.as_x();
  const auto expr_new = expr.mapped_expanding([&](const DeltaNCoExpr::ObjectT& term_old) -> DeltaNCoExpr {
    std::vector<std::vector<Delta>> term_new;
    for (const auto& copart_old : term_old) {
      std::vector<Delta> copart_new;
      for (const Delta& d_old : copart_old) {
        const Delta d_new = monom_substitute_variables(d_old, new_points_v);
        if (d_new.is_nil()) {
          return {};
        }
        copart_new.push_back(d_new);
      }
      term_new.push_back(copart_new);
    }
    return DeltaNCoExpr::single(term_new);
  }).without_annotations();
  return normalize_coproduct(expr_new);
}

DeltaNCoExpr substitute_variables_1_based(const DeltaNCoExpr& expr, const XArgs& new_points) {
  return substitute_variables_0_based(expr, concat({X::Undefined()}, new_points.as_x()));
}

DeltaExpr involute(const DeltaExpr& expr, const std::vector<int>& points) {
  return expr.mapped_expanding([&](const std::vector<Delta>& term) {
    return tensor_product(absl::MakeConstSpan(
      mapped(term, [&](const Delta& d) -> DeltaExpr {
        const auto [p1, p2, p3, p4, p5, p6] = to_array<6>(points);
        if (d == Delta(p6,p5)) {
          return D(p6,p1) - D(p1,p2) + D(p2,p3) - D(p3,p4) + D(p4,p5);
        } else if (d == Delta(p6,p4)) {
          return D(p4,p2) + D(p3,p1) - D(p1,p5) + D(p6,p1) - D(p1,p2) - D(p3,p4) + D(p4,p5);
        } else if (d == Delta(p6,p2)) {
          return D(p6,p1) - D(p1,p5) + D(p5,p3) - D(p3,p4) + D(p4,p2);
        } else {
          return DeltaExpr::single({d});
        }
      })
    ));
  });
}

DeltaExpr sort_term_multiples(const DeltaExpr& expr) {
  return expr.mapped([&](const std::vector<Delta>& term) {
    return sorted(term);
  });
}

DeltaExpr terms_with_unique_muptiples(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    const auto term_sorted = sorted(term);
    return absl::c_adjacent_find(term_sorted) == term_sorted.end();
  });
}

DeltaExpr terms_with_nonunique_muptiples(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    const auto term_sorted = sorted(term);
    return absl::c_adjacent_find(term_sorted) != term_sorted.end();
  });
}


DeltaExpr terms_with_num_distinct_variables(const DeltaExpr& expr, int num_distinct) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return num_distinct_variables(term) == num_distinct;
  });
}

DeltaExpr terms_with_min_distinct_variables(const DeltaExpr& expr, int min_distinct) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return num_distinct_variables(term) >= min_distinct;
  });
}

DeltaExpr terms_containing_only_variables(const DeltaExpr& expr, const std::vector<int>& indices) {
  absl::flat_hash_set<int> indices_set(indices.begin(), indices.end());
  return expr.filtered([&](const std::vector<Delta>& term) {
    return absl::c_all_of(term, [&](const Delta& d) {
      return indices_set.contains(d.a().idx()) && indices_set.contains(d.b().idx());
    });
  });
}

DeltaExpr terms_without_variables(const DeltaExpr& expr, const std::vector<int>& indices) {
  absl::flat_hash_set<int> indices_set(indices.begin(), indices.end());
  return expr.filtered([&](const std::vector<Delta>& term) {
    return !absl::c_any_of(term, [&](const Delta& d) {
      return indices_set.contains(d.a().idx()) && indices_set.contains(d.b().idx());
    });
  });
}


static bool between(int point, std::pair<int, int> segment) {
  const auto [a, b] = segment;
  CHECK_LT(a, b);
  ASSERT(all_unique_unsorted(std::array{point, a, b}));
  return a < point && point < b;
}

// TODO: Test.
bool are_weakly_separated(const Delta& d1, const Delta& d2) {
  if (d1.is_nil() || d2.is_nil()) {
    return true;
  }
  const int x1 = d1.a().as_simple_var();
  const int y1 = d1.b().as_simple_var();
  const int x2 = d2.a().as_simple_var();
  const int y2 = d2.b().as_simple_var();
  if (!all_unique_unsorted(std::array{x1, y1, x2, y2})) {
    return true;
  }
  const bool intersect = between(x1, {x2, y2}) != between(y1, {x2, y2});
  return !intersect;
}

// Optimization potential: consider whether this can be done in O(N) time;
bool is_weakly_separated(const DeltaExpr::ObjectT& term) {
  for (int i : range(term.size())) {
    for (int j : range(i)) {
      if (!are_weakly_separated(term[i], term[j])) {
        return false;
      }
    }
  }
  return true;
}
bool is_weakly_separated(const DeltaNCoExpr::ObjectT& term) {
  return is_weakly_separated(flatten(term));
}


static bool less_inv(X a, X b) {
  CHECK(a.form() == XForm::var || a.form() == XForm::neg_var);
  CHECK(b.form() == XForm::var || b.form() == XForm::neg_var);
  return cmp::projected(a, b, [](X x) { return std::pair{x.form(), x.idx()}; });
}

static bool between_inv(X point, std::pair<X, X> segment) {
  const auto [a, b] = segment;
  CHECK_LT(a, b);
  return less_inv(a, point) && less_inv(point, b);
}

std::array<X, 2> delta_points_inv(const Delta& d) {
  return d.b() == Zero
    ? std::array{d.a(), d.a().negated()}
    : std::array{d.a(), d.b()};
}

static auto delta_points_inv(const Delta& d, bool invert) {
  const auto points = delta_points_inv(d);
  return invert
    ? sorted(points, less_inv)
    : sorted(mapped_array(points, [](const X x) { return x.negated(); }), less_inv);
}

// TODO: Test.
bool are_weakly_separated_inv(const Delta& d1, const Delta& d2) {
  if (d1.is_nil() || d2.is_nil()) {
    return true;
  }
  for (const bool invert_d1 : {false, true}) {
    for (const bool invert_d2 : {false, true}) {
      const auto [x1, y1] = delta_points_inv(d1, invert_d1);
      const auto [x2, y2] = delta_points_inv(d2, invert_d2);
      if (!all_unique_unsorted(std::array{x1, y1, x2, y2}, less_inv)) {
        continue;
      }
      const bool intersect = between_inv(x1, {x2, y2}) != between_inv(y1, {x2, y2});
      if (intersect) {
        return false;
      }
    }
  }
  return true;
}

bool is_weakly_separated_inv(const DeltaExpr::ObjectT& term) {
  for (int i : range(term.size())) {
    for (int j : range(i)) {
      if (!are_weakly_separated_inv(term[i], term[j])) {
        return false;
      }
    }
  }
  return true;
}
bool is_weakly_separated_inv(const DeltaNCoExpr::ObjectT& term) {
  return is_weakly_separated_inv(flatten(term));
}

bool is_totally_weakly_separated_inv(const DeltaExpr& expr) {
  return !expr.contains([](const auto& term) { return !is_weakly_separated_inv(term); });
}
bool is_totally_weakly_separated_inv(const DeltaNCoExpr& expr) {
  return !expr.contains([](const auto& term) { return !is_weakly_separated_inv(term); });
}

DeltaExpr keep_non_weakly_separated_inv(const DeltaExpr& expr) {
  return expr.filtered([](const auto& term) { return !is_weakly_separated_inv(term); });
}
DeltaNCoExpr keep_non_weakly_separated_inv(const DeltaNCoExpr& expr) {
  return expr.filtered([](const auto& term) { return !is_weakly_separated_inv(term); });
}


// TODO: Remove circular neighbour (n,1) when the number of points n is odd,
//   similarly to passes_normalize_remove_consecutive for GammaExpr.
bool passes_normalize_remove_consecutive(const DeltaExpr::ObjectT& term) {
  return absl::c_all_of(term, [](const Delta& d) {
    int a = d.a().as_simple_var();
    int b = d.b().as_simple_var();
    sort_two(a, b);
    return b != a + 1;
  });
}

DeltaExpr normalize_remove_consecutive(const DeltaExpr& expr) {
  return expr.filtered([](const auto& term) {
    return passes_normalize_remove_consecutive(term);
  });
}

bool inv_points_are_central_symmetric(const XArgs& xpoints) {
  const auto points = xpoints.as_x();
  if (points.size() % 2 != 0) {
    return false;
  }
  const int half_num_points = points.size() / 2;
  for (const int i : range(half_num_points)) {
    if (points[i] != -points[i+half_num_points]) {
      return false;
    }
  }
  return true;
}


static void graph_mark_reached(
    int start,
    const std::vector<std::vector<int>>& nbrs,
    std::vector<bool>& reached) {
  if (reached.at(start)) {
    return;
  }
  reached.at(start) = true;
  for (int v : nbrs.at(start)) {
    graph_mark_reached(v, nbrs, reached);
  }
}

static bool graph_is_connected(const std::vector<Delta>& deltas) {
  std::vector<std::pair<int, int>> graph;
  for (const Delta& d : deltas) {
    if (!d.a().is_constant() && !d.b().is_constant()) {
      graph.push_back({d.a().idx(), d.b().idx()});
    }
  }
  if (graph.empty()) {
    return true;
  }
  int max_vertex = -1;
  for (const auto& e : graph) {
    max_vertex = std::max({max_vertex, e.first, e.second});
  }
  std::vector<std::vector<int>> nbrs(max_vertex+1, std::vector<int>{});
  for (const auto& e : graph) {
    nbrs[e.first].push_back(e.second);
    nbrs[e.second].push_back(e.first);
  }
  std::vector<bool> reached(max_vertex+1, false);
  graph_mark_reached(graph.front().first, nbrs, reached);
  for (const auto& e : graph) {
    if (!reached[e.first] || !reached[e.second]) {
      return false;
    }
  }
  return true;
}

DeltaExpr terms_with_connected_variable_graph(const DeltaExpr& expr) {
  return expr.filtered([&](const std::vector<Delta>& term) {
    return graph_is_connected(term);
  });
}

int count_var(const DeltaExpr::ObjectT& term, int var) {
  return absl::c_count_if(term, [&](const Delta& d) {
    return d.a().idx() == var || d.b().idx() == var;
  });
};

int num_distinct_variables(const std::vector<Delta>& term) {
  std::vector<int> elements;
  for (const Delta& d : term) {
    if (!d.a().is_constant()) {
      elements.push_back(d.a().idx());
    }
    if (!d.b().is_constant()) {
      elements.push_back(d.b().idx());
    }
  }
  return num_distinct_elements_unsorted(elements);
}

void print_sorted_by_num_distinct_variables(std::ostream& os, const DeltaExpr& expr) {
  to_ostream_grouped(
    os, expr, std::less<>{},
    num_distinct_variables, std::less<>{},
    [](int num_vars) {
      return absl::StrCat(num_vars, " vars");
    },
    &DeltaExpr::Param::object_to_string
  );
}
