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
#include "lib/iterated_integral.h"
#include "lib/itertools.h"
#include "lib/lexicographical.h"
#include "lib/linalg.h"
#include "lib/lira_ones.h"
#include "lib/loops.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
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


struct GammaACoExprParam : GammaCoExpr::Param {
  static bool lyndon_compare(const VectorT::value_type& lhs, const VectorT::value_type& rhs) {
    const auto lhs_size = -static_cast<int>(lhs.size());
    const auto rhs_size = -static_cast<int>(rhs.size());
    return std::tie(lhs_size, lhs) < std::tie(rhs_size, rhs);
  };
};

using GammaACoExpr = Linear<GammaACoExprParam>;

using GrPolylogACoSpace = std::vector<GammaACoExpr>;

// Converts each term
//     x1 * x2 * ... * xn
// into a sum
//   + (x1^x2) * x3 * ... * xn
//   + x1 * (x2^x3) * ... * xn
//     ...
//   + x1 * x1 * ... * (x{n-1}^xn)
//
GammaACoExpr expand_into_glued_pairs(const GammaExpr& expr) {
  using ExprT = GammaExpr;
  using CoExprT = GammaACoExpr;
  return expr.mapped_expanding([](const auto& term) {
    CoExprT expanded_expr;
    for (const int i : range(term.size() - 1)) {
      std::vector<GammaExpr> expanded_term;
      for (const int j : range(term.size() - 1)) {
        if (j < i) {
          expanded_term.push_back(GammaExpr::single({term[j]}));
        } else if (j == i) {
          expanded_term.push_back(GammaExpr::single({term[j], term[j+1]}));
        } else {
          expanded_term.push_back(GammaExpr::single({term[j+1]}));
        }
      }
      expanded_expr += internal::abstract_coproduct_vec<CoExprT>(expanded_term);
    }
    return expanded_expr;
  });
}

GammaExpr unglue_pairs(const GammaACoExpr& coexpr) {
  return coexpr.mapped<GammaExpr>(DISAMBIGUATE(flatten));
}

GammaExpr glue_unglue_transform(const GammaExpr& expr) {
  return unglue_pairs(expand_into_glued_pairs(expr));
}


bool keep_for_normalize_remove_consecutive(const GammaExpr::ObjectT& term) {
  return absl::c_none_of(term, [](const Gamma& g) {
    const auto& v = g.index_vector();
    const auto it = absl::c_adjacent_find(v, [](int a, int b) {
      return b != a + 1;
    });
    return it == v.end();
  });
}

GammaExpr normalize_remove_consecutive(const GammaExpr& expr) {
  return expr.filtered([](const auto& term) {
    return keep_for_normalize_remove_consecutive(term);
  });
}

GrPolylogSpace normalize_space_remove_consecutive(const GrPolylogSpace& space) {
  return mapped(space, &normalize_remove_consecutive);
}

std::string GrL3_kernel(int dimension, const XArgs& xargs) {
  const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, xargs));
  const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, xargs));
  const auto product_space = mapped(
    cartesian_product(l2, l1),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
  return polylog_space_kernel_describe(
    product_space,
    DISAMBIGUATE(identity_function),
    DISAMBIGUATE(ncomultiply)
  );
}

std::string GrL4_a_kernel(int dimension, const XArgs& xargs) {
  const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, xargs));
  const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, xargs));
  const auto l3 = normalize_space_remove_consecutive(GrL3(dimension, xargs));
  const auto product_space = mapped(
    concat(
      cartesian_product(l3, l1),
      cartesian_product(l2, l2)
    ),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
  return polylog_space_kernel_describe(
    product_space,
    DISAMBIGUATE(identity_function),
    DISAMBIGUATE(ncomultiply)
  );
}

std::string GrL4_b_kernel(int dimension, const XArgs& xargs) {
  const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, xargs));
  const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, xargs));
  const auto product_space = mapped(
    cartesian_product(l2, l1, l1),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
  return polylog_space_kernel_describe(
    product_space,
    DISAMBIGUATE(identity_function),
    DISAMBIGUATE(ncomultiply)
  );
}

// TODO: Use for converting to `X`
template<typename T>
struct convert_to_t {
  template<class U>
  T operator()(U&& u) const {
    return T(std::forward<U>(u));
  }
};
template<typename T>
constexpr convert_to_t<T> convert_to;

// No dependencies: space rank == space.size()
GrPolylogSpace GrQLi3_test_space(const XArgs& xargs) {  // dimension = 3
  auto args = xargs.as_x();
  CHECK_EQ(6, args.size());
  GrPolylogSpace ret;
  for (int discarded_point : range(6)) {
    const auto combines_args = removed_index(args, discarded_point);
    for (const int shift : range(5)) {
      const auto rotated_args = rotated_vector(combines_args, shift);
      ret.push_back(GrQLi3(rotated_args[0])(slice(rotated_args, 1)));
    }
  }
  return ret;
}

// No dependencies: space rank == space.size()
GrPolylogSpace GrQLi4_test_space(const XArgs& xargs) {  // dimension = 3
  const int weight = 4;
  auto args = xargs.as_x();
  CHECK_EQ(7, args.size());
  GrPolylogSpace ret;
  for (const int bonus_point_idx : range(7)) {
    const X bonus_point = args[bonus_point_idx];
    const auto main_args_set = removed_index(args, bonus_point_idx);
    CHECK_EQ(6, main_args_set.size());
    ret.push_back(GrQLiVec(weight, {bonus_point}, main_args_set));
    for (const auto& main_args : combinations(main_args_set, 4)) {
      ret.push_back(GrQLiVec(weight, {bonus_point}, main_args));
    }
  }
  return ret;
}


int main(int /*argc*/, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    // .set_encoder(Encoder::ascii)
    .set_encoder(Encoder::unicode)
    .set_rich_text_format(RichTextFormat::console)
    // .set_rich_text_format(RichTextFormat::html)
    // .set_expression_line_limit(FormattingConfig::kNoLineLimit)
    .set_expression_line_limit(100)
    .set_annotation_sorting(AnnotationSorting::length)
    .set_compact_x(true)
  );


  // // const auto expr = GrQLiVec(2, {5}, {1,2,3,4});
  // const auto expr = GrQLi2(5)(1,2,3,4);
  // std::cout << expr;
  // std::cout << to_lyndon_basis(expr);

  // std::cout << are_weakly_separated(Gamma({2,3,5}), Gamma({1,4,5})) << "\n";

  // const auto expr = to_lyndon_basis(GrQLi3(5)(1,2,3,4));
  // std::cout << expr;
  // std::cout << keep_weakly_separated(expr);
  // std::cout << expr - keep_weakly_separated(expr);

  // std::cout << dump_to_string(GrL2({1,2,3,4,5})) << "\n";

  // for (const int num_points : range_incl(5, 8)) {
  //   const auto points = seq_incl(1, num_points);
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << compute_polylog_space_dim(GrL2(points), DISAMBIGUATE(to_lyndon_basis));
  //   std::cout << "\n";
  // }

  // const int num_points = 6;
  // const auto points = seq_incl(1, num_points);
  // std::cout << "p=" << num_points << ": ";
  // std::cout << compute_polylog_space_dim(GrQLi3_test_space(points), DISAMBIGUATE(to_lyndon_basis));
  // std::cout << "\n";

  // const int num_points = 7;
  // const auto points = seq_incl(1, num_points);
  // std::cout << "p=" << num_points << ": ";
  // std::cout << compute_polylog_space_dim(GrQLi4_test_space(points), DISAMBIGUATE(to_lyndon_basis));
  // std::cout << "\n";

  // const int dimension = 3;
  // const int weight = 3;
  // for (const int num_points : range_incl(5, 9)) {
  //   const auto points = seq_incl(1, num_points);
  //   const auto alphabet = combinations(points, dimension);
  //   GrPolylogNCoSpace s1;
  //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //     // const auto term = mapped(w, [](const auto& t) { return Gamma(t); });
  //     const auto term = mapped(w, convert_to<Gamma>);
  //     if (is_weakly_separated(term)) {
  //       s1.push_back(ncomultiply(GammaExpr::single(term)));
  //     }
  //   }
  //   GrPolylogNCoSpace s2 = mapped(
  //     cartesian_product(GrL1(points), GrL2(points)),
  //     [](const auto& elements) {
  //       const auto& [a, b] = elements;
  //       return ncoproduct(a, b);
  //     }
  //   );
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function)) << "\n";
  //   // Equals `compute_polylog_space_dim(GrLBasic(3, points), DISAMBIGUATE(to_lyndon_basis))`
  // }

  // const int dimension = 3;
  // const int weight = 4;
  // for (const int num_points : range_incl(5, 9)) {
  //   const auto points = to_vector(range_incl(1, num_points));
  //   const auto alphabet = combinations(points, dimension);
  //   GrPolylogNCoSpace s1;
  //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //     const auto term = mapped(w, convert_to<Gamma>);
  //     if (is_weakly_separated(term)) {
  //       s1.push_back(ncomultiply(GammaExpr::single(term)));
  //     }
  //   }
  //   const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, points));
  //   const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, points));
  //   const auto l3 = normalize_space_remove_consecutive(GrL3(dimension, points));
  //   const auto s2 = mapped(
  //     concat(
  //       cartesian_product(l3, l1),
  //       cartesian_product(l2, l2)
  //     ),
  //     APPLY(DISAMBIGUATE(ncoproduct))
  //   );
  //   std::cout << "p=" << num_points << ": ";
  //   std::cout << polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function)) << "\n";
  // }

  // DeltaExpr expr = DeltaExpr::single({Delta(x1,Zero), Delta(x2,Zero), Delta(x3,Zero), Delta(x4,Zero)});
  // std::cout << comultiply(expr, {1,1,2});

  // TablePrinter t;
  // t.set_enable_alignment(false);
  // t.set_column_separator("\t");
  // for (const int dimension : range_incl(3, 5)) {
  //   for (const int num_points : range_incl(5, 11)) {
  //     const auto points = seq_incl(1, num_points);
  //     const int rank = compute_polylog_space_dim(GrL3(dimension, points), DISAMBIGUATE(to_lyndon_basis));
  //     std::cout << "d=" << dimension << ", p=" << num_points << ": ";
  //     std::cout << rank << "\n";
  //     t.set_content({num_points, dimension}, to_string(rank));
  //   }
  // }
  // std::cout << t.to_string();

  // for (const int dimension : range_incl(2, 5)) {
  //   for (const int num_points : range_incl(5, 8)) {
  //     const auto points = seq_incl(1, num_points);
  //     const auto a = GrL4_a_kernel(dimension, points);
  //     const auto b = GrL4_b_kernel(dimension, points);
  //     std::cout << "d=" << dimension << ", p=" << num_points << ":  ";
  //     std::cout << a << "  vs  " << b << "\n";
  //   }
  // }

  // const auto prepare = [](const GammaExpr& expr) {
  //   // return normalize_remove_consecutive(keep_weakly_separated(expr));
  //   return normalize_remove_consecutive(expr);
  // };
  // const int dimension = 3;
  // const int weight = 4;
  // for (const int num_points : range_incl(5, 9)) {
  //   Profiler profiler;
  //   const auto points = to_vector(range_incl(1, num_points));
  //   GrPolylogNCoSpace s1;
  //   const auto alphabet = combinations(points, dimension);
  //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //     const auto term = mapped(w, convert_to<Gamma>);
  //     s1.push_back(ncomultiply(prepare(GammaExpr::single(term))));
  //   }
  //   // for (const auto& alphabet_basis : combinations(points, num_points - 1)) {
  //   //   const auto alphabet = combinations(alphabet_basis, dimension);
  //   //   for (const auto& w : get_lyndon_words(alphabet, weight)) {
  //   //     const auto term = mapped(w, convert_to<Gamma>);
  //   //     s1.push_back(ncomultiply(prepare(GammaExpr::single(term))));
  //   //   }
  //   // }
  //   const auto l1 = mapped(GrL1(dimension, points), prepare);
  //   const auto l2 = mapped(GrL2(dimension, points), prepare);
  //   const auto l3 = mapped(GrL3(dimension, points), prepare);
  //   const auto s2 = mapped(
  //     concat(
  //       cartesian_product(l3, l1),
  //       cartesian_product(l2, l2)
  //     ),
  //     APPLY(DISAMBIGUATE(ncoproduct))
  //   );
  //   profiler.finish("spaces");
  //   const auto description = polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function));
  //   profiler.finish("ranks");
  //   std::cout << "p=" << num_points << ": " << description << "\n";
  // }

  // for (const int dimension : range_incl(2, 9)) {
  //   for (const int num_points : range_incl(5, 11)) {
  //     const auto points = seq_incl(1, num_points);
  //     const auto description = GrL3_kernel(dimension, points);
  //     std::cout << "d=" << dimension << ", p=" << num_points << ": ";
  //     std::cout << description << "\n";
  //   }
  // }

  // for (const int dimension : range_incl(4, 4)) {
  //   for (const int num_points : range_incl(5, 10)) {
  //     const auto points = seq_incl(1, num_points);
  //     std::cout << "d=" << dimension << ", p=" << num_points << ":  ";
  //     std::cout << GrL3_kernel(dimension, points);
  //     // std::cout << "  vs  ";
  //     // std::cout << compute_polylog_space_dim(GrLBasic(3, points), DISAMBIGUATE(to_lyndon_basis));
  //     std::cout << "\n";
  //   }
  // }

  // const int dimension = 3;
  // const int num_points = 7;
  // const auto points = seq_incl(1, num_points);
  // const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, points));
  // const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, points));
  // for (const int num_l1 : range(0, 10)) {
  //   Profiler profiler;
  //   const auto space = mapped(
  //     cartesian_product(l2, combinations(seq_incl(0, l1.size() - 1), num_l1)),
  //     [&](const auto& args) {
  //       const auto& [l2_arg, l1_indices] = args;
  //       std::vector<GammaExpr> v = concat({l2_arg}, choose_indices(l1, l1_indices));
  //       return ncoproduct(v);
  //     }
  //   );
  //   profiler.finish("spaces");
  //   const auto rank = compute_polylog_space_dim(space, DISAMBIGUATE(ncomultiply));
  //   profiler.finish("rank");
  //   std::cout << "l2*l1^" << num_l1 << ": " << rank << "\n";
  // }

  const int dimension = 3;
  const int weight = 5;
  for (const int num_points : range_incl(5, 9)) {
    const auto points = to_vector(range_incl(1, num_points));
    const auto alphabet = combinations(points, dimension);
    GrPolylogACoSpace s1;
    for (const auto& w : get_lyndon_words(alphabet, weight)) {
      const auto term = mapped(w, convert_to<Gamma>);
      if (is_weakly_separated(term) && keep_for_normalize_remove_consecutive(term)) {
        s1.push_back(expand_into_glued_pairs(GammaExpr::single(term)));
      }
    }
    const auto l1 = normalize_space_remove_consecutive(GrL1(dimension, points));
    const auto l2 = normalize_space_remove_consecutive(GrL2(dimension, points));
    const auto s2 = mapped(
      cartesian_product(l2, cartesian_power(l1, weight - 2)),
      [&](const auto& args) {
        const auto& [l2_arg, l1_args] = args;
        std::vector<GammaExpr> v = concat({l2_arg}, to_vector(l1_args));
        // TODO: Promote abstract_coproduct.
        return internal::abstract_coproduct_vec<GammaACoExpr>(v);
      }
    );
    std::cout << "p=" << num_points << ": ";
    std::cout << polylog_spaces_intersection_describe(s1, s2, DISAMBIGUATE(identity_function)) << "\n";
  }
}
