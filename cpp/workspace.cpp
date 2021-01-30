#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/packed.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"


std::vector<int> common_elements(std::vector<std::vector<int>> sets) {
  std::vector<int> ret(sets.front().begin(), sets.front().end());
  absl::c_sort(ret);
  for (auto one_set : sets) {
    absl::c_sort(one_set);
    std::vector<int> new_ret;
    absl::c_set_intersection(ret, one_set, std::back_inserter(new_ret));
    ret = std::move(new_ret);
  }
  return ret;
}

int num_common_elements(std::vector<std::vector<int>> sets) {
  return common_elements(sets).size();
}


using Loops = std::vector<std::vector<int>>;
using LoopsInvariant = std::array<int, 5>;

// std::string loops_common_elements_description(const Loops& loops) {
//   auto fully_common = to_set(common_elements(loops));
//   std::array element_collections = {
//     common_elements({loops.at(0), loops.at(1)}),
//     common_elements({loops.at(0), loops.at(2)}),
//     common_elements({loops.at(1), loops.at(2)}),
//   };
//   return str_join(element_collections, " ", [&](const auto& elements) {
//     return fmt::chevrons(
//       str_join(elements, ",", [&](int p) {
//         const auto p_str = to_string(p);
//         return fully_common.contains(p)
//           ? fmt::colored(p_str, TextColor::bright_red)
//           : fmt::colored(p_str, TextColor::bright_yellow);
//       })
//     );
//   });
// }

std::string loops_description(const Loops& loops) {
  absl::flat_hash_set<int> fully_common = to_set(common_elements(loops));
  absl::flat_hash_set<int> partially_common;
  for (const auto& common_set : {
        common_elements({loops.at(0), loops.at(1)}),
        common_elements({loops.at(0), loops.at(2)}),
        common_elements({loops.at(1), loops.at(2)}),
      }) {
    partially_common.insert(common_set.begin(), common_set.end());
  }
  return str_join(
    mapped(loops, [&](const std::vector<int>& loop) {
      return fmt::brackets(
        str_join(loop, ",", [&](int p) {
          const auto p_str = to_string(p);
          return fully_common.contains(p)
            ? fmt::colored(p_str, TextColor::bright_red)
            : partially_common.contains(p)
              ? fmt::colored(p_str, TextColor::bright_blue)
              : p_str;
        })
      );
    }),
    ""
  );
}

LoopsInvariant loops_invariant(const Loops& loops) {
  int a = num_common_elements({loops.at(0), loops.at(1)});
  int b = num_common_elements({loops.at(0), loops.at(2)});
  int c = num_common_elements({loops.at(1), loops.at(2)});
  if (loops[0].size() == loops[2].size()) {  // sync with semi_lyndon
    sort_two(a, c);
  }
  return {
    static_cast<int>(loops[1].size()),
    num_common_elements(loops),
    a,
    b,
    c,
  };
}

class LoopsNames {
public:
  std::string loops_name(const Loops& loops) {
    const auto invariant = loops_invariant(loops);
    if (!names_.contains(invariant)) {
      std::string name;
      name = fmt::braces(
        fmt::colored(
          pad_left(to_string(next_idx_++), 2),
          TextColor::bright_cyan
        )
      );
      names_[invariant] = name;
    }
    return names_.at(invariant);
  }

  int total_names() const { return next_idx_; }

private:
  int next_idx_ = 1;
  absl::flat_hash_map<LoopsInvariant, std::string> names_;
};

static LoopsNames loops_names;

struct LoopExprParam : public SimpleLinearParam<Loops> {
  static std::string object_to_string(const ObjectT& loops) {
    // const std::string loops_str = str_join(
    //   mapped(loops, [&](const std::vector<int>& loop) {
    //     return fmt::brackets(str_join(loop, ","));
    //   }),
    //   ""
    // );
    const std::string loops_str = loops_description(loops);
    const std::string loops_name_str = loops_names.loops_name(loops);
    // const std::vector<int> loops_common_elements = common_elements(loops);
    // const std::string common_elements_str = loops_common_elements.empty() ? "-" : str_join(loops_common_elements, ",");
    // const std::string common_elements_str = loops_common_elements_description(loops);
    // return absl::StrCat(loops_str, "  ", loops_name_str, "  ", common_elements_str);
    return absl::StrCat(loops_str, "  ", loops_name_str);
  }
};

using LoopExpr = Linear<LoopExprParam>;

LiraExpr loop_expr_to_lira_expr(const LoopExpr& expr) {
  return expr.mapped_expanding([](const auto& loops) {
    using RatioExpr = Linear<SimpleLinearParam<std::vector<RatioOrUnity>>>;
    const std::vector<RatioExpr> ratio_exprs =
      mapped(loops, [](const std::vector<int>& loop) -> RatioExpr {
        switch (loop.size()) {
          case 4: {
            return RatioExpr::single({Ratio(loop)});
          }
          case 5: {
            RatioExpr ret;
            for (int i = 0; i < 5; ++i) {
              ret += neg_one_pow(i) * RatioExpr::single({Ratio(removed_index(loop, i))});
            }
            return ret;
          }
          default:
            FATAL(absl::StrCat("Unsupported loop size: ", loop.size()));
        }
      });
    return outer_product(absl::MakeConstSpan(ratio_exprs),
      [](const auto& lhs, const auto& rhs) {
        return concat(lhs, rhs);
      }, AnnNone()
    ).template mapped<LiraExpr>([](const auto& loops) {
      return LiraParamOnes(loops);
    });
  });
}

LoopExpr semi_lyndon(const LoopExpr& expr) {
  return expr.mapped([&](auto loops) {
    CHECK_EQ(loops.size(), 3);
    if (loops[0].size() == loops[2].size()) {
      sort_two(loops[0], loops[2]);
    }
    return loops;
  });
}

LoopExpr fully_normalize_loops(const LoopExpr& expr) {
  return expr.mapped_expanding([&](const auto& loops) -> LoopExpr {
    int sign = 1;
    auto new_loops = mapped(loops, [&](std::vector<int> loop) {
      sign *= sort_with_sign(loop);
      return loop;
    });
    return sign * LoopExpr::single(new_loops);
  });
}

LoopExpr remove_duplicate_loops(const LoopExpr& expr) {
  return expr.filtered([&](const auto& loops) {
    for (int i = 0; i < loops.size(); ++i) {
      for (int j = 0; j < loops.size(); ++j) {
        if (i == j) {
          continue;
        }
        const auto outer = loops[i];
        const auto inner = loops[j];
        CHECK(absl::c_is_sorted(outer));
        CHECK(absl::c_is_sorted(inner));
        if (absl::c_includes(outer, inner)) {
          return false;
        }
      }
    }
    return true;
  });
}

LoopExpr loop_expr_substitute(const LoopExpr& expr, const absl::flat_hash_map<int, int>& substitutions) {
  auto loop_subst = expr.mapped_expanding([&](const Loops& loops) -> LoopExpr {
    Loops new_loops;
    for (const auto& loop : loops) {
      auto new_loop = mapped(loop, [&](int p) {
        return substitutions.contains(p) ? substitutions.at(p) : p;
      });
      if (!all_unique(new_loop)) {
        return {};
      }
      new_loops.push_back(std::move(new_loop));
    }
    return LoopExpr::single(new_loops);
  });
  return semi_lyndon(remove_duplicate_loops(fully_normalize_loops(loop_subst)));
}

LoopExpr loop_expr_substitute(const LoopExpr& expr, const std::vector<int>& new_indices) {
  absl::flat_hash_map<int, int> substitutions;
  for (int i = 0; i < new_indices.size(); ++i) {
    substitutions[i+1] = new_indices[i];
  }
  return loop_expr_substitute(expr, substitutions);
}

LoopExpr cycle(
    const LoopExpr& expr, const std::vector<std::vector<int>>& cycles) {
  absl::flat_hash_map<int, int> substitutions;
  for (const auto& cycle : cycles) {
    for (int i = 0; i < cycle.size(); ++i) {
      const int p = cycle[i];
      const int q = cycle[(i+1) % cycle.size()];
      CHECK(!substitutions.contains(p));
      substitutions[p] = q;
    }
  }
  return loop_expr_substitute(expr, substitutions);
}

LoopExpr loop_expr_degenerate(
    const LoopExpr& expr, int total_points, const std::vector<std::vector<int>>& groups) {
  std::set<int> remaining_points;
  for (int i = 1; i <= total_points; ++i) {
    remaining_points.insert(i);
  }
  int next_idx = 1;
  absl::flat_hash_map<int, int> substitutions;
  for (const auto& group : groups) {
    for (int p : group) {
      CHECK(remaining_points.count(p));
      remaining_points.erase(p);
      substitutions[p] = next_idx;
    }
    next_idx++;
  }
  for (int p : remaining_points) {
    substitutions[p] = next_idx++;
  }
  return loop_expr_substitute(expr, substitutions);
}

LoopExpr loop_expr_smart_combine(const LoopExpr& a, const LoopExpr& b, int total_points) {
  LoopExpr best_expr = a + b;
  for (const auto& seq : all_permutations(seq_incl(1, total_points))) {
    for (int sign : {-1, 1}) {
      LoopExpr candidate = a + sign * loop_expr_substitute(b, seq);
      if (candidate.l1_norm() < best_expr.l1_norm()) {
        best_expr = candidate;
      }
    }
  }
  return best_expr;
}

struct LoopsDecomposition {                        // elements in loops number...
  std::vector<int> elements_3_way;                 // (0,1,2)
  std::array<std::vector<int>, 3> elements_2_way;  // (0,1), (0,2), (1,2)
  std::array<std::vector<int>, 3> elements_1_way;  // (0), (1), (2)
};

LoopsDecomposition decompose_loops(const Loops& loops) {
  CHECK_EQ(loops.size(), 3);
  std::vector<int> elements_3_way = common_elements(loops);
  std::array<std::vector<int>, 3> elements_2_way = {
    set_difference(common_elements({loops[0], loops[1]}), elements_3_way),
    set_difference(common_elements({loops[0], loops[2]}), elements_3_way),
    set_difference(common_elements({loops[1], loops[2]}), elements_3_way),
  };
  std::vector<int> elements_2_or_3_way = concat(elements_3_way, flatten(elements_2_way));
  absl::c_sort(elements_2_or_3_way);
  CHECK(all_unique(elements_2_or_3_way));
  std::array<std::vector<int>, 3> elements_1_way = {
    set_difference(sorted(loops[0]), elements_2_or_3_way),
    set_difference(sorted(loops[1]), elements_2_or_3_way),
    set_difference(sorted(loops[2]), elements_2_or_3_way),
  };
  return {elements_3_way, elements_2_way, elements_1_way};
}

LoopExpr to_canonical_permutation(const LoopExpr& expr) {
  return expr.mapped_expanding([](const Loops& loops) {
    const auto decomposition = decompose_loops(loops);
    std::vector<int> full_permutation = concat(
      decomposition.elements_3_way,
      flatten(decomposition.elements_2_way),
      flatten(decomposition.elements_1_way)
    );
    int sign = 1;
    // TODO: Inverse into a vector instead.
    absl::flat_hash_map<int, int> position_of;
    for (int i = 0; i < full_permutation.size(); ++i) {
      position_of[full_permutation[i]] = i + 1;
    }
    Loops new_loops;
    for (const auto& loop : loops) {
      const std::vector<std::pair<int, int>> loop_permutation_orig = mapped(loop, [&](int p) {
        return std::pair{position_of.at(p), p};
      });
      auto loop_permutation = loop_permutation_orig;
      const int loop_permutation_sign = sort_with_sign(loop_permutation);
      sign *= loop_permutation_sign;
      new_loops.push_back(mapped(loop_permutation, [&](auto p) { return p.second; }));
    }
    return sign * LoopExpr::single(new_loops);
  });
}

StringExpr expr_type_1_to_column(const LoopExpr& expr) {
  return expr.mapped<StringExpr>([](const Loops& loops) {
    const auto decomposition = decompose_loops(loops);
    // std::vector<int> v = {
    //   decomposition.elements_3_way.at(0),
    //   decomposition.elements_3_way.at(1),
    //   decomposition.elements_1_way.at(2).at(0),
    //   decomposition.elements_1_way.at(2).at(1),
    //   decomposition.elements_1_way.at(0).at(0),
    //   decomposition.elements_2_way.at(0).at(0),
    //   decomposition.elements_2_way.at(2).at(0),
    // };
    std::vector<int> v = concat(
      decomposition.elements_3_way,        // x2
      decomposition.elements_1_way.at(2),  // x2
      decomposition.elements_1_way.at(0),
      decomposition.elements_2_way.at(0),
      decomposition.elements_2_way.at(2)
    );
    CHECK_EQ(v.size(), 7);
    return fmt::brackets(str_join(v, ","));
  });
}

int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_formatter(Formatter::ascii)
    .set_formatter(Formatter::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_annotation_sorting(AnnotationSorting::length)
  );


  static const int N = 9;
  LoopExpr loop_templates;

  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6}, {1,6,7,8,9}});
  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6,7}, {1,7,8,9}});
  loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,8,9}, {4,5,6,7,8}});
  loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,9}, {5,6,7,8,9}});
  loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,8,9}, {5,6,7,8}});

  // In Lyndon basis:
  // loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,5,6}, {1,6,7,8,9}});
  // loop_templates += LoopExpr::single({{1,2,3,4}, {1,7,8,9}, {1,4,5,6,7}});
  // loop_templates += LoopExpr::single({{1,7,8,9}, {1,2,3,4}, {1,4,5,6,7}});
  // loop_templates -= LoopExpr::single({{1,2,3,4}, {1,4,8,9}, {4,5,6,7,8}});
  // loop_templates += LoopExpr::single({{1,2,3,4}, {1,4,5,9}, {5,6,7,8,9}});
  // loop_templates -= LoopExpr::single({{1,2,3,4}, {5,6,7,8}, {1,4,5,8,9}});
  // loop_templates -= LoopExpr::single({{5,6,7,8}, {1,2,3,4}, {1,4,5,8,9}});

  auto loop_expr = loop_templates.mapped_expanding([](const Loops& loops) {
    return sum_looped_vec([&](const std::vector<X>& x_args) {
      const auto args = mapped(x_args, [](X x) { return x.var(); });
      return LoopExpr::single(
        mapped(loops, [&](const std::vector<int>& loop) {
          return choose_indices_one_based(args, loop);
        })
      );
    }, 9, {1,2,3,4,5,6,7,8,9}, SumSign::plus);
  });
  loop_expr = semi_lyndon(loop_expr);


  // std::cout << loop_expr_degenerate(loop_expr, N, {{1,3}});
  // return 0;


  const auto a = loop_expr_degenerate(loop_expr, N, {{1,3}, {2,4}});
  const auto b = loop_expr_degenerate(loop_expr, N, {{1,3}, {2,5}});
  const auto c = loop_expr_degenerate(loop_expr, N, {{1,4}, {2,5}});
  const auto d = loop_expr_degenerate(loop_expr, N, {{1,3}, {4,6}});  // == cycle(d, {{1,2}, {3,4}, {5,7}})
  const auto e = loop_expr_degenerate(loop_expr, N, {{1,3}, {4,7}});
  const auto f = loop_expr_degenerate(loop_expr, N, {{1,3}, {5,7}});  // == cycle(f, {{1,2}, {3,5}, {6,7}})
  const auto g = loop_expr_degenerate(loop_expr, N, {{1,3}, {2,6}});
  const auto h = loop_expr_degenerate(loop_expr, N, {{1,3}, {5,8}});
  const auto i = loop_expr_degenerate(loop_expr, N, {{1,4}, {2,6}});
  const auto j = loop_expr_degenerate(loop_expr, N, {{1,5}, {2,4}});  // == cycle(j, {{4,7}, {5,6}})
  const auto k = loop_expr_degenerate(loop_expr, N, {{1,6}, {2,4}});
  const auto l = loop_expr_degenerate(loop_expr, N, {{1,6}, {2,5}});
  const auto x = loop_expr_degenerate(loop_expr, N, {{1,4}, {2,7}});
  const auto y = loop_expr_degenerate(loop_expr, N, {{1,4}, {2,8}});
  const auto z = loop_expr_degenerate(loop_expr, N, {{1,4}, {5,8}});
  const auto u = loop_expr_degenerate(loop_expr, N, {{1,5}, {2,6}});
  const auto w = loop_expr_degenerate(loop_expr, N, {{1,5}, {3,7}});
  const auto m = loop_expr_degenerate(loop_expr, N, {{1,3,5}});
  const auto n = loop_expr_degenerate(loop_expr, N, {{1,3,6}});
  const auto o = loop_expr_degenerate(loop_expr, N, {{1,4,7}});

  const auto a1 = cycle(a, {{2,4}, {5,7}});
  const auto v = n + m - a + a1;  // NICE  ({1},{1},{2})

  // std::cout << "a " << a << "\n";
  // std::cout << "m " << m << "\n";
  // std::cout << "v " << v << "\n";
  // // std::cout << "b " << b << "\n";
  // std::cout << "c " << c << "\n";
  // std::cout << "d " << d << "\n";
  // std::cout << "e " << e << "\n";
  // std::cout << "f " << f << "\n";
  // // std::cout << "g " << g << "\n";
  // std::cout << "h " << h << "\n";
  // std::cout << "i " << i << "\n";
  // std::cout << "j " << j << "\n";
  // std::cout << "k " << k << "\n";
  // std::cout << "l " << l << "\n";
  // // std::cout << "n " << n << "\n";
  // std::cout << "o " << o << "\n";
  // std::cout << "x " << x << "\n";
  // std::cout << "y " << y << "\n";
  // std::cout << "z " << z << "\n";
  // std::cout << "u " << u << "\n";
  // // std::cout << "w " << w << "\n";

  // std::cout << "===\n\n";

  const auto p = n + m;
  // std::cout << p << "\n";

  // std::cout << loop_expr_substitute(c, {{3,6}, {6,7}, {7,3}}) << "\n";
  // const auto q =
  //   + c
  //   + loop_expr_substitute(c, {{3,6}, {6,7}, {7,3}})
  //   + loop_expr_substitute(c, {{6,3}, {7,6}, {3,7}})
  // ;

  // const auto r = c + i;
  // std::cout << r << "\n";
  // const auto p1 = loop_expr_substitute(p, {{1,2}, {2,3}, {3,1}});
  // std::cout << p1 << "\n";
  // std::cout << r + p1 << "\n";
  // const auto d1 = loop_expr_substitute(d, {{4,2}, {2,4}, {5,7}, {7,5}});
  // const auto s = j + d1;
  // std::cout << d1 << "\n";
  // std::cout << s << "\n";

  // const auto d1 = loop_expr_substitute(d, {{2,3}, {3,2}});
  // const auto t = j + d1;
  // std::cout << j << "\n";
  // std::cout << d1 << "\n";
  // std::cout << t << "\n";

  // ZERO:  n - cycle(g, {{1,2}}) - a
  // ZERO (but above is simpler):  g - cycle(v, {{1,2}}) - cycle(a, {{1,4}, {5,7}}) + cycle(m, {{1,2}})
  // ZERO:  n + m - cycle(g + b, {{1,2}})

  const auto v1 = cycle(a, {{2,6}, {3,4,5}});
  // std::cout << v1 << "\n";
  const auto va = v1 + m;
  const auto va1 = cycle(va, {{1,2,3}, {5,6}, {4,7}});
  const auto vb = b - va1;
  // std::cout << va << "\n";
  // std::cout << va1 << "\n";
  // std::cout << vb << "\n";
  // std::cout << cycle(vb, {{1,4}, {5,6}}) << "\n";
  // std::cout << cycle(vb, {{1,4,7}, {5,6}}) << "\n";
  // std::cout << cycle(vb, {{1,4}, {5,6,7}}) << "\n";
  // std::cout << cycle(vb, {{1,6}, {5,4}}) << "\n";

  // const auto k1 = cycle(k, {{1,2}});
  // const auto w = i + k1;
  // // std::cout << k1 << "\n";
  // // std::cout << w << "\n";

  const auto a2 = cycle(a, {{2,4}, {5,7}});
  const auto b1 = cycle(b, {{1,2}});
  // std::cout << n - a << "\n";
  // std::cout << a2 << "\n";
  // std::cout << n - a + a2 << "\n";
  // std::cout << b1 << "\n";
  // std::cout << n - a + a2 + b1 << "\n";
  // std::cout << n - a + a2 + b1 - a << "\n";
  // std::cout << n - a + a2 + b1 - a - v << "\n";
  // ZERO:  n - a + a2 + b1 - a - v

  const auto m1 = cycle(m, {{1,2,5}, {6,7}});
  const auto m2 = cycle(m, {{2,5,6}, {4,7}});
  const auto fm = f + m1 + m2;  // NICE: {1,1,9,9}
  // std::cout << m1 << "\n";
  // std::cout << m2 << "\n";
  // std::cout << fm << "\n";

  const auto efd = e + f + d;
  // std::cout << efd << "\n";

  // std::cout << cycle(fm, {{3,1}, {5,2}}) << "\n";
  const auto c1 = cycle(c, {{4,7}, {5,6}});
  const auto c2 = c - c1;
  // std::cout << c1 << "\n";
  // std::cout << c2 << "\n";
  // std::cout << cycle(c2, {{6,7}}) << "\n";
  // std::cout << c2 + cycle(c2, {{6,7}}) << "\n";
  // std::cout << c1 + j << "\n";

  const auto c3 = cycle(c, {{1,2}});
  const auto c4 = c - c3;
  // std::cout << c << "\n";
  // std::cout << c3 << "\n";
  // std::cout << c4 << "\n";
  // std::cout << cycle(c4, {{1,6}}) << "\n";
  // std::cout << c4 - cycle(c4, {{1,6}}) << "\n";

  // const auto x1 =
  //   + x
  //   - cycle(fm, {{5,6}, {4,7}})
  //   - cycle(d, {{5,6}, {4,7}})
  // ;
  // std::cout << x1 << "\n";
  // const auto x2 =
  //   + x
  //   + cycle(e, {{5,6}, {4,7}})
  // ;
  // // std::cout << x2 << "\n";
  // const auto x3 =
  //   + x2
  //   - cycle(m, {{1,2}, {4,6}, {3,7}})
  //   - m
  // ;
  // // std::cout << x3 << "\n";
  // const auto x4 =  // :(
  //   + x3
  //   + cycle(v, {{1,2}, {4,6}, {3,7}})
  //   - cycle(a, {{1,6}, {3,5}})
  // ;
  // // std::cout << x4 << "\n";

  const auto o0 =
    + o
    // - m
    - cycle(m, {{2,6}, {3,5}})
    - cycle(m, {{3,7}, {4,6}})
    - cycle(m, {{2,4}, {5,7}})
    - a
    - v
    + cycle(a, {{2,4}, {5,7}})
    // + cycle(a, {{2,7}, {3,4}, {5,6}})
    + cycle(v, {{2,3,4,5,6,7}})
    - cycle(a, {{2,3}, {4,7}, {5,6}})
    - cycle(v, {{2,3}, {4,7}, {5,6}})
  ;
  const auto m0 = m - cycle(m, {{2,6}});  //  ~=  m - cycle(m, {{3,5}})
  const auto v0 = v + cycle(v, {{1,6}});
  std::cout << o0 << "\n";
  std::cout << m0 << "\n";
  std::cout << v0 << "\n";

  std::cout << "===\n\n";

  std::cout << to_canonical_permutation(o0) << "\n";
  std::cout << to_canonical_permutation(m0) << "\n";
  std::cout << to_canonical_permutation(v0) << "\n";

  std::cout << "===\n\n";

  std::cout << expr_type_1_to_column(to_canonical_permutation(o0)) << "\n";
  std::cout << expr_type_1_to_column(to_canonical_permutation(m0)) << "\n";
  std::cout << expr_type_1_to_column(to_canonical_permutation(v0)) << "\n";
}
