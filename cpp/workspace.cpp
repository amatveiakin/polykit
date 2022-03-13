#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/delta_parse.h"
#include "lib/enumerator.h"
#include "lib/expr_matrix_builder.h"
#include "lib/file_util.h"
#include "lib/format.h"
#include "lib/gamma.h"
#include "lib/integer_math.h"
#include "lib/iterated_integral.h"
#include "lib/itertools.h"
#include "lib/lexicographical.h"
#include "lib/linalg.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog_cgrli.h"
#include "lib/polylog_grli.h"
#include "lib/polylog_grqli.h"
#include "lib/polylog_li.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/polylog_via_correlators.h"
#include "lib/polylog_qli.h"
#include "lib/polylog_space.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/pvector.h"
#include "lib/range.h"
#include "lib/sequence_iteration.h"
#include "lib/set_util.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/table_printer.h"
#include "lib/theta.h"
#include "lib/triangulation.h"
#include "lib/zip.h"


GammaExpr SymmCGrLi3(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  constexpr int weight = 3;
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  auto expr =
    + CGrLi(weight, args({1,2,3,4,5,6}))
  ;
  expr +=
    - GrQLiVec(weight, args({2}), args({3,4,5,6}))
    + GrQLiVec(weight, args({5}), args({1,2,3,6}))
    + GrQLiVec(weight, args({2}), args({1,3,4,5}))
    - GrQLiVec(weight, args({5}), args({1,2,4,6}))
    - GrQLiVec(weight, args({2}), args({1,3,4,6}))
    + GrQLiVec(weight, args({5}), args({1,3,4,6}))
    + GrQLiVec(weight, args({3}), args({1,2,4,6}))
    - GrQLiVec(weight, args({6}), args({1,3,4,5}))
    + GrQLiVec(weight, args({1}), args({3,4,5,6}))
    - GrQLiVec(weight, args({4}), args({1,2,3,6}))
  ;
  // TODO: Why is this not equivalent? !!!
  // expr += sum_looped_vec([](const std::vector<int>& points) {
  //   const auto args = [&](const std::vector<int>& indices) {
  //     return choose_indices_one_based(points, indices);
  //   };
  //   return
  //     - GrQLiVec(weight, args({1}), args({2,3,4,6}))
  //     // + GrQLiVec(weight, args({1}), args({2,4,5,6}))
  //   ;
  // }, 6, {1,2,3,4,5,6}, SumSign::alternating);
  // return expr;
  expr -=
    - GrQLiVec(weight, args({1}), args({2,3,4,6}))
    + GrQLiVec(weight, args({2}), args({1,3,4,5}))
    - GrQLiVec(weight, args({3}), args({2,4,5,6}))
    + GrQLiVec(weight, args({4}), args({1,3,5,6}))
    - GrQLiVec(weight, args({5}), args({1,2,4,6}))
    + GrQLiVec(weight, args({6}), args({1,2,3,5}))
  ;
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("SymmCGrLi"), {weight}),
      points
    )
  );
}

// TODO: Finish symmetrizing
GammaExpr SymmCGrLi4(const std::vector<int>& points) {
  CHECK_EQ(points.size(), 6);
  constexpr int weight = 4;
  const auto args = [&](const std::vector<int>& indices) {
    return choose_indices_one_based(points, indices);
  };
  auto expr =
    + CGrLi(weight, args({1,2,3,4,5,6}))
    - CGrLi(weight, args({2,3,4,5,6,1}))
    + CGrLi(weight, args({3,4,5,6,1,2}))
    - CGrLi(weight, args({4,5,6,1,2,3}))
    + CGrLi(weight, args({5,6,1,2,3,4}))
    - CGrLi(weight, args({6,1,2,3,4,5}))
  ;
  expr -=
    + CGrLi(weight, args({1,6,5,4,3,2}))
    - CGrLi(weight, args({6,5,4,3,2,1}))
    + CGrLi(weight, args({5,4,3,2,1,6}))
    - CGrLi(weight, args({4,3,2,1,6,5}))
    + CGrLi(weight, args({3,2,1,6,5,4}))
    - CGrLi(weight, args({2,1,6,5,4,3}))
  ;
  // return expr;
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("SymmCGrLi"), {weight}),
      points
    )
  );
}

GammaExpr symmetrize_double(const GammaExpr& expr, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  const int sign = neg_one_pow(num_points);
  return expr + sign * substitute_variables(expr, rotated_vector(points, 1));
}

GammaExpr symmetrize_loop(const GammaExpr& expr, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  GammaExpr ret;
  for (const int i : range(num_points)) {
    const int sign = num_points % 2 == 1 ? 1 : neg_one_pow(i);
    ret += sign * substitute_variables(expr, rotated_vector(points, i));
  }
  return ret;
}

// TODO: Test:  symmetrize_double(symmetrize_loop(x)) == 0

template<typename Container>
static Container one_minus_cross_ratio(Container p) {
  CHECK_EQ(4, p.size());
  std::swap(p[1], p[2]);
  return p;
}

PolylogSpace CL1_inv(const std::vector<int>& args) {
  const auto& args_inv = concat(
    mapped(args, [](const int idx) { return X(idx); }),
    mapped(args, [](const int idx) { return -X(idx); })
  );
  PolylogSpace space;
  const int weight = 1;
  append_vector(space, mapped_expanding(combinations(args_inv, 4), [&](const auto& p) {
    return std::array{
      QLiVec(weight, p),
      QLiVec(weight, one_minus_cross_ratio(p))
    };
  }));
  return space;
}

PolylogSpace CL2_inv(const std::vector<int>& args) {
  const auto& args_inv = concat(
    mapped(args, [](const int idx) { return X(idx); }),
    mapped(args, [](const int idx) { return -X(idx); })
  );
  PolylogSpace space;
  const int weight = 2;
  append_vector(space, mapped(combinations(args_inv, 4), [&](const auto& p) {
    return QLiVec(weight, p);
  }));
  return space;
}

// TODO: Factor out
// TODO: Optimize: don't compute rank on each step
// TODO: Optimize: precompute `prepare(expr)`
// Optimization potential: pre-apply for common spaces
template<typename SpaceT, typename PrepareF>
SpaceT space_basis(const SpaceT& space, const PrepareF& prepare) {
  SpaceT ret;
  int rank = 0;
  for (const auto& expr : space) {
    ret.push_back(expr);
    const int new_rank = space_rank(ret, prepare);
    if (new_rank == rank) {
      ret.pop_back();
    }
    rank = new_rank;
  }
  return ret;
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

// static bool is_nil_inv(const Delta& d) {
//   return d.is_nil() || d.a() == Zero || d.b() == Zero;
// }

auto delta_points_inv(const Delta& d) {
  // HACK: "unglue" points assuming (x_i-0) compues only from (x_i-(-x_i)).  TODO: remove.
  return d.b() == Zero
    ? std::array{d.a(), d.a().negated()}
    : std::array{d.a(), d.b()};
}

auto delta_points_inv(const Delta& d, bool invert) {
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

// auto expand_inv(const Delta& d) {
//   return std::vector{d.a(), d.a().negated(), d.b(), d.b().negated()};
// }

// // TODO: Test.
// bool are_weakly_separated_inv(const Delta& d1, const Delta& d2) {
//   const auto d1_expanded = expand_inv(d1);
//   const auto d2_expanded = expand_inv(d2);
//   const auto a = set_difference(d1_expanded, d2_expanded);
//   const auto b = set_difference(d2_expanded, d1_expanded);
//   if (a.empty() || b.empty()) {
//     return true;
//   }
//   int last_color = 0;  // a: 1,  b: 2
//   int num_color_changes = 0;
//   CHECK_EQ(a.size(), b.size());
//   for (const auto& p : set_union(
//     mapped(a, [](const X x) { return std::pair{x, 1}; }),
//     mapped(b, [](const X x) { return std::pair{x, 2}; })
//   )) {
//     const int color = p.second;
//     if (color != 0) {
//       if (last_color != color) {
//         ++num_color_changes;
//         last_color = color;
//       }
//     }
//   }
//   CHECK_LE(2, num_color_changes) << dump_to_string(d1) << " vs " << dump_to_string(d2);
//   return num_color_changes <= 3;
// }

// Optimization potential: consider whether this can be done in O(N) time;
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



DeltaNCoExpr supervremenno(X x1, X x2, X x3, X x4) {
  return
    + ncoproduct(QLi2(x1,x2,x3,x4), QLi2(x4,-x1,-x4,x1))
    - ncoproduct(QLi2(x2,x3,x4,-x1), QLi2(-x1,-x2,x1,x2))
    + ncoproduct(QLi2(x3,x4,-x1,-x2), QLi2(-x2,-x3,x2,x3))
    - ncoproduct(QLi2(x4,-x1,-x2,-x3), QLi2(-x3,-x4,x3,x4))
  ;
}

DeltaNCoExpr QLi4_Comult_2_2(X x1, X x2, X x3, X x4, X x5, X x6) {
  return
    + ncoproduct(QLi2(x1,x2,x3,x4), QLi2(x1,x4,x5,x6))
    + ncoproduct(QLiNeg2(x2,x3,x4,x5), QLi2(x1,x2,x5,x6))
    + ncoproduct(QLi2(x3,x4,x5,x6), QLi2(x1,x2,x3,x6))
  ;
}


int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    .set_unicode_version(UnicodeVersion::simple)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_expression_line_limit(30)
    .set_annotation_sorting(AnnotationSorting::length)
    .set_compact_x(true)
  );

  Profiler profiler;
  int64_t checksum = 0;
  for (EACH : range(1000)) {
    // checksum += to_lyndon_basis(QLi4(1,2,3,4,5,6,7,8)).l1_norm();
    checksum += QLi4(1,2,3,4,5,6,7,8).l1_norm();
  }
  std::cout << "Checksum: " << checksum << "\n";
  profiler.finish("QLi");

  return 0;



  // const int weight = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // auto space = CGrL_Dim4_naive_test_space(weight, points);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // for (const int shift : range(points.size() / 2)) {
  //   space.push_back(CGrLi(weight, rotated_vector(points, shift)));
  // }
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // std::cout << is_totally_weakly_separated(substitute_variables(CGrLi(4, {1,2,3,4,5,6,7,8}), {1,2,3,4,5,6,1,8})) << "\n";
  // std::cout << substitute_variables(CGrLi(5, {1,2,3,4,5,6,7,8}), {1,2,3,3,5,6,7,8}) << "\n";

  // const int weight = 4;
  // auto space = CGrL_Dim4_naive_test_space(weight, {1,2,3,4,5,6,7,8});
  // std::cout << to_string(space_venn_ranks(
  //   space,
  //   // {CGrLi(weight, {1,1,3,4,5,6,7,8})},
  //   {substitute_variables(CGrLi(weight, {1,2,3,4,5,6,7,8}), {1,2,3,4,5,6,1,8})},
  //   DISAMBIGUATE(to_lyndon_basis)
  // )) << "\n";

  // const auto expr = CGrLi(4, {1,1,3,4,5,6,7,8});
  // const auto expr_alt = substitute_variables(CGrLi(4, {1,2,3,4,5,6,7,8}), {1,1,3,4,5,6,7,8});
  // std::cout << (expr == expr_alt) << "\n";

  // const int weight = 5;
  // const std::vector points = {1,2,3,4,5,6,7,8,9};
  // std::cout << "w=" << weight << "\n";
  // auto space = CGrL_Dim4_naive_test_space(weight, points);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // const int dimension = 4;
  // const std::vector points = {1,2,3,4,5,6,7,8};
  // std::cout << to_string(space_venn_ranks(
  //   mapped(cartesian_product(CGrL_Dim4_naive_test_space(4, points), GrL1(dimension, points)), applied(DISAMBIGUATE(ncoproduct))),
  //   {ncomultiply(CGrLi(5, points))},
  //   DISAMBIGUATE(identity_function)
  // )) << "\n";



#if 0
  const auto prepare = [](const GammaExpr& expr) {
    // return expr.filtered([](const auto& term) {
    //   return absl::c_all_of(term, [](const Gamma& g) {
    //     return g.index_bitset().test(1 - Gamma::kBitsetOffset);
    //   });
    // });
    // return to_lyndon_basis(expr.filtered([](const auto& term) {
    //   Gamma::BitsetT common_vars;
    //   Gamma::BitsetT all_vars;
    //   common_vars.set();
    //   for (const Gamma& g : term) {
    //     all_vars |= g.index_bitset();
    //     common_vars &= g.index_bitset();
    //   }
    //   Gamma::BitsetT expected_common_vars = vector_to_bitset_or<Gamma::BitsetT>({1}, Gamma::kBitsetOffset).value();
    //   Gamma::BitsetT expected_vars = vector_to_bitset_or<Gamma::BitsetT>({1,2,4,5,6}, Gamma::kBitsetOffset).value();
    //   return common_vars == expected_common_vars && all_vars == expected_vars;
    // }));
    return to_lyndon_basis(expr.filtered([](const auto& term) {
      Gamma::BitsetT common_vars;
      Gamma::BitsetT all_vars;
      common_vars.set();
      for (const Gamma& g : term) {
        all_vars |= g.index_bitset();
        common_vars &= g.index_bitset();
      }
      return bitset_to_vector(common_vars).size() == 1 && bitset_to_vector(all_vars).size() == 5;
    }));
  };

  const auto to_grqli_func_expr = [](const auto& term) -> StringExpr {
    Gamma::BitsetT common_vars;
    Gamma::BitsetT all_vars;
    common_vars.set();
    for (const Gamma& g : term) {
      all_vars |= g.index_bitset();
      common_vars &= g.index_bitset();
    }
    const auto common_vars_vector = bitset_to_vector(common_vars, Gamma::kBitsetOffset);
    const auto all_vars_vector = bitset_to_vector(all_vars, Gamma::kBitsetOffset);
    if (common_vars_vector.size() == 1 && all_vars_vector.size() == 5) {
      return StringExpr::single(absl::StrCat(
        "GrQLi3(",
        common_vars_vector.front(),
        ")(",
        str_join(set_difference(all_vars_vector, common_vars_vector), ","),
        ")"
      ));
    } else {
      return {};
    }
  };
#endif

  // auto expr = (
  //   + CGrLi(3, {1,2,3,4,5,6})
  //   - CGrLi(3, {1,2,3,4,5,7})
  //   + CGrLi(3, {1,2,3,4,6,7})
  //   - CGrLi(3, {1,2,3,5,6,7})
  //   + CGrLi(3, {1,2,4,5,6,7})
  //   - CGrLi(3, {1,3,4,5,6,7})
  //   + CGrLi(3, {2,3,4,5,6,7})
  // );

  // std::cout << prepare(expr);
  // std::cout << prepare(GrQLi3(1)(2,4,5,6));
  // std::cout << prepare(expr - GrQLi3(1)(2,4,5,6));
  // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);

  // expr += (
  //   - GrQLi3(1)(2,4,5,6)
  //   + GrQLi3(1)(2,4,5,7)
  //   - GrQLi3(1)(2,4,6,7)
  //   + GrQLi3(1)(3,4,5,6)
  //   - GrQLi3(1)(3,4,5,7)
  //   + GrQLi3(1)(3,4,6,7)
  //   + GrQLi3(2)(1,4,5,6)
  //   - GrQLi3(2)(1,4,5,7)
  //   + GrQLi3(2)(1,4,6,7)
  //   - GrQLi3(2)(3,4,5,6)
  //   + GrQLi3(2)(3,4,5,7)
  //   - GrQLi3(2)(3,4,6,7)
  //   - GrQLi3(3)(1,4,5,6)
  //   + GrQLi3(3)(1,4,5,7)
  //   - GrQLi3(3)(1,4,6,7)
  //   + GrQLi3(3)(2,4,5,6)
  //   - GrQLi3(3)(2,4,5,7)
  //   + GrQLi3(3)(2,4,6,7)
  // );
  // // std::cout << prepare(expr);
  // std::cout << to_lyndon_basis(expr);

  // // std::cout << to_string(space_venn_ranks(
  // //   GrL_core(3, 3, {1,2,3,4,5,6,7}, false, 0),
  // //   {expr},
  // //   DISAMBIGUATE(to_lyndon_basis)
  // // )) << "\n";


  // const auto lhs =
  //   +6*CGrLi(3, {1,2,3,4,5,6})
  // ;
  // const auto rhs =
  //   + CGrLi(3, {1,2,3,4,5,6})
  //   - CGrLi(3, {2,3,4,5,6,1})
  //   + CGrLi(3, {3,4,5,6,1,2})
  //   - CGrLi(3, {4,5,6,1,2,3})
  //   + CGrLi(3, {5,6,1,2,3,4})
  //   - CGrLi(3, {6,1,2,3,4,5})
  // ;
  // auto expr = lhs - rhs;
  // // std::cout << prepare(expr);
  // // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);
  // expr +=
  //   -2 * GrQLi3(1)(2,3,4,5)
  //   -4 * GrQLi3(2)(3,4,5,6)
  //   -2 * GrQLi3(3)(1,4,5,6)
  //   +2 * GrQLi3(4)(1,2,5,6)
  //   +4 * GrQLi3(5)(1,2,3,6)
  //   +2 * GrQLi3(6)(1,2,3,4)

  //   +2 * GrQLi3(1)(2,3,4,6)
  //   +4 * GrQLi3(2)(1,3,4,5)
  //   +2 * GrQLi3(3)(2,4,5,6)
  //   -2 * GrQLi3(4)(1,3,5,6)
  //   -4 * GrQLi3(5)(1,2,4,6)
  //   -2 * GrQLi3(6)(1,2,3,5)

  //   -2 * GrQLi3(1)(2,3,5,6)
  //   -4 * GrQLi3(2)(1,3,4,6)
  //   -2 * GrQLi3(3)(1,2,4,5)
  //   +2 * GrQLi3(4)(2,3,5,6)
  //   +4 * GrQLi3(5)(1,3,4,6)
  //   +2 * GrQLi3(6)(1,2,4,5)

  //   -2 * GrQLi3(1)(2,4,5,6)
  //   +2 * GrQLi3(2)(1,3,5,6)
  //   +4 * GrQLi3(3)(1,2,4,6)
  //   +2 * GrQLi3(4)(1,2,3,5)
  //   -2 * GrQLi3(5)(2,3,4,6)
  //   -4 * GrQLi3(6)(1,3,4,5)

  //   +4 * GrQLi3(1)(3,4,5,6)
  //   +2 * GrQLi3(2)(1,4,5,6)
  //   -2 * GrQLi3(3)(1,2,5,6)
  //   -4 * GrQLi3(4)(1,2,3,6)
  //   -2 * GrQLi3(5)(1,2,3,4)
  //   +2 * GrQLi3(6)(2,3,4,5)
  // ;
  // std::cout << prepare(expr);

  // std::cout << to_lyndon_basis(
  //   + SymmCGrLi3({1,2,3,4,5,6})
  //   + SymmCGrLi3({2,3,4,5,6,1})
  // );


  // auto expr =
  //   + CGrLi(3, {1,2,3,4,5,6})
  //   + CGrLi(3, {2,3,4,5,6,1})
  // ;
  // expr +=
  //   - GrQLi3(1)(2,4,5,6)
  //   + GrQLi3(1)(3,4,5,6)
  //   + GrQLi3(2)(1,3,4,5)
  //   - GrQLi3(2)(1,3,4,6)
  //   + GrQLi3(2)(1,4,5,6)
  //   - GrQLi3(2)(3,4,5,6)
  //   - GrQLi3(3)(1,2,4,5)
  //   + GrQLi3(3)(1,2,4,6)
  //   - GrQLi3(3)(1,4,5,6)
  //   + GrQLi3(3)(2,4,5,6)
  //   + GrQLi3(4)(1,2,3,5)
  //   - GrQLi3(4)(1,2,3,6)
  //   - GrQLi3(5)(1,2,3,4)
  //   + GrQLi3(5)(1,2,3,6)
  //   - GrQLi3(5)(1,2,4,6)
  //   + GrQLi3(5)(1,3,4,6)
  //   + GrQLi3(6)(1,2,3,4)
  //   - GrQLi3(6)(1,2,3,5)
  //   + GrQLi3(6)(1,2,4,5)
  //   - GrQLi3(6)(1,3,4,5)
  // ;
  // // std::cout << prepare(expr);
  // std::cout << to_lyndon_basis(expr);


  // auto expr = (
  //   + SymmCGrLi3({1,2,3,4,5,6})
  //   - SymmCGrLi3({1,2,3,4,5,7})
  //   + SymmCGrLi3({1,2,3,4,6,7})
  //   - SymmCGrLi3({1,2,3,5,6,7})
  //   + SymmCGrLi3({1,2,4,5,6,7})
  //   - SymmCGrLi3({1,3,4,5,6,7})
  //   + SymmCGrLi3({2,3,4,5,6,7})
  // );
  // expr +=
  //   - GrQLi3(1)(2,3,4,6)
  //   + GrQLi3(1)(2,3,5,7)
  //   - GrQLi3(1)(2,4,5,6)
  //   - GrQLi3(1)(2,4,6,7)
  //   + GrQLi3(1)(3,4,5,7)
  //   + GrQLi3(1)(3,5,6,7)
  //   + GrQLi3(2)(1,3,4,6)
  //   - GrQLi3(2)(1,3,5,7)
  //   + GrQLi3(2)(1,4,5,6)
  //   + GrQLi3(2)(1,4,6,7)
  //   - GrQLi3(2)(3,4,5,7)
  //   - GrQLi3(2)(3,5,6,7)
  //   - GrQLi3(3)(1,2,4,6)
  //   + GrQLi3(3)(1,2,5,7)
  //   - GrQLi3(3)(1,4,5,6)
  //   - GrQLi3(3)(1,4,6,7)
  //   + GrQLi3(3)(2,4,5,7)
  //   + GrQLi3(3)(2,5,6,7)
  //   + GrQLi3(4)(1,2,3,6)
  //   - GrQLi3(4)(1,2,5,7)
  //   + GrQLi3(4)(1,3,5,6)
  //   + GrQLi3(4)(1,3,6,7)
  //   - GrQLi3(4)(2,3,5,7)
  //   - GrQLi3(4)(2,5,6,7)
  //   - GrQLi3(5)(1,2,3,6)
  //   + GrQLi3(5)(1,2,4,7)
  //   - GrQLi3(5)(1,3,4,6)
  //   - GrQLi3(5)(1,3,6,7)
  //   + GrQLi3(5)(2,3,4,7)
  //   + GrQLi3(5)(2,4,6,7)
  //   + GrQLi3(6)(1,2,3,5)
  //   - GrQLi3(6)(1,2,4,7)
  //   + GrQLi3(6)(1,3,4,5)
  //   + GrQLi3(6)(1,3,5,7)
  //   - GrQLi3(6)(2,3,4,7)
  //   - GrQLi3(6)(2,4,5,7)
  //   - GrQLi3(7)(1,2,3,5)
  //   + GrQLi3(7)(1,2,4,6)
  //   - GrQLi3(7)(1,3,4,5)
  //   - GrQLi3(7)(1,3,5,6)
  //   + GrQLi3(7)(2,3,4,6)
  //   + GrQLi3(7)(2,4,5,6)
  // ;
  // // std::cout << prepare(expr);
  // std::cout << to_lyndon_basis(expr);  // ZERO
  // // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);



  // auto expr =
  //   + SymmCGrLi4({1,2,3,4,5,6})
  //   + SymmCGrLi4({2,3,4,5,6,7})
  //   + SymmCGrLi4({3,4,5,6,7,1})
  //   + SymmCGrLi4({4,5,6,7,1,2})
  //   + SymmCGrLi4({5,6,7,1,2,3})
  //   + SymmCGrLi4({6,7,1,2,3,4})
  //   + SymmCGrLi4({7,1,2,3,4,5})
  // ;
  // expr +=
  //   -4*GrQLi4(1)(2,3,4,5,6,7)
  //   -4*GrQLi4(2)(3,4,5,6,7,1)
  //   -4*GrQLi4(3)(4,5,6,7,1,2)
  //   -4*GrQLi4(4)(5,6,7,1,2,3)
  //   -4*GrQLi4(5)(6,7,1,2,3,4)
  //   -4*GrQLi4(6)(7,1,2,3,4,5)
  //   -4*GrQLi4(7)(1,2,3,4,5,6)
  // ;
  // std::cout << to_string(space_venn_ranks(
  //   GrL_core(4, 3, {1,2,3,4,5,6,7}, false, 0),
  //   {expr},
  //   DISAMBIGUATE(to_lyndon_basis)
  // )) << "\n";
  // // std::cout << prepare(expr);
  // // std::cout << to_lyndon_basis(expr);
  // // std::cout << to_lyndon_basis(expr).termwise_abs().mapped_expanding(to_grqli_func_expr);



  // std::cout << to_string(space_mapping_ranks(
  //   CGrL3_Dim3_test_space({1,2,3,4,5,6}),
  //   // CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{
  //       // to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,1})),
  //       // to_lyndon_basis(expr - substitute_variables(expr, {1,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(expr, {1,2,3,4,5,6})
  //         - substitute_variables(expr, {1,2,3,4,5,7})
  //         + substitute_variables(expr, {1,2,3,4,6,7})
  //         - substitute_variables(expr, {1,2,3,5,6,7})
  //         + substitute_variables(expr, {1,2,4,5,6,7})
  //         - substitute_variables(expr, {1,3,4,5,6,7})
  //         + substitute_variables(expr, {2,3,4,5,6,7})
  //       ),
  //       to_lyndon_basis(
  //         + pullback(substitute_variables(expr, {1,2,3,4,5,6}), {7})
  //         - pullback(substitute_variables(expr, {1,2,3,4,5,7}), {6})
  //         + pullback(substitute_variables(expr, {1,2,3,4,6,7}), {5})
  //         - pullback(substitute_variables(expr, {1,2,3,5,6,7}), {4})
  //         + pullback(substitute_variables(expr, {1,2,4,5,6,7}), {3})
  //         - pullback(substitute_variables(expr, {1,3,4,5,6,7}), {2})
  //         + pullback(substitute_variables(expr, {2,3,4,5,6,7}), {1})
  //       ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CGrL3_Dim3_test_space({1,2,3,4,5}),
  //   // CGrL_Dim3_naive_test_space(4, {1,2,3,4,5}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     // const auto subsum =
  //     //   + substitute_variables(expr, {1,2,3,4,5})
  //     //   + substitute_variables(expr, {2,3,4,5,1})
  //     //   + substitute_variables(expr, {3,4,5,1,2})
  //     //   + substitute_variables(expr, {4,5,1,2,3})
  //     //   + substitute_variables(expr, {5,1,2,3,4})
  //     // ;
  //     const auto subsum = expr;
  //     const auto sum =
  //       + substitute_variables(subsum, {1,2,3,4,5})
  //       - substitute_variables(subsum, {2,3,4,5,6})
  //       + substitute_variables(subsum, {3,4,5,6,1})
  //       - substitute_variables(subsum, {4,5,6,1,2})
  //       + substitute_variables(subsum, {5,6,1,2,3})
  //       - substitute_variables(subsum, {6,1,2,3,4})
  //     ;
  //     return to_lyndon_basis(sum);
  //   }
  // )) << "\n";


  // std::cout << to_string(space_mapping_ranks(
  //   CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7,8}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{
  //       to_lyndon_basis(expr + substitute_variables(expr, {2,3,4,5,6,7,8,1})),
  //       to_lyndon_basis(expr + substitute_variables(expr, {1,8,7,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(expr, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(expr, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(expr, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(expr, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(expr, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(expr, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(expr, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(expr, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(expr, {2,3,4,5,6,7,8,9})
  //       ),
  //       // to_lyndon_basis(
  //       //   + pullback(substitute_variables(expr, {1,2,3,4,5,6,7,8}), {9})
  //       //   - pullback(substitute_variables(expr, {1,2,3,4,5,6,7,9}), {8})
  //       //   + pullback(substitute_variables(expr, {1,2,3,4,5,6,8,9}), {7})
  //       //   - pullback(substitute_variables(expr, {1,2,3,4,5,7,8,9}), {6})
  //       //   + pullback(substitute_variables(expr, {1,2,3,4,6,7,8,9}), {5})
  //       //   - pullback(substitute_variables(expr, {1,2,3,5,6,7,8,9}), {4})
  //       //   + pullback(substitute_variables(expr, {1,2,4,5,6,7,8,9}), {3})
  //       //   - pullback(substitute_variables(expr, {1,3,4,5,6,7,8,9}), {2})
  //       //   + pullback(substitute_variables(expr, {2,3,4,5,6,7,8,9}), {1})
  //       // ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     const auto subsum =
  //       + substitute_variables(expr, {1,2,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7,1})
  //       + substitute_variables(expr, {3,4,5,6,7,1,2})
  //       + substitute_variables(expr, {4,5,6,7,1,2,3})
  //       + substitute_variables(expr, {5,6,7,1,2,3,4})
  //       + substitute_variables(expr, {6,7,1,2,3,4,5})
  //       + substitute_variables(expr, {7,1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,7,6,5,4,3,2})
  //       - substitute_variables(expr, {2,1,7,6,5,4,3})
  //       - substitute_variables(expr, {3,2,1,7,6,5,4})
  //       - substitute_variables(expr, {4,3,2,1,7,6,5})
  //       - substitute_variables(expr, {5,4,3,2,1,7,6})
  //       - substitute_variables(expr, {6,5,4,3,2,1,7})
  //       - substitute_variables(expr, {7,6,5,4,3,2,1})
  //     ;
  //     const auto sum =
  //       + substitute_variables(subsum, {1,2,3,4,5,6,7})
  //       - substitute_variables(subsum, {1,2,3,4,5,6,8})
  //       + substitute_variables(subsum, {1,2,3,4,5,7,8})
  //       - substitute_variables(subsum, {1,2,3,4,6,7,8})
  //       + substitute_variables(subsum, {1,2,3,5,6,7,8})
  //       - substitute_variables(subsum, {1,2,4,5,6,7,8})
  //       + substitute_variables(subsum, {1,3,4,5,6,7,8})
  //       - substitute_variables(subsum, {2,3,4,5,6,7,8})
  //     ;
  //     // return to_lyndon_basis(sum);
  //     return std::tuple{
  //       to_lyndon_basis(sum + substitute_variables(sum, {2,3,4,5,6,7,8,1})),
  //       to_lyndon_basis(sum + substitute_variables(sum, {1,8,7,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(sum, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(sum, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(sum, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(sum, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(sum, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(sum, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(sum, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(sum, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(sum, {2,3,4,5,6,7,8,9})
  //       ),
  //       to_lyndon_basis(
  //         + pullback(substitute_variables(sum, {1,2,3,4,5,6,7,8}), {9})
  //         - pullback(substitute_variables(sum, {1,2,3,4,5,6,7,9}), {8})
  //         + pullback(substitute_variables(sum, {1,2,3,4,5,6,8,9}), {7})
  //         - pullback(substitute_variables(sum, {1,2,3,4,5,7,8,9}), {6})
  //         + pullback(substitute_variables(sum, {1,2,3,4,6,7,8,9}), {5})
  //         - pullback(substitute_variables(sum, {1,2,3,5,6,7,8,9}), {4})
  //         + pullback(substitute_variables(sum, {1,2,4,5,6,7,8,9}), {3})
  //         - pullback(substitute_variables(sum, {1,3,4,5,6,7,8,9}), {2})
  //         + pullback(substitute_variables(sum, {2,3,4,5,6,7,8,9}), {1})
  //       ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6,7}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     const auto subsum =
  //       + substitute_variables(expr, {1,2,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7,1})
  //       + substitute_variables(expr, {3,4,5,6,7,1,2})
  //       + substitute_variables(expr, {4,5,6,7,1,2,3})
  //       + substitute_variables(expr, {5,6,7,1,2,3,4})
  //       + substitute_variables(expr, {6,7,1,2,3,4,5})
  //       + substitute_variables(expr, {7,1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,7,6,5,4,3,2})
  //       - substitute_variables(expr, {2,1,7,6,5,4,3})
  //       - substitute_variables(expr, {3,2,1,7,6,5,4})
  //       - substitute_variables(expr, {4,3,2,1,7,6,5})
  //       - substitute_variables(expr, {5,4,3,2,1,7,6})
  //       - substitute_variables(expr, {6,5,4,3,2,1,7})
  //       - substitute_variables(expr, {7,6,5,4,3,2,1})
  //     ;
  //     const auto sum =
  //       + pullback(substitute_variables(subsum, {1,2,3,4,5,6,7}), {8})
  //       - pullback(substitute_variables(subsum, {1,2,3,4,5,6,8}), {7})
  //       + pullback(substitute_variables(subsum, {1,2,3,4,5,7,8}), {6})
  //       - pullback(substitute_variables(subsum, {1,2,3,4,6,7,8}), {5})
  //       + pullback(substitute_variables(subsum, {1,2,3,5,6,7,8}), {4})
  //       - pullback(substitute_variables(subsum, {1,2,4,5,6,7,8}), {3})
  //       + pullback(substitute_variables(subsum, {1,3,4,5,6,7,8}), {2})
  //       - pullback(substitute_variables(subsum, {2,3,4,5,6,7,8}), {1})
  //     ;
  //     // return to_lyndon_basis(sum);
  //     return std::tuple{
  //       to_lyndon_basis(sum + substitute_variables(sum, {2,3,4,5,6,7,8,1})),
  //       // to_lyndon_basis(sum + substitute_variables(sum, {1,8,7,6,5,4,3,2})),
  //       to_lyndon_basis(
  //         + substitute_variables(sum, {1,2,3,4,5,6,7,8})
  //         - substitute_variables(sum, {1,2,3,4,5,6,7,9})
  //         + substitute_variables(sum, {1,2,3,4,5,6,8,9})
  //         - substitute_variables(sum, {1,2,3,4,5,7,8,9})
  //         + substitute_variables(sum, {1,2,3,4,6,7,8,9})
  //         - substitute_variables(sum, {1,2,3,5,6,7,8,9})
  //         + substitute_variables(sum, {1,2,4,5,6,7,8,9})
  //         - substitute_variables(sum, {1,3,4,5,6,7,8,9})
  //         + substitute_variables(sum, {2,3,4,5,6,7,8,9})
  //       ),
  //     };
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   // CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7}),
  //   concat(
  //     CGrL_Dim4_naive_test_space(4, {1,2,3,4,5,6,7}),
  //     CGrL_Dim3_naive_test_space(4, {1,2,3,4,5,6,7})
  //   ),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     const auto subsum =
  //       + substitute_variables(expr, {1,2,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7,1})
  //       + substitute_variables(expr, {3,4,5,6,7,1,2})
  //       + substitute_variables(expr, {4,5,6,7,1,2,3})
  //       + substitute_variables(expr, {5,6,7,1,2,3,4})
  //       + substitute_variables(expr, {6,7,1,2,3,4,5})
  //       + substitute_variables(expr, {7,1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,7,6,5,4,3,2})
  //       - substitute_variables(expr, {2,1,7,6,5,4,3})
  //       - substitute_variables(expr, {3,2,1,7,6,5,4})
  //       - substitute_variables(expr, {4,3,2,1,7,6,5})
  //       - substitute_variables(expr, {5,4,3,2,1,7,6})
  //       - substitute_variables(expr, {6,5,4,3,2,1,7})
  //       - substitute_variables(expr, {7,6,5,4,3,2,1})
  //     ;
  //     if (expr.dimension() == 4) {
  //       const auto sum =
  //         + substitute_variables(subsum, {1,2,3,4,5,6,7})
  //         - substitute_variables(subsum, {1,2,3,4,5,6,8})
  //         + substitute_variables(subsum, {1,2,3,4,5,7,8})
  //         - substitute_variables(subsum, {1,2,3,4,6,7,8})
  //         + substitute_variables(subsum, {1,2,3,5,6,7,8})
  //         - substitute_variables(subsum, {1,2,4,5,6,7,8})
  //         + substitute_variables(subsum, {1,3,4,5,6,7,8})
  //         - substitute_variables(subsum, {2,3,4,5,6,7,8})
  //       ;
  //       return to_lyndon_basis(sum);
  //     } else {
  //       CHECK_EQ(expr.dimension(), 3);
  //       const auto sum =
  //         + pullback(substitute_variables(subsum, {1,2,3,4,5,6,7}), {8})
  //         - pullback(substitute_variables(subsum, {1,2,3,4,5,6,8}), {7})
  //         + pullback(substitute_variables(subsum, {1,2,3,4,5,7,8}), {6})
  //         - pullback(substitute_variables(subsum, {1,2,3,4,6,7,8}), {5})
  //         + pullback(substitute_variables(subsum, {1,2,3,5,6,7,8}), {4})
  //         - pullback(substitute_variables(subsum, {1,2,4,5,6,7,8}), {3})
  //         + pullback(substitute_variables(subsum, {1,3,4,5,6,7,8}), {2})
  //         - pullback(substitute_variables(subsum, {2,3,4,5,6,7,8}), {1})
  //       ;
  //       return to_lyndon_basis(sum);
  //     }
  //   }
  // )) << "\n";


  // std::cout << to_string(space_mapping_ranks(
  //   CL4({1,2,3,4,5,6}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return to_lyndon_basis(
  //       + substitute_variables(expr, {1,2,3,4,5,6})
  //       - substitute_variables(expr, {1,2,3,4,5,7})
  //       + substitute_variables(expr, {1,2,3,4,6,7})
  //       - substitute_variables(expr, {1,2,3,5,6,7})
  //       + substitute_variables(expr, {1,2,4,5,6,7})
  //       - substitute_variables(expr, {1,3,4,5,6,7})
  //       + substitute_variables(expr, {2,3,4,5,6,7})
  //     );
  //   }
  // )) << "\n";

  // std::cout << to_string(space_mapping_ranks(
  //   CL4({1,2,3,4,5}),
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return to_lyndon_basis(
  //       + substitute_variables(expr, {1,2,3,4,5})
  //       - substitute_variables(expr, {1,2,3,4,6})
  //       + substitute_variables(expr, {1,2,3,5,6})
  //       - substitute_variables(expr, {1,2,4,5,6})
  //       + substitute_variables(expr, {1,3,4,5,6})
  //       - substitute_variables(expr, {2,3,4,5,6})
  //     );
  //   }
  // )) << "\n";


  // for (const int weight : range_incl(2, 4)) {
  //   for (const int num_points : range_incl(4, 7)) {
  //     const auto points = to_vector(range_incl(1, num_points));
  //     const auto ranks = space_mapping_ranks(
  //       concat(
  //         CGrL_test_space(weight, 2, points),
  //         CGrL_test_space(weight, 3, points),
  //         CGrL_test_space(weight, 4, points),
  //         CGrL_test_space(weight, 5, points),
  //         CGrL_test_space(weight, 6, points)
  //       ),
  //       DISAMBIGUATE(to_lyndon_basis),
  //       [&](const auto& expr) {
  //         // Note. This is actually a condition on complex cohomology, which is way more complicated
  //         //   in theory, but for practical intents and purposes we can do this since each element
  //         //   lies on one the source space bases.
  //         const auto q = GammaExpr();
  //         // const auto q = to_lyndon_basis(symmetrize_double(expr, num_points));
  //         // const auto x = to_lyndon_basis(symmetrize_loop(chern_arrow_left(expr, num_points + 1), num_points + 1));
  //         // const auto y = to_lyndon_basis(symmetrize_loop(chern_arrow_up(expr, num_points + 1), num_points + 1));
  //         const auto x = to_lyndon_basis(chern_arrow_left(expr, num_points + 1));
  //         const auto y = to_lyndon_basis(chern_arrow_up(expr, num_points + 1));
  //         const auto z = GammaExpr();
  //         switch (expr.dimension()) {
  //           // case 2: return std::tuple{x, y, z, z};
  //           // case 3: return std::tuple{z, x, y, z};
  //           // case 4: return std::tuple{z, z, x, y};
  //           // case 2: return std::tuple{x, y, z, z, z};
  //           // case 3: return std::tuple{z, x, y, z, z};
  //           // case 4: return std::tuple{z, z, x, y, z};
  //           // case 5: return std::tuple{z, z, z, x, y};
  //           case 2: return std::tuple{x, y, z, z, z, z,  q, z, z, z, z};
  //           case 3: return std::tuple{z, x, y, z, z, z,  z, q, z, z, z};
  //           case 4: return std::tuple{z, z, x, y, z, z,  z, z, q, z, z};
  //           case 5: return std::tuple{z, z, z, x, y, z,  z, z, z, q, z};
  //           case 6: return std::tuple{z, z, z, z, x, y,  z, z, z, z, q};
  //           default: FATAL("Unexpected dimension");
  //         };
  //       }
  //     );
  //     std::cout << "w=" << weight << ", n=" << num_points << ":  " << to_string(ranks) << "\n";
  //   }
  // }


  // TablePrinter t;
  // // for (const int weight : {3}) {
  // //   for (const int n : {3}) {
  // for (const int weight : range_incl(2, 6)) {
  //   for (const int n : range_incl(1, 8)) {
  //     const int fixed_a = 1;
  //     const int fixed_b = 2;
  //     const int nonfixed_start = 3;
  //     PolylogSpace space;
  //     for (const auto& seq : nondecreasing_sequences(n, weight)) {
  //       const auto symbol = tensor_product(absl::MakeConstSpan(mapped(seq, [](const int k) {
  //         return D(k + nonfixed_start, fixed_a) - D(fixed_b, fixed_a);
  //       })));
  //       // std::cout << to_lyndon_basis(symbol);
  //       space.push_back(symbol);
  //       // space.push_back(to_lyndon_basis(symbol));
  //     }
  //     // const auto ranks = space_mapping_ranks(
  //     //   space,
  //     //   DISAMBIGUATE(identity_function),
  //     //   [](const auto& expr) {
  //     //     return to_lyndon_basis(project_on(fixed_b, expr));
  //     //   }
  //     // );
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(identity_function),
  //       [](const auto& expr) {
  //         return to_lyndon_basis(expr.filtered([&](const std::vector<Delta>& term) {
  //           // return count_var(term, fixed_b) > 0;
  //           return count_var(term, fixed_b) == 1;
  //           // return true;
  //         }));
  //       }
  //     );
  //     // std::cout << "w=" << weight << ", n=" << n << ": " << to_string(ranks) << "\n";
  //     // t.set_content({n - 1, weight - 2}, to_string(ranks.kernel()));
  //     const int expected = sum(mapped(range_incl(0, weight), [&](const int k) { return binomial(n-1, k); }));
  //     t.set_content(
  //       {n - 1, weight - 2},
  //       // absl::StrCat(ranks.kernel(), "~", expected)
  //       absl::StrCat(ranks.kernel())
  //     );
  //     CHECK_EQ(ranks.kernel(), expected);
  //   }
  // }
  // std::cout << t.to_string();


  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto ranks = space_mapping_ranks(
  //     mapped(GrL3(3, points), DISAMBIGUATE(to_lyndon_basis)),
  //     DISAMBIGUATE(identity_function),
  //     [](const auto& expr) {
  //       return project_on(1, expr);
  //     }
  //   );
  //   const auto lower_rank = space_rank(GrL3(3, slice(points, 1)), DISAMBIGUATE(to_lyndon_basis));
  //   std::cout << "p=" << num_points << ":  " << to_string(ranks) << "  vs  " << lower_rank << "\n";
  // }


  // const auto triplet_tmpl =
  //   + GammaExpr::single({Gamma({1,2}), Gamma({1,3})})
  //   + GammaExpr::single({Gamma({1,3}), Gamma({1,4})})
  //   + GammaExpr::single({Gamma({1,4}), Gamma({1,2})})
  // ;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // for (const int num_points : range_incl(4, 8)) {
  //   const int dimension = 3;
  //   const int weight = 3;
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto coords = combinations(points, dimension);
  //   GrPolylogNCoSpace space_comult;
  //   for (const auto& word : get_lyndon_words(coords, weight)) {
  //     const auto expr = GammaExpr::single(mapped(word, convert_to<Gamma>));
  //     const auto expr_pr = pr(expr);
  //     if (!expr_pr.is_zero()) {
  //       space_comult.push_back(ncomultiply(expr_pr, {1,2}));
  //     }
  //   }
  //   GrPolylogSpace space_triplets;
  //   for (const auto& indices : permutations(slice(points, 1), 4)) {
  //     space_triplets.push_back(substitute_variables(triplet_tmpl, indices));
  //   }
  //   GrPolylogSpace fx = mapped(GrFx(dimension, points), pr);
  //   GrPolylogNCoSpace second_space = mapped(cartesian_product(space_triplets, fx), applied(DISAMBIGUATE(ncoproduct)));
  //   // TODO: Space coproduct helper function !!!
  //   const auto ranks = space_venn_ranks(
  //     space_comult,
  //     second_space,
  //     DISAMBIGUATE(identity_function)
  //   );
  //   std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  // }


  // const int dimension = 3;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto ranks = space_venn_ranks(
  //     mapped(GrFx(dimension, points), pr),
  //     mapped(GrL1(dimension, points), pr),
  //     DISAMBIGUATE(to_lyndon_basis)
  //   );
  //   std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  // }

  // const int dimension = 3;
  // const auto pr = [](const auto& expr) {
  //   return project_on(1, expr);
  // };
  // const int weight = 2;
  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   GrPolylogSpace l2 = mapped(GrL2(dimension, points), pr);
  //   GrPolylogSpace l1 = mapped(GrFx(dimension, points), pr);
  //   // GrPolylogSpace l2 = GrL2(dimension, points);
  //   // GrPolylogSpace l1 = GrL1(dimension, points);
  //   GrPolylogNCoSpace space = mapped(
  //     cartesian_combinations(std::vector{std::pair{l2, 1}, std::pair{l1, weight-2}}),
  //     DISAMBIGUATE(ncoproduct_vec)
  //   );
  //   const auto ranks = space_mapping_ranks(
  //     space,
  //     DISAMBIGUATE(identity_function),
  //     DISAMBIGUATE(ncomultiply)
  //   );
  //   // const auto lambda3_rank = space_rank(
  //   //   mapped(combinations(l1, 3), DISAMBIGUATE(ncoproduct_vec)),
  //   //   DISAMBIGUATE(to_lyndon_basis)
  //   // );
  //   const int l1_rank = space_rank(l1, DISAMBIGUATE(to_lyndon_basis));
  //   const int lambda3_l1_rank = binomial(l1_rank, weight);
  //   // std::cout << "p=" << num_points << ": " << to_string(ranks) << "\n";
  //   std::cout << "p=" << num_points << ": " << lambda3_l1_rank << " - " << ranks.image();
  //   std::cout << " = " << (lambda3_l1_rank - ranks.image()) << "\n";
  // }

  // for (const int num_points : range_incl(4, 8)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const int rank = space_rank(mapped(GrFx(dimension, points), pr), DISAMBIGUATE(to_lyndon_basis));
  //   std::cout << "p=" << num_points << ": " << rank << "\n";
  // }


  // std::cout << space_rank(GrL4_Dim3({1,2,3,4,5,6,7}), DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // std::cout << space_rank(
  //   mapped(GrL4_Dim3({1,2,3,4,5,6,7}), [](const auto& expr) {
  //     return project_on(7, expr);
  //   }),
  //   DISAMBIGUATE(to_lyndon_basis)
  // ) << "\n";

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6,7};
  // const auto space = mapped(
  //   concat(
  //     cartesian_combinations(std::vector{std::pair{GrL3(dimension, points), 1}, std::pair{GrL1(dimension, points), 1}}),
  //     cartesian_combinations(std::vector{std::pair{GrL2(dimension, points), 2}})
  //   ),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return ncomultiply(expr);
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6};
  // const auto cl1 = CGrL_test_space(1, dimension, points);
  // const auto cl2 = CGrL_test_space(2, dimension, points);
  // const auto cl3 = CGrL_test_space(3, dimension, points);
  // const auto space = mapped(
  //   concat(
  //     cartesian_combinations(std::vector{std::pair{cl3, 1}, std::pair{cl1, 1}}),
  //     cartesian_combinations(std::vector{std::pair{cl2, 2}})
  //   ),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return std::tuple{ncomultiply(expr), keep_non_weakly_separated(expr)};
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6,7};
  // const auto space = mapped(
  //   cartesian_combinations(std::vector{std::pair{GrL2(dimension, points), 1}, std::pair{GrL1(dimension, points), 2}}),
  //   [](const auto& exprs) {
  //     return ncoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_mapping_ranks(
  //   space,
  //   DISAMBIGUATE(to_lyndon_basis),
  //   [](const auto& expr) {
  //     return ncomultiply(expr);
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const int weight = 4;
  // const int dimension = 3;
  // const std::vector points = {1,2,3,4,5,6};
  // const auto coords = combinations(slice(points, 1), dimension - 1);
  // GrPolylogACoSpace space_words = mapped(
  //   gr_free_lie_coalgebra(weight, dimension, points),
  //   DISAMBIGUATE(expand_into_glued_pairs)
  // );
  // const auto space_l = mapped(
  //   cartesian_combinations(std::vector{
  //     std::pair{GrL2(dimension, points), 1},
  //     std::pair{GrL1(dimension, points), 1},
  //     std::pair{GrL1(dimension, points), 1},
  //   }),
  //   [](const auto& exprs) {
  //     return acoproduct_vec(mapped(exprs, [](const auto& e) {
  //       return project_on(1, e);
  //     }));
  //   }
  // );
  // const auto ranks = space_venn_ranks(
  //   space_words,
  //   space_l,
  //   DISAMBIGUATE(identity_function)
  // );
  // std::cout << to_string(ranks) << "\n";


  // const int num_points = 6;
  // const auto points = to_vector(range_incl(1, num_points));
  // const auto ranks = space_mapping_ranks(CL4(points), DISAMBIGUATE(to_lyndon_basis), [](const auto& expr) {
  //   return std::tuple{
  //     to_lyndon_basis(substitute_variables(expr, {1,1,3,4,5,6})),
  //     to_lyndon_basis(substitute_variables(expr, {1,2,2,4,5,6})),
  //     to_lyndon_basis(substitute_variables(expr, {1,2,3,3,5,6})),
  //     to_lyndon_basis(substitute_variables(expr, {1,2,3,4,4,6})),
  //     to_lyndon_basis(substitute_variables(expr, {1,2,3,4,5,5})),
  //     to_lyndon_basis(substitute_variables(expr, {6,2,3,4,5,6})),
  //   };
  // });
  // std::cout << to_string(ranks) << "\n";


  // for (const int half_num_points : range_incl(2, 6)) {
  //   const auto points = to_vector(range_incl(1, half_num_points));
  //   Profiler profiler;
  //   auto cl1 = CL1_inv(points);
  //   auto cl2 = CL2_inv(points);
  //   profiler.finish("cl1/cl2 spaces");
  //   cl1 = space_basis(cl1, DISAMBIGUATE(to_lyndon_basis));
  //   cl2 = space_basis(cl2, DISAMBIGUATE(to_lyndon_basis));
  //   profiler.finish("cl1/cl2 bases");
  //   const auto space_a = mapped(
  //     get_lyndon_words(cl1, 4),
  //     [](const auto& components) {
  //       return expand_into_glued_pairs(tensor_product(absl::MakeConstSpan(components)));
  //     }
  //   );
  //   profiler.finish("space_a");
  //   const auto space_b = mapped(
  //     cartesian_product(cl2, cl1, cl1),
  //     applied(DISAMBIGUATE(acoproduct))
  //   );
  //   profiler.finish("space_b");
  //   const auto ranks = space_venn_ranks(space_a, space_b, DISAMBIGUATE(identity_function));
  //   profiler.finish("ranks");
  //   std::cout << "n=" << (half_num_points*2) << ": " << to_string(ranks) << "\n";
  // }

  // for (const int weight : range_incl(2, 4)) {
  // // for (const int weight : {4}) {
  //   for (const int half_num_points : range_incl(2, 4)) {
  //     const auto points_raw = to_vector(range_incl(1, half_num_points));
  //     const auto& points_inv = concat(
  //       mapped(points_raw, [](const int idx) { return X(idx); }),
  //       mapped(points_raw, [](const int idx) { return -X(idx); })
  //     );
  //     Profiler profiler;
  //     const auto space = mapped(L(weight, points_inv), DISAMBIGUATE(to_lyndon_basis));
  //     // profiler.finish("space");
  //     const auto ranks = space_mapping_ranks(space, DISAMBIGUATE(identity_function), [](const auto& expr) {
  //       return std::tuple{
  //         keep_non_weakly_separated_inv(expr),
  //         // ncomultiply(expr, {2,2}),
  //       };
  //     });
  //     // profiler.finish("ranks");
  //     std::cout << "w=" << weight << ", n=" << points_inv.size() << ": ";
  //     std::cout << to_string(ranks) << "\n";
  //   }
  // }


#if 0
  const std::vector points_inv = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // const auto space_a = mapped(CL4(points_inv), DISAMBIGUATE(ncoproduct));
  auto space_a = CL4(points_inv);
  for (const int i : range(points_inv.size())) {
    auto p = points_inv;
    p[i] = Inf;
    append_vector(space_a, CL4(p));
  }

  // const auto space_b = mapped(cartesian_power(L2({x1,x2,x3,x4,-x1}), 2), DISAMBIGUATE(ncoproduct_vec));
  // const auto space_b = mapped(cartesian_power(L2({x1,x2,x3,x4,-x1,-x2,-x3,-x4}), 2), DISAMBIGUATE(ncoproduct_vec));
  const auto new_func_comult =
    + ncoproduct(QLi2(x1,x2,x3,x4), QLi2(x4,-x1,-x4,x1))
    - ncoproduct(QLi2(x2,x3,x4,-x1), QLi2(-x1,-x2,x1,x2))
    + ncoproduct(QLi2(x3,x4,-x1,-x2), QLi2(-x2,-x3,x2,x3))
    - ncoproduct(QLi2(x4,-x1,-x2,-x3), QLi2(-x3,-x4,x3,x4))
  ;
  // const std::vector space_b = {new_func_comult};

  // const auto ranks = space_mapping_ranks(
  //   concat(
  //     space_a,
  //     space_b
  //   ),
  //   // space_a,
  //   DISAMBIGUATE(identity_function),
  //   [](const auto& expr) {
  //     if (expr.is_zero()) {
  //       return std::tuple{
  //         DeltaNCoExpr(),
  //         DeltaNCoExpr(),
  //       };
  //     }
  //     const int num_coparts = expr.element().first.size();  // TODO: add a helper function for this
  //     if (num_coparts == 1) {
  //       return std::tuple{
  //         keep_non_weakly_separated_inv(expr),
  //         ncomultiply(expr, {2,2}),
  //       };
  //     } else if (num_coparts == 2) {
  //       return std::tuple{
  //         DeltaNCoExpr(),
  //         -expr,
  //       };
  //     } else {
  //       FATAL(absl::StrCat("Unexpected num_coparts: ", num_coparts));
  //     }
  //   }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const auto ranks = space_mapping_ranks(
  //   mapped(space_a, DISAMBIGUATE(to_lyndon_basis)),
  //   DISAMBIGUATE(identity_function),
  //   DISAMBIGUATE(keep_non_weakly_separated_inv)
  //   // [&](const auto& expr) {
  //   //   return std::tuple{
  //   //     keep_non_weakly_separated_inv(expr),
  //   //     ncomultiply(expr, {2,2}) - new_func_comult,
  //   //   };
  //   // }
  // );
  // std::cout << to_string(ranks) << "\n";

  // const auto ranks = space_venn_ranks(
  //   mapped(space_a, [](const auto& expr) { return ncomultiply(expr, {2,2}); }),
  //   space_b,
  //   DISAMBIGUATE(identity_function)
  // );
  // std::cout << to_string(ranks) << "\n";


  // CHECK(is_totally_weakly_separated_inv(QLi4(x1,x2,x3,x4,-x1,Inf)));
  // std::cout << is_totally_weakly_separated_inv(QLi4(x1,x2,x3,x4,Inf,-x2)) << "\n";
  const std::vector space = {
    QLi4(x1,x2,x3,x4,-x1,Inf),
    QLi4(x2,x3,x4,-x1,-x2,Inf),
    QLi4(x3,x4,-x1,-x2,-x3,Inf),
    QLi4(x4,-x1,-x2,-x3,-x4,Inf),
    QLi4(x1,x2,Inf,-x1,-x2,-x3),
    // QLi4(x1,x2,x3,x4,Inf,-x1),
    // QLi4(x2,x3,x4,-x1,Inf,-x2),
    // QLi4(x3,x4,-x1,-x2,Inf,-x3),
    // QLi4(x4,-x1,-x2,-x3,Inf,-x4),
    // QLi4(x1,x2,x3,Inf,x4,-x1),
    // QLi4(x2,x3,x4,Inf,-x1,-x2),
    // QLi4(x3,x4,-x1,Inf,-x2,-x3),
    // QLi4(x4,-x1,-x2,Inf,-x3,-x4),
  };
  // for (const auto& expr : space) {
  //   CHECK(is_totally_weakly_separated_inv(expr));
  // }
  const auto ranks = space_mapping_ranks(
    mapped(space, DISAMBIGUATE(to_lyndon_basis)),
    DISAMBIGUATE(identity_function),
    [&](const auto& expr) {
      return std::tuple{
        // keep_non_weakly_separated_inv(expr),
        ncomultiply(expr, {2,2}) - new_func_comult,
      };
    }
  );
  std::cout << to_string(ranks) << "\n";
#endif

  // const auto ranks = space_venn_ranks(
  //   space_a,
  //   // space,
  //   {QLi4(x1,x2,Inf,-x1,-x2,-x3)},
  //   DISAMBIGUATE(to_lyndon_basis)
  // );
  // std::cout << to_string(ranks) << "\n";


  // const std::vector space = {
  //   + supervremenno(x1,x2,x3,x4)
  //   - supervremenno(x1,x2,x3,x5)
  //   + supervremenno(x1,x2,x4,x5)
  //   - supervremenno(x1,x3,x4,x5)
  //   + supervremenno(x2,x3,x4,x5),
  //   + ncomultiply(QLi4(x1,x2,x3,x4,x5,-x1), {2,2})
  //   + ncomultiply(QLi4(x2,x3,x4,x5,-x1,-x2), {2,2})
  //   + ncomultiply(QLi4(x3,x4,x5,-x1,-x2,-x3), {2,2})
  //   + ncomultiply(QLi4(x4,x5,-x1,-x2,-x3,-x4), {2,2})
  //   + ncomultiply(QLi4(x5,-x1,-x2,-x3,-x4,-x5), {2,2})
  // };
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // std::cout << to_lyndon_basis(
  //   + supervremenno(x1,x2,x3,x4)
  //   - supervremenno(x1,x2,x3,x5)
  //   + supervremenno(x1,x2,x4,x5)
  //   - supervremenno(x1,x3,x4,x5)
  //   + supervremenno(x2,x3,x4,x5)
  // );
  // std::cout << to_lyndon_basis(
  //   + ncomultiply(QLi4(x1,x2,x3,x4,x5,-x1), {2,2})
  //   + ncomultiply(QLi4(x2,x3,x4,x5,-x1,-x2), {2,2})
  //   + ncomultiply(QLi4(x3,x4,x5,-x1,-x2,-x3), {2,2})
  //   + ncomultiply(QLi4(x4,x5,-x1,-x2,-x3,-x4), {2,2})
  //   + ncomultiply(QLi4(x5,-x1,-x2,-x3,-x4,-x5), {2,2})
  // );

  // std::cout << to_lyndon_basis(
  //   +  supervremenno(x1,x2,x3,x4)
  //   -  supervremenno(x1,x2,x3,x5)
  //   +  supervremenno(x1,x2,x4,x5)
  //   -  supervremenno(x1,x3,x4,x5)
  //   +  supervremenno(x2,x3,x4,x5)
  //   // -2*ncomultiply(QLi4(x1,x2,x3,x4,x5,-x1), {2,2})
  //   // -2*ncomultiply(QLi4(x2,x3,x4,x5,-x1,-x2), {2,2})
  //   // -2*ncomultiply(QLi4(x3,x4,x5,-x1,-x2,-x3), {2,2})
  //   // -2*ncomultiply(QLi4(x4,x5,-x1,-x2,-x3,-x4), {2,2})
  //   // -2*ncomultiply(QLi4(x5,-x1,-x2,-x3,-x4,-x5), {2,2})
  //   -2*QLi4_Comult_2_2(x1,x2,x3,x4,x5,-x1)
  //   -2*QLi4_Comult_2_2(x2,x3,x4,x5,-x1,-x2)
  //   -2*QLi4_Comult_2_2(x3,x4,x5,-x1,-x2,-x3)
  //   -2*QLi4_Comult_2_2(x4,x5,-x1,-x2,-x3,-x4)
  //   -2*QLi4_Comult_2_2(x5,-x1,-x2,-x3,-x4,-x5)
  // );

  // std::cout << to_string(space_mapping_ranks(
  //   mapped(cartesian_power(L2({x1,x2,x3,x4,-x1,-x2,-x3,-x4}), 2), DISAMBIGUATE(ncoproduct_vec)),
  //   DISAMBIGUATE(identity_function),
  //   DISAMBIGUATE(keep_non_weakly_separated_inv)
  // )) << "\n";

  // auto expr = QLi4(x1,x2,x3,x4,-x1,-x2,-x3,-x4);
  // expr = to_lyndon_basis(expr);
  // std::cout << keep_non_weakly_separated_inv(expr) << "\n";

  // for (const int weight : range_incl(2, 4)) {
  //   for (const int half_num_points : range_incl(2, 6)) {
  //     const auto points_raw = to_vector(range_incl(1, half_num_points));
  //     const auto& points_inv = concat(
  //       mapped(points_raw, [](const int idx) { return X(idx); }),
  //       mapped(points_raw, [](const int idx) { return -X(idx); })
  //     );
  //     const int rank = space_rank(CLC(weight, points_inv), DISAMBIGUATE(to_lyndon_basis));
  //     std::cout << "w=" << weight << ", n=" << points_inv.size() << ": " << rank << "\n";
  //   }
  // }



  const int num_points = 9;
  auto source = sum_looped_vec(
    [&](const auto& args) {
      return LiQuad(num_points / 2 - 1, args);
    },
    num_points,
    to_vector(range_incl(1, num_points - 1))
  );

  auto expr = theta_expr_to_lira_expr_without_products(source.without_annotations());

  constexpr char kInvalidInput[] = "Invalid input: ";
  bool short_form_ratios = true;
  std::cout << "Functional\n" << source.annotations() << "\n";
  while (true) {
    std::vector<std::vector<int>> balls;
    std::unique_ptr<Snowpal> snowpal;
    auto reset_snowpal = [&]() {
      snowpal = absl::make_unique<Snowpal>(expr, num_points);
      for (const auto& b : balls) {
        snowpal->add_ball(b);
      }
    };
    reset_snowpal();
    std::cout << expr;
    while (true) {
      std::cout << "\n> ";
      std::string input;
      std::getline(std::cin, input);
      trim(input);
      if (input.empty()) {
        continue;
      } else if (input == "q" || input == "quit") {
        return 0;
      } else if (input == "sf" || input == "short_forms") {
        short_form_ratios = !short_form_ratios;
        if (short_form_ratios) {
          std::cout << "Short form cross-ratios: enabled\n";
        } else {
          std::cout << "Short form cross-ratios: disabled\n";
        }
        to_ostream(std::cout, *snowpal, short_form_ratios);
        continue;
      } else if (input == "r" || input == "reset") {
        break;
      } else if (input == "b" || input == "back") {
        if (balls.empty()) {
          std::cout << kInvalidInput << "nothing to remove\n";
          continue;
        }
        balls.pop_back();
        reset_snowpal();
        to_ostream(std::cout, *snowpal, short_form_ratios);
        continue;
      }
      std::vector<int> ball;
      try {
        for (const char ch : input) {
          if (std::isspace(ch)) {
            continue;
          }
          const int var = std::stoi(std::string(1, ch));
          if (var < 1 || var > num_points) {
            throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
          }
          ball.push_back(var);
        }
      } catch (const std::exception& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        continue;
      }
      try {
        snowpal->add_ball(ball);
        balls.push_back(ball);
        to_ostream(std::cout, *snowpal, short_form_ratios);
      } catch (const IllegalTreeCutException& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        reset_snowpal();
      }
    }
  }
}
