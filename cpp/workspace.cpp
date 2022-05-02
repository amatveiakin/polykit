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
#include "lib/kappa.h"
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

// TODO: Split DeltaExpr into the one with minuses and without:
//   without: type A
//   with (point in involution): type C
//   Q. what to do about `Zero`?

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


TypeDPolylogSpace type_d_free_lie_coalgebra(int weight) {
  const auto coords = concat(
    mapped(combinations({1,2,3,4,5,6}, 3), [](const auto& points) {
      return Kappa(Gamma(points));
    }),
    std::vector{ Kappa(KappaX{}), Kappa(KappaY{}) }
  );
  return mapped(get_lyndon_words(coords, weight), [](const auto& word) {
    return KappaExpr::single(word);
  });
}



// TODO: Factor out
inline int mod_eq(int a, int b, int mod) {
  return pos_mod(a, mod) == pos_mod(b, mod);
}

// ProjectionExpr alt_project_on(int axis, const DeltaExpr& expr, int num_vars) {
//   return project_on(axis, expr).filtered([&](const auto& term) {
//     return !absl::c_any_of(term, [&](const X& x) {
//       const int idx = x.as_simple_var();
//       return (
//         pos_mod(idx - 1, num_vars) == pos_mod(axis, num_vars)
//         // pos_mod(idx - 1, num_vars) == pos_mod(axis, num_vars) ||
//         // pos_mod(idx + 1, num_vars) == pos_mod(axis, num_vars)
//       );
//     });
//   });
// }

int var_sign(X x) {
  switch (x.form()) {
    case XForm::var: return 1;
    case XForm::neg_var: return -1;
    default: FATAL(absl::StrCat("Unexpected form: ", to_string(x)));
  }
}

// bool is_frozen_coord(const Delta& d, int num_vars) {
//   const auto [a, b] = delta_points_inv(d);
//   const bool same_sign = var_sign(a) == var_sign(b);
//   const int diff = std::abs(a.idx() - b.idx());
//   return (
//     (mod_eq(diff, 1, num_vars) && same_sign) ||
//     (mod_eq(diff, -1, num_vars) && !same_sign)
//   );
// }

bool is_frozen_coord(const Delta& d, int num_vars) {  // TODO: !!!
  CHECK_EQ(num_vars, 4);
  return d == Delta(x1,x2) || d == Delta(x2,x3) || d == Delta(x3,x4);
  // return d == Delta(x1,x2) || d == Delta(x2,x3);
  // return d == Delta(x1,x2);
  // return false;
}

DeltaExpr alt_project_on(const Delta& axis, const DeltaExpr& expr, int num_vars) {
  return expr.filtered([&](const auto& term) {
    return absl::c_all_of(term, [&](const Delta& d) {
      return !are_weakly_separated_inv(d, axis) || is_frozen_coord(d, num_vars);
    });
  });
}


// TODO: Factor out
// TODO: Fast pow
int int_pow(int x, int p) {
  int ret = 1;
  for (EACH: range(p)) {
    ret *= x;
  }
  return ret;
}

DeltaExpr typeC_QLi_arg4(int weight, const XArgs& args) {
  const auto& points = args.as_x();
  CHECK_EQ(points.size(), 4);
  CHECK_GE(weight, 2);
  return QLiVec(weight, args).dived_int(int_pow(2, weight-1)).without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("typeC_QLi"), {weight}),
      points
    )
  );
}

DeltaExpr typeC_QLi_arg8(int weight, const XArgs& args) {
  CHECK(inv_points_are_central_symmetric(args));
  const auto& points = args.as_x();
  CHECK_EQ(points.size(), 8);
  CHECK_GE(weight, 2);
  const int w = weight - 1;
  auto expr =
    + tensor_product(
      QLiVec   (1, choose_indices_one_based(points, std::vector{1,4,5,8})),
      QLiVec   (w, choose_indices_one_based(points, std::vector{1,2,3,4}))
    )
    - tensor_product(
      QLiNegVec(1, choose_indices_one_based(points, std::vector{2,5,6,1})),
      QLiNegVec(w, choose_indices_one_based(points, std::vector{2,3,4,5}))
    )
    + tensor_product(
      QLiVec   (1, choose_indices_one_based(points, std::vector{3,6,7,2})),
      QLiVec   (w, choose_indices_one_based(points, std::vector{3,4,5,6}))
    )
    - tensor_product(
      QLiNegVec(1, choose_indices_one_based(points, std::vector{4,7,8,3})),
      QLiNegVec(w, choose_indices_one_based(points, std::vector{4,5,6,7}))
    )
  ;
  if (weight > 2) {
    expr += tensor_product(cross_ratio(points), typeC_QLi_arg8(w, points)).dived_int(2);
  }
  return expr.without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("typeC_QLi"), {weight}),
      points
    )
  );
}

DeltaExpr typeC_QLi(int weight, const XArgs& args) {
  switch (args.size()) {
    case 4: return typeC_QLi_arg4(weight, args);
    case 8: return typeC_QLi_arg8(weight, args);
    default: FATAL("Unsupported number of arguments");
  }
}


// DeltaExpr typeC_QLiSymm4(const XArgs& args) {
//   const int weight = 4;
//   const auto& points = args.as_x();
//   CHECK_EQ(points.size(), 8);
//   return (
//     + typeC_QLi(4, points)
//     + (
//       + QLiVec(4, choose_indices_one_based(points, {1,2,3,4}))
//       + QLiVec(4, choose_indices_one_based(points, {2,3,4,5}))
//       + QLiVec(4, choose_indices_one_based(points, {3,4,5,6}))
//       + QLiVec(4, choose_indices_one_based(points, {4,5,6,7}))
//       + QLiVec(4, choose_indices_one_based(points, {5,6,7,8}))
//       + QLiVec(4, choose_indices_one_based(points, {6,7,8,1}))
//       + QLiVec(4, choose_indices_one_based(points, {7,8,1,2}))
//       + QLiVec(4, choose_indices_one_based(points, {8,1,2,3}))
//     ) * 2
//     + (
//       + QLiVec(4, choose_indices_one_based(points, {1,2,5,6}))
//       + QLiVec(4, choose_indices_one_based(points, {2,3,6,7}))
//       + QLiVec(4, choose_indices_one_based(points, {3,4,7,8}))
//       + QLiVec(4, choose_indices_one_based(points, {4,5,8,1}))
//     ).dived_int(4)
//   ).without_annotations().annotate(
//     fmt::function_num_args(
//       fmt::sub_num(fmt::opname("typeC_QLiSymm"), {weight}),
//       points
//     )
//   );
// }

DeltaExpr typeC_QLiSymm(int weight, const XArgs& args) {
  if (args.size() == 4) {
    return typeC_QLi(weight, args);
  }
  const auto& points = args.as_x();
  CHECK_EQ(points.size(), 8);
  return (
    + typeC_QLi(weight, points)
    + (
      + QLiVec(weight, choose_indices_one_based(points, {1,2,3,4}))
      - QLiVec(weight, choose_indices_one_based(points, {3,4,5,2}))
      + QLiVec(weight, choose_indices_one_based(points, {3,4,5,6}))
      - QLiVec(weight, choose_indices_one_based(points, {5,6,7,4}))
    )
    + (
      + typeC_QLi(weight, choose_indices_one_based(points, {1,2,5,6}))
      + typeC_QLi(weight, choose_indices_one_based(points, {3,4,7,8}))
    )
  ).without_annotations().annotate(
    fmt::function_num_args(
      fmt::sub_num(fmt::opname("typeC_QLiSymm"), {weight}),
      points
    )
  );
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
    // .set_expression_line_limit(30)
    // .set_annotation_sorting(AnnotationSorting::length)
    .set_annotation_sorting(AnnotationSorting::lexicographic)
    .set_compact_x(true)
  );


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
  //     const int rank = space_rank(typeC_CL(weight, points_inv), DISAMBIGUATE(to_lyndon_basis));
  //     std::cout << "w=" << weight << ", n=" << points_inv.size() << ": " << rank << "\n";
  //   }
  // }


#if 0
  // constexpr int dimension = 3;
  constexpr int num_points = 6;
  // const auto points = to_vector(range_incl(1, num_points));

  static constexpr auto kappa_y_to_x = [](const KappaExpr& expr) {
    return expr.mapped([&](const auto& term) {
      return mapped(term, [&](const Kappa& k) {
        return std::visit(overloaded {
          [&](const KappaX&) -> Kappa { return KappaY{}; },
          [&](const KappaY&) -> Kappa { return KappaX{}; },
          [&](const Gamma& g) -> Kappa { return g; }
        }, k);
      });
    });
  };
  static constexpr auto cycle_indices = [](const KappaExpr& expr, int shift) {
    return expr.mapped([&](const auto& term) {
      return mapped(term, [&](const Kappa& k) {
        return std::visit(overloaded {
          [&](const KappaX&) -> Kappa { return KappaX{}; },
          [&](const KappaY&) -> Kappa { return KappaY{}; },
          [&](const Gamma& g) -> Kappa {
            return Gamma(mapped(g.index_vector(), [&](const int idx) {
              return pos_mod(idx + shift - 1, num_points) + 1;
            }));
          },
        }, k);
      });
    });
  };
  static constexpr auto n_log = [](int weight, const std::array<KappaExpr, 3>& triple) {
    const auto &[a, b, c] = triple;
    CHECK_GE(weight, 2);
    KappaExpr ret = tensor_product(a - c, b - c);
    for (EACH : range(weight - 2)) {
      ret = tensor_product(b - c, ret);
    }
    return ret;
  };

  const std::vector<std::vector<KappaExpr>> b2_generators_y_cyclable = {
    {
      K(1,3,4) + K_Y(),
      K(1,3,6) + K(1,4,5) + K(2,3,4),
      K(1,4,6) + K(3,4,5) + K(1,2,3),
    },
    {
      K(3,4,6) + K_Y(),
      K(1,4,6) + K(2,3,6) + K(3,4,5),
      K(1,3,6) + K(2,3,4) + K(4,5,6),
    },
    {
      K(2,3,6) + K(1,4,5),
      K_Y(),
      K(1,2,3) + K(4,5,6),
    },
  };
  const std::vector<std::vector<KappaExpr>> b2_generators_y_fixed = {
    {
      K(1,3,5) + K_Y(),
      K(1,2,3) + K(3,4,5) + K(1,5,6),
      K(1,3,6) + K(2,3,5) + K(1,4,5),
    },
    {
      K(2,4,6) + K_Y(),
      K(4,5,6) + K(2,3,4) + K(1,2,6),
      K(1,4,6) + K(2,4,5) + K(2,3,6),
    },
  };
  for (const auto& expr : flatten(b2_generators_y_fixed)) {
    CHECK(expr == cycle_indices(expr, 2));
  }

  const auto b2_generators_x_cyclable = mapped_nested<2>(b2_generators_y_cyclable, kappa_y_to_x);
  const auto b2_generators_x_fixed = mapped_nested<2>(b2_generators_y_fixed, kappa_y_to_x);

  const auto b2_generators = concat(
    mapped_nested<2>(b2_generators_y_cyclable, [](const auto& expr) { return cycle_indices(expr, 0); }),
    mapped_nested<2>(b2_generators_y_cyclable, [](const auto& expr) { return cycle_indices(expr, 2); }),
    mapped_nested<2>(b2_generators_y_cyclable, [](const auto& expr) { return cycle_indices(expr, 4); }),
    mapped_nested<2>(b2_generators_y_fixed, [](const auto& expr) { return cycle_indices(expr, 0); }),
    mapped_nested<2>(b2_generators_x_cyclable, [](const auto& expr) { return cycle_indices(expr, 1); }),
    mapped_nested<2>(b2_generators_x_cyclable, [](const auto& expr) { return cycle_indices(expr, 3); }),
    mapped_nested<2>(b2_generators_x_cyclable, [](const auto& expr) { return cycle_indices(expr, 5); }),
    mapped_nested<2>(b2_generators_x_fixed, [](const auto& expr) { return cycle_indices(expr, 1); })
  );

  const auto fx = concat(
    mapped(combinations(to_vector(range_incl(1, num_points)), 3), [](const auto& points) {
      return KappaExpr::single({Kappa(Gamma(points))});
    }),
    {K_X(), K_Y()}
  );
  CHECK_EQ(fx.size(), 20 + 2);

  const auto b2_full = concat(
    mapped(GrL2(3, to_vector(range_incl(1, num_points))), gamma_expr_to_kappa_expr),
    mapped(b2_generators, [](const auto& gen) {
      // (a-c) * (b-c) == a*b + b*c + c*a
      return n_log(2, to_array<3>(gen));
    })
  );
  const auto b2 = space_basis(b2_full, DISAMBIGUATE(to_lyndon_basis));

  // TODO: Test
  // for (const auto& gen : b2_generators) {
  //   for (const int weight : range_incl(2, 4)) {
  //     const auto expr = n_log(weight, to_array<3>(gen));
  //     CHECK_EQ(expr.weight(), weight);
  //     CHECK(is_totally_weakly_separated(expr));
  //   }
  // }

  std::cout << "Fx rank = " << space_rank(fx, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  std::cout << "B2 rank = " << space_rank(b2, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  for (const int weight : range_incl(2, 6)) {
    const auto space_words = mapped(
      filtered(
        type_d_free_lie_coalgebra(weight),
        DISAMBIGUATE(is_totally_weakly_separated)
      ),
      DISAMBIGUATE(expand_into_glued_pairs)
    );
    const auto space_l = mapped(
      cartesian_combinations(concat(
        std::vector{std::pair{b2, 1}},
        std::vector(weight-2, std::pair{fx, 1})
      )),
      DISAMBIGUATE(acoproduct_vec)
    );
    const auto ranks = space_venn_ranks(
      space_words,
      space_l,
      DISAMBIGUATE(identity_function)
    );
    std::cout << "w=" << weight << ": " << to_string(ranks) << "\n";


    const auto new_space = mapped(b2_generators, [&](const auto& gen) {
      return expand_into_glued_pairs(n_log(weight, to_array<3>(gen)));
    });
    CHECK(space_contains(space_words, new_space, DISAMBIGUATE(identity_function)));
    CHECK(space_contains(space_l, new_space, DISAMBIGUATE(identity_function)));
    std::cout << "Contains: OK\n";
  }
#endif



  // for (const int num_points : range_incl(5, 7)) {
  //   for (const int weight : range_incl(2, 5)) {
  //     const auto space = CL(weight, to_vector(range_incl(1, num_points)));
  //     const auto space_pr = mapped(space, DISAMBIGUATE(project_on_x1));
  //     const auto space_pr_alt = mapped(space, [&](const auto& expr) {
  //       return alt_project_on(1, expr, num_points);
  //     });
  //     const int rank = space_rank(space, DISAMBIGUATE(to_lyndon_basis));
  //     const int rank_pr = space_rank(space_pr, DISAMBIGUATE(to_lyndon_basis));
  //     const int rank_pr_alt = space_rank(space_pr_alt, DISAMBIGUATE(to_lyndon_basis));
  //     std::cout << "p=" << num_points << ", w=" << weight << ": ";
  //     std::cout << rank << " / " << rank_pr << " / " << rank_pr_alt << "\n";
  //   }
  // }

  // for (const int num_vars : range_incl(2, 4)) {
  //   for (const int weight : range_incl(2, 5)) {
  //     const auto& args = concat(
  //       mapped(range_incl(1, num_vars), [](const int idx) { return X(idx); }),
  //       mapped(range_incl(1, num_vars), [](const int idx) { return -X(idx); })
  //       // std::vector{Inf}
  //     );
  //     auto space = L(weight, args);
  //     // TODO: Why so slow?
  //     space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(identity_function),
  //       DISAMBIGUATE(keep_non_weakly_separated_inv)
  //     );
  //     std::cout << "p=" << args.size() << "(" << num_vars << "), w=" << weight << ": ";
  //     std::cout << to_string(ranks) << "\n";
  //   }
  // }

  // const auto expr = to_lyndon_basis(QLi3(x1,x2,x3,x4,-x1,-x2,-x3,-x4));
  // std::cout << expr;
  // const auto axis = Delta(x1,x3);
  // std::cout << "projection on " << to_string(axis) << ": " << alt_project_on(axis, expr, 4);

  // // TODO: Fix !!!
  // for (const int num_vars : range_incl(4, 5)) {
  //   for (const int weight : range_incl(3, 4)) {
  //     const auto& args = concat(
  //       mapped(range_incl(1, num_vars), [](const int idx) { return X(idx); }),
  //       mapped(range_incl(1, num_vars), [](const int idx) { return -X(idx); })
  //     );
  //     auto space = L(weight, args);
  //     space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  //     const auto ranks = space_mapping_ranks(
  //       space,
  //       DISAMBIGUATE(identity_function),
  //       [&](const auto& expr) {
  //         return std::tuple{
  //           keep_non_weakly_separated_inv(expr),
  //           alt_project_on(Delta(x1,x4), expr, num_vars),
  //         };
  //       }
  //     );
  //     std::cout << "p=" << args.size() << "(" << num_vars << "), w=" << weight << ": ";
  //     std::cout << to_string(ranks) << "\n";
  //   }
  // }

  // const std::vector p = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // for (const auto& s : combinations(p, 2)) {
  //   const auto [a, b] = to_array<2>(s);
  //   const auto d = Delta(a, b);
  //   if (is_frozen_coord(d, 4)) {
  //     std::cout << to_string(d) << "\n";
  //   }
  // }



  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // const int weight = 5;
  // const auto expr = typeC_QLi(weight, points);
  // std::cout << is_totally_weakly_separated_inv(expr) << "\n";
  // auto space = L(weight, points);
  // // auto space = typeC_CL(weight, points);
  // space = mapped_parallel(space, DISAMBIGUATE(to_lyndon_basis));
  // std::cout << space_rank(space, DISAMBIGUATE(identity_function)) << "\n";
  // space.push_back(to_lyndon_basis(expr));
  // std::cout << space_rank(space, DISAMBIGUATE(identity_function)) << "\n";

  // const int weight = 4;
  // auto space = typeC_CL(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5});
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";
  // space.push_back(typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}));
  // space.push_back(typeC_QLi(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5}));
  // space.push_back(typeC_QLi(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5}));
  // space.push_back(typeC_QLi(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5}));
  // space.push_back(typeC_QLi(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5}));
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << "\n";

  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(project_on_x1(expr)).filtered([](const auto& term) {
  //     return absl::c_count(term, Zero) == 0;
  //   });
  // };
  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // const int weight = 4;

  // std::cout << prepare(typeC_QLi(weight, points));
  // std::cout << prepare(QLi3(x1,-x2,-x3,-x4));
  // std::cout << prepare(QLi3(x1,x2,-x3,-x4));
  // std::cout << prepare(QLi3(x1,x2,x3,-x4));
  // std::cout << prepare(QLi3(x1,x2,x3,x4));

  // std::cout << prepare(
  //   +  typeC_QLi(weight, points)
  //   -4*QLi3(x1,-x2,-x3,-x4)
  //   -4*QLi3(x1,x2,x3,-x4)
  // );
  // std::cout << prepare(QLi3(x1,x2,x3,-x1));
  // std::cout << prepare(QLi3(-x4,x1,x2,x4));
  // std::cout << prepare(QLi3(-x3,-x4,x1,x3));
  // std::cout << prepare(QLi3(x1,-x1,-x3,-x4));
  // std::cout << prepare(QLi3(x2,x1,-x4,-x2));
  // std::cout << prepare(QLi3(x3,x2,x1,-x3));

  // std::cout << prepare(
  //   +  typeC_QLi(weight, points)
  //   -4*QLi3(x1,-x2,-x3,-x4)
  //   -4*QLi3(x1,x2,x3,-x4)
  //   +4*QLi3(x1,x2,x3,-x1)
  //   +4*QLi3(-x4,x1,x2,x4)
  //   +4*QLi3(x2,x1,-x4,-x2)
  // );

  // std::cout << to_lyndon_basis(
  //   + typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   - typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1})
  //   + 2 * (
  //     + QLi3(x1,x2,x3,x4)
  //     - QLi3(x2,x3,x4,-x1)
  //     + QLi3(x3,x4,-x1,-x2)
  //     - QLi3(x4,-x1,-x2,-x3)
  //     + QLi3(-x1,-x2,-x3,-x4)
  //     - QLi3(-x2,-x3,-x4,x1)
  //     + QLi3(-x3,-x4,x1,x2)
  //     - QLi3(-x4,x1,x2,x3)
  //   )
  // );

  // for (const int weight : range_incl(2, 7)) {
  //   const auto space = mapped_parallel(
  //     typeC_CL(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}), DISAMBIGUATE(to_lyndon_basis)
  //   );
  //   const auto a = to_lyndon_basis(typeC_QLi(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4}));
  //   const auto b = to_lyndon_basis(typeC_QLi(weight, {x2,x3,x4,-x1,-x2,-x3,-x4,x1}));
  //   std::cout << "w=" << weight << "\n";
  //   std::cout << "diff lies in = " << space_contains(space, {a - b}, DISAMBIGUATE(identity_function)) << "\n";
  //   std::cout << "sum lies in = " << space_contains(space, {a + b}, DISAMBIGUATE(identity_function)) << "\n";
  //   std::cout << "\n";
  // }

  // const int weight = 4;
  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // // const auto space = mapped_expanding(range(4), [&](const int shift) {
  // //   return std::array{
  // //     QLiVec(weight, choose_indices_one_based(rotated_vector(points, shift), {1,2,3,4})),
  // //     QLiVec(weight, choose_indices_one_based(rotated_vector(points, shift), {1,2,5,6})),
  // //   };
  // // });
  // const auto qli_a = to_lyndon_basis(
  //   + QLi4(x1,x2,x3,x4)
  //   + QLi4(x2,x3,x4,-x1)
  //   + QLi4(x3,x4,-x1,-x2)
  //   + QLi4(x4,-x1,-x2,-x3)
  //   + QLi4(-x1,-x2,-x3,-x4)
  //   + QLi4(-x2,-x3,-x4,x1)
  //   + QLi4(-x3,-x4,x1,x2)
  //   + QLi4(-x4,x1,x2,x3)
  // );
  // const auto qli_b = to_lyndon_basis(
  //   + QLi4(x1,x2,-x1,-x2)
  //   + QLi4(x2,x3,-x2,-x3)
  //   + QLi4(x3,x4,-x3,-x4)
  //   + QLi4(x4,-x1,-x4,x1)
  // );
  // const std::vector space = {qli_a, qli_b};

  // const auto typec_qli = to_lyndon_basis(
  //   + typeC_QLi(weight, points)
  //   + typeC_QLi(weight, rotated_vector(points, 1))
  // );
  // // std::cout << dump_to_string(space) << "\n";
  // // std::cout << to_string(space_venn_ranks(space, {expr}, DISAMBIGUATE(identity_function))) << "\n";
  // std::cout << 8*qli_a + qli_b + 2*typec_qli;

  // const std::vector points = {x1,x2,x3,x4,-x1,-x2,-x3,-x4};
  // for (const int weight : range_incl(2, 6)) {
  //   const int sign = neg_one_pow(weight);
  //   std::cout << to_lyndon_basis(
  //     + typeC_QLiSymm(weight, points)
  //     + sign * typeC_QLiSymm(weight, rotated_vector(points, 1))
  //   );
  // }

  // std::cout << to_lyndon_basis(typeC_QLi(weight, {x1,x1,x3,x4,-x1,-x1,-x3,-x4}));  // ZERO
  // std::cout << to_lyndon_basis(typeC_QLi(weight, {x1,x2,x2,x4,-x1,-x2,-x2,-x4}));
  // std::cout << to_lyndon_basis(QLi4(1,1,2,3,4,5));  // ZERO
  // std::cout << to_lyndon_basis(QLi4(1,2,2,3,4,5));


  // std::cout << to_lyndon_basis(
  //   + QLi2(x1,x2,x3,-x1)
  //   + QLi2(x2,x3,-x1,-x2)
  //   + QLi2(x3,-x1,-x2,-x3)
  //   + typeC_QLi(2, {x1,x2,-x1,-x2})
  //   + typeC_QLi(2, {x2,x3,-x2,-x3})
  //   + typeC_QLi(2, {x3,-x1,-x3,x1})
  // );
  // std::cout << to_lyndon_basis(
  //   + QLi2(x1,x2,x3,-x1)
  //   + QLi2(x2,x3,-x1,-x2)
  //   + QLi2(x3,-x1,-x2,-x3)
  //   + typeC_QLi(2, {x1,x2,-x1,-x2})
  //   - typeC_QLi(2, {x1,x3,-x1,-x3})
  //   + typeC_QLi(2, {x2,x3,-x2,-x3})
  // );

  // const int weight = 3;
  // const std::vector space = {
  //   typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   ,
  //   + typeC_QLi(weight, {x1,x2,-x1,-x2})
  //   + typeC_QLi(weight, {x2,x3,-x2,-x3})
  //   + typeC_QLi(weight, {x3,x4,-x3,-x4})
  //   + typeC_QLi(weight, {x4,-x1,-x4,x1})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x3,x4})
  //   + QLiSymmVec(weight, {x2,x3,x4,-x1})
  //   + QLiSymmVec(weight, {x3,x4,-x1,-x2})
  //   + QLiSymmVec(weight, {x4,-x1,-x2,-x3})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x3,-x1})
  //   + QLiSymmVec(weight, {x2,x3,x4,-x2})
  //   + QLiSymmVec(weight, {x3,x4,-x1,-x3})
  //   + QLiSymmVec(weight, {x4,-x1,-x2,-x4})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x3,-x3})
  //   + QLiSymmVec(weight, {x2,x3,x4,-x4})
  //   + QLiSymmVec(weight, {x3,x4,-x1,x1})
  //   + QLiSymmVec(weight, {x4,-x1,-x2,x2})
  //   ,
  //   + QLiSymmVec(weight, {x1,x2,x4,-x1})
  //   + QLiSymmVec(weight, {x2,x3,-x1,-x2})
  //   + QLiSymmVec(weight, {x3,x4,-x2,-x3})
  //   + QLiSymmVec(weight, {x4,-x1,-x3,-x4})
  //   ,
  //   + typeC_QLi(weight, {x1,x3,-x1,-x3})
  //   + typeC_QLi(weight, {x2,x4,-x2,-x4})
  // };
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << " / " << space.size() << "\n";

  // const auto prepare = [](const auto& expr) {
  //   return to_lyndon_basis(expr);
  //   // return to_lyndon_basis(project_on_x1(expr));
  //   // return to_lyndon_basis(project_on_x1(expr)).filtered([](const auto& term) {
  //   //   return num_distinct_elements_unsorted(mapped(term, [](X x) { return x.idx(); })) >= 3;
  //   //   // std::vector<int> variables;
  //   //   // for (X x: term) {
  //   //   //   if (!x.is_constant()) {
  //   //   //     variables.push_back(x.idx());
  //   //   //   }
  //   //   // }
  //   //   // return num_distinct_elements_unsorted(variables) >= 3;
  //   // });
  // };
  // for (const int weight : {2, 3}) {
  //   std::cout << prepare(
  //     + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //     - (
  //       + typeC_QLi(weight, {x1,x2,-x1,-x2})
  //       + typeC_QLi(weight, {x2,x3,-x2,-x3})
  //       + typeC_QLi(weight, {x3,x4,-x3,-x4})
  //       + typeC_QLi(weight, {x1,x4,-x1,-x4})
  //     )
  //     + (
  //       + typeC_QLi(weight, {x1,x3,-x1,-x3})
  //       + typeC_QLi(weight, {x2,x4,-x2,-x4})
  //     )
  //     - (
  //       + QLiSymmVec(weight, {x1,x2,x3,x4})
  //       + QLiSymmVec(weight, {x2,x3,x4,-x1})
  //       + QLiSymmVec(weight, {x3,x4,-x1,-x2})
  //       + QLiSymmVec(weight, {x4,-x1,-x2,-x3})
  //     )
  //     - (
  //       + QLiSymmVec(weight, {x1,x2,x4,-x1})
  //       + QLiSymmVec(weight, {x2,x3,-x1,-x2})
  //       + QLiSymmVec(weight, {x3,x4,-x2,-x3})
  //       + QLiSymmVec(weight, {x4,-x1,-x3,-x4})
  //     )
  //     + (
  //       + QLiSymmVec(weight, {x1,x2,x3,-x1})
  //       + QLiSymmVec(weight, {x2,x3,x4,-x2})
  //       + QLiSymmVec(weight, {x3,x4,-x1,-x3})
  //       + QLiSymmVec(weight, {x4,-x1,-x2,-x4})
  //     )
  //     + (
  //       + QLiSymmVec(weight, {x1,x2,x3,-x3})
  //       + QLiSymmVec(weight, {x2,x3,x4,-x4})
  //       + QLiSymmVec(weight, {x1,x3,x4,-x1})
  //       + QLiSymmVec(weight, {x2,x4,-x1,-x2})
  //     )
  //   );
  // }

  // const int weight = 4;
  // const auto space = typeC_CL(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5}),
  // const std::vector new_funcs = {
  //   + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   - typeC_QLiSymm(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5})
  //   + typeC_QLiSymm(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5})
  //   - typeC_QLiSymm(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5})
  //   + typeC_QLiSymm(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5})
  // };
  // std::cout << to_string(space_venn_ranks(space, new_funcs, DISAMBIGUATE(to_lyndon_basis))) << "\n";

  // const int weight = 4;
  // const auto space = typeC_CB(weight, {x1,x2,x3,x4,x5,-x1,-x2,-x3,-x4,-x5});
  // const auto expr =
  //   + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
  //   - typeC_QLiSymm(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5})
  //   + typeC_QLiSymm(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5})
  //   - typeC_QLiSymm(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5})
  //   + typeC_QLiSymm(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5})
  //   - QLiSymm4(x1,x2,x3,x4,x5,-x1)
  //   - QLiSymm4(x2,x3,x4,x5,-x1,-x2)
  //   - QLiSymm4(x3,x4,x5,-x1,-x2,-x3)
  //   - QLiSymm4(x4,x5,-x1,-x2,-x3,-x4)
  //   - QLiSymm4(x5,-x1,-x2,-x3,-x4,-x5)
  // ;
  // std::cout << expr;
  // std::cout << to_string(space_venn_ranks(space, {expr}, DISAMBIGUATE(to_lyndon_basis))) << "\n";

  const int weight = 4;
  const auto typec_qli_expr =
    // sign for typeC_QLiSymm = (-1)^(sum of the positive indices)
    + typeC_QLiSymm(weight, {x1,x2,x3,x4,-x1,-x2,-x3,-x4})
    - typeC_QLiSymm(weight, {x1,x2,x3,x5,-x1,-x2,-x3,-x5})
    + typeC_QLiSymm(weight, {x1,x2,x4,x5,-x1,-x2,-x4,-x5})
    - typeC_QLiSymm(weight, {x1,x3,x4,x5,-x1,-x3,-x4,-x5})
    + typeC_QLiSymm(weight, {x2,x3,x4,x5,-x2,-x3,-x4,-x5})
    - typeC_QLiSymm(weight, {x1,x2,-x1,-x2})
    + typeC_QLiSymm(weight, {x1,x3,-x1,-x3})
    - typeC_QLiSymm(weight, {x1,x4,-x1,-x4})
    + typeC_QLiSymm(weight, {x1,x5,-x1,-x5})
    - typeC_QLiSymm(weight, {x2,x3,-x2,-x3})
    + typeC_QLiSymm(weight, {x2,x4,-x2,-x4})
    - typeC_QLiSymm(weight, {x2,x5,-x2,-x5})
    - typeC_QLiSymm(weight, {x3,x4,-x3,-x4})
    + typeC_QLiSymm(weight, {x3,x5,-x3,-x5})
    - typeC_QLiSymm(weight, {x4,x5,-x4,-x5})
    - QLiSymm4(x1,x2,x3,x4,x5,-x1)
    - QLiSymm4(x2,x3,x4,x5,-x1,-x2)
    - QLiSymm4(x3,x4,x5,-x1,-x2,-x3)
    - QLiSymm4(x4,x5,-x1,-x2,-x3,-x4)
    - QLiSymm4(x5,-x1,-x2,-x3,-x4,-x5)
  ;
  // const auto space = concat(
  //   CB(weight, {x1,x2,x3,x4,x5,-x1}),
  //   CB(weight, {x2,x3,x4,x5,-x1,-x2}),
  //   CB(weight, {x3,x4,x5,-x1,-x2,-x3}),
  //   CB(weight, {x4,x5,-x1,-x2,-x3,-x4}),
  //   CB(weight, {x5,-x1,-x2,-x3,-x4,-x5})
  // );
  // std::cout << to_string(space_venn_ranks(space, {typec_qli_expr}, DISAMBIGUATE(to_lyndon_basis))) << "\n";


  const auto eqn =
    + typec_qli_expr
    - 2 * (
      + QLi4(x1,x2,x3,x4)
      + QLi4(x2,x3,x4,x5)
      + QLi4(x3,x4,x5,-x1)
      + QLi4(x4,x5,-x1,-x2)
      + QLi4(x5,-x1,-x2,-x3)
    )
    + (
      + QLi4(x1,x2,x3,x5)
      + QLi4(x2,x3,x4,-x1)
      + QLi4(x3,x4,x5,-x2)
      + QLi4(x4,x5,-x1,-x3)
      + QLi4(x5,-x1,-x2,-x4)
    )
    + (
      + QLi4(x1,x3,x4,x5)
      + QLi4(x2,x4,x5,-x1)
      + QLi4(x3,x5,-x1,-x2)
      + QLi4(x4,-x1,-x2,-x3)
      + QLi4(x5,-x2,-x3,-x4)
    )
    - (
      + QLi4(x1,x2,x4,x5)
      + QLi4(x2,x3,x5,-x1)
      + QLi4(x3,x4,-x1,-x2)
      + QLi4(x4,x5,-x2,-x3)
      + QLi4(x5,-x1,-x3,-x4)
    )
    - (
      + QLi4(x1,x2,x3,-x1)
      + QLi4(x2,x3,x4,-x2)
      + QLi4(x3,x4,x5,-x3)
      + QLi4(x4,x5,-x1,-x4)
      + QLi4(x5,-x1,-x2,-x5)
    )
    + (
      + QLi4(x1,x2,x4,-x1)
      + QLi4(x2,x3,x5,-x2)
      + QLi4(x3,x4,-x1,-x3)
      + QLi4(x4,x5,-x2,-x4)
      + QLi4(x5,-x1,-x3,-x5)
    )
    - (
      + QLi4(x1,x2,x5,-x1)
      + QLi4(x2,x3,-x1,-x2)
      + QLi4(x3,x4,-x2,-x3)
      + QLi4(x4,x5,-x3,-x4)
      + QLi4(x5,-x1,-x4,-x5)
    )
    - (
      + QLi4(x1,x3,x4,-x1)
      + QLi4(x2,x4,x5,-x2)
      + QLi4(x3,x5,-x1,-x3)
      + QLi4(x4,-x1,-x2,-x4)
      + QLi4(x5,-x2,-x3,-x5)
    )
    + (
      + QLi4(x1,x3,x5,-x1)
      + QLi4(x2,x4,-x1,-x2)
      + QLi4(x3,x5,-x2,-x3)
      + QLi4(x4,-x1,-x3,-x4)
      + QLi4(x5,-x2,-x4,-x5)
    )
    - (
      + QLi4(x1,x4,x5,-x1)
      + QLi4(x2,x5,-x1,-x2)
      + QLi4(x3,-x1,-x2,-x3)
      + QLi4(x4,-x2,-x3,-x4)
      + QLi4(x5,-x3,-x4,-x5)
    )
  ;
  std::cout << to_lyndon_basis(eqn);
  // std::cout << space_rank(space, DISAMBIGUATE(to_lyndon_basis)) << " / " << space.size() << "\n";
  // for (const auto& expr : space) {
  //   std::cout << to_lyndon_basis(expr);
  //   // std::cout << to_lyndon_basis(project_on_x1(expr));
  //   // std::cout << to_lyndon_basis(project_on_x1(expr)).filtered([](const auto& term) {
  //   //   return num_distinct_elements_unsorted(mapped(term, [](X x) { return x.idx(); })) >= 3;
  //   //   // std::vector<int> variables;
  //   //   // for (X x: term) {
  //   //   //   if (!x.is_constant()) {
  //   //   //     variables.push_back(x.idx());
  //   //   //   }
  //   //   // }
  //   //   // return num_distinct_elements_unsorted(variables) >= 3;
  //   // });
  // }
}
