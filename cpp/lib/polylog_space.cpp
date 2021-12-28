// TODO: Be consistent about parallelisation.
//
// Optimization potential: Convert spaces to Lyndon basis on construction.
// Pros:
//   + Less verbose, don't have to pass to_lyndon_basis to each rank computations.
//   + Could be faster if Lyndon basis is required multiple times, e.g. when spaces
//     are used to construct larger co-spaces via coproduct.
// Cons:
//   - What if there are Hopf algebra use-cases?
//   - Could be slower if, in fact, only comultiplication of a space is required.
//
// Optimization potential: Store spaces as a collection of `std::shared_ptr<const Expr>`
//   instead of raw `Expr`.
// Pros:
//   + Less copying, especially in expressions like `cartesian_product(space_a, space_b)`.
// Cons:
//   - More verbose interface due to having to pack/update shared_ptr.
//   - Might slow down parallel code if all threads are reading from the same place
//     (although it's all read-only access - how bad is that?)

#include "polylog_space.h"

#include "expr_matrix_builder.h"
#include "iterated_integral.h"
#include "polylog_grli.h"
#include "polylog_grqli.h"
#include "polylog_lira.h"
#include "polylog_qli.h"
#include "triangulation.h"


// Permutes elements such that
//   cross_ratio(one_minus_cross_ratio(p)) == 1 - cross_ratio(p)
template<typename Container>
static Container one_minus_cross_ratio(Container p) {
  CHECK_EQ(4, p.size());
  std::swap(p[1], p[2]);
  return p;
}


template<typename SpaceT>
std::string space_to_string(const SpaceT& space) {
  return absl::StrCat("<", str_join(space, ", ", [](const auto& expr) {
    return annotations_one_liner(expr.annotations());
  }), ">");
}

std::string dump_to_string_impl(const PolylogSpace& space) { return space_to_string(space); }
std::string dump_to_string_impl(const PolylogNCoSpace& space) { return space_to_string(space); }
std::string dump_to_string_impl(const GrPolylogSpace& space) { return space_to_string(space); }
std::string dump_to_string_impl(const GrPolylogNCoSpace& space) { return space_to_string(space); }
std::string dump_to_string_impl(const GrPolylogACoSpace& space) { return space_to_string(space); }


PolylogSpace CB_naive_via_QLi_fours(int weight, const XArgs& xargs) {
  return mapped(combinations(xargs.as_x(), 4), [&](const auto& p) { return QLiVec(weight, p); });
}

PolylogSpace CB(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  switch (weight) {
    case 1: {
      const int n = args.size();
      PolylogSpace ret;
      for (const int i : range(n)) {
        for (const int j : range(i+2, n)) {
          const int ip = i + 1;
          CHECK_LT(ip, n);
          const int jp = (j + 1) % n;
          if (jp != i) {
            ret.push_back(Log(args[i], args[j], args[ip], args[jp]));
          }
        }
      }
      return ret;
    }
    case 2: {
      const auto head = slice(args, 0, 1);
      const auto tail = slice(args, 1);
      return mapped(combinations(tail, 3), [&](const auto& p) { return QLi2(concat(head, p)); });
    }
    default: {
      return CB_naive_via_QLi_fours(weight, xargs);
    }
  }
}

PolylogSpace CL(int weight, const XArgs& xargs) {
  if (weight <= 2) {
    return CB(weight, xargs);
  }
  const auto& args = xargs.as_x();
  PolylogSpace space;
  const int max_args = std::min<int>(args.size(), (weight / 2 + 1) * 2);
  for (int num_args = 4; num_args <= max_args; num_args += 2) {
    append_vector(space, mapped(combinations(args, num_args), [&](const auto& p) {
      return QLiVec(weight, p);
    }));
  }
  return space;
}

PolylogSpace old_CL4_via_A2(const XArgs& args) {
  return concat(
    CB4(args),
    mapped(combinations(args.as_x(), 5), [](const auto& p) { return A2(p); })
  );
}

// PolylogSpace H(int weight, const XArgs& xargs) {
//   auto args = xargs.as_x();
//   const X special_point = args.back();
//   args.pop_back();
//   PolylogSpace ret;
//   for (const auto& seq : cartesian_power(args, weight + 2)) {
//     auto expr = IAlt(concat({special_point}, seq));
//     CHECK(expr.is_zero() || expr.weight() == weight);
//     ret.push_back(std::move(expr));
//   }
//   return ret;
// }

PolylogSpace H(int weight, const XArgs& xargs) {
  PolylogSpace ret = L(weight, xargs);
  for (int w : range_incl(1, weight / 2)) {
    PolylogSpace space_a = H(w, xargs);
    PolylogSpace space_b = H(weight - w, xargs);
    for (const auto& a : space_a) {
      for (const auto& b : space_b) {
        ret.push_back(shuffle_product_expr(a, b));
      }
    }
  }
  return ret;
}

PolylogSpace Fx(const XArgs& xargs) {
  return mapped(combinations(xargs.as_x(), 2), [](const auto& p) {
    const auto [a, b] = to_array<2>(p);
    return D(a, b);
  });
}

PolylogSpace LInf(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  // Note: See tests for alternative definitions that support arbitrary arguments, but have duplicates.
  // Note: See L for alternative definitions that support arbitrary arguments.
  CHECK(!args.empty() && args.back() == Inf) << dump_to_string(args);
  // TODO: Is this how it should be defined for weight 1?
  if (weight == 1) {
    return CB1(xargs);
  }
  PolylogSpace ret;
  for (const int alphabet_size : range(2, args.size() - 1)) {
    const X last_arg = args[alphabet_size];
    append_vector(
      ret,
      mapped_parallel(
        get_lyndon_words(slice(args, 0, alphabet_size), weight),
        [last_arg](const auto& word) {
          return Corr(concat(word, {last_arg}));
        }
      )
    );
  }
  return ret;
}

PolylogSpace L(int weight, const XArgs& xargs) {
  if (weight == 1) {
    return CB1(xargs);
  }
  auto args = xargs.as_x();
  const X special_point = args.back();
  args.pop_back();
  PolylogSpace ret;
  for (const int alphabet_size : range(2, args.size())) {
    const X last_arg = args[alphabet_size];
    append_vector(
      ret,
      mapped_parallel(
        get_lyndon_words(slice(args, 0, alphabet_size), weight),
        [special_point, last_arg](const auto& word) {
          return CorrAlt(concat(
            std::vector{special_point},
            word,
            std::vector{last_arg}
          ));
        }
      )
    );
  }
  return ret;
}

PolylogSpace XCoords(int weight, const XArgs& args) {
  const auto cluster_coordinates_set = get_triangulation_quadrangles(args.as_x());
  PolylogSpace ret;
  for (const auto& cluster_coordinates : cluster_coordinates_set) {
    append_vector(
      ret,
      mapped_parallel(
        get_lyndon_words(cluster_coordinates, weight),
        [](const auto& word) {
          return tensor_product(absl::MakeConstSpan(
            mapped(word, DISAMBIGUATE(cross_ratio))
          ));
        }
      )
    );
  }
  return ret;
}

PolylogSpace ACoords(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  const auto triangulations = get_triangulations(args);
  PolylogSpace ret;
  for (auto triangulation : triangulations) {
    // Add outer edges.
    for (const int i : range(args.size())) {
      triangulation.push_back({args[i], args[(i+1) % args.size()]});
    }
    // Optimization potential: construct the vector directly without tensor_product.
    append_vector(
      ret,
      mapped_parallel(
        get_lyndon_words(triangulation, weight),
        [](const auto& word) {
          return tensor_product(absl::MakeConstSpan(
            mapped(word, [](const auto& d) { return D(d.first, d.second); })
          ));
        }
      )
    );
  }
  return ret;
}

PolylogSpace ACoordsHopf(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  const auto triangulations = get_triangulations(args);
  PolylogSpace ret;
  for (auto triangulation : triangulations) {
    // Add outer edges.
    for (const int i : range(args.size())) {
      triangulation.push_back({args[i], args[(i+1) % args.size()]});
    }
    // Optimization potential: construct the vector directly without tensor_product.
    for (const auto& word : cartesian_power(triangulation, weight)) {
      ret.push_back(tensor_product(absl::MakeConstSpan(
        mapped(word, [](const auto& d) { return D(d.first, d.second); })
      )));
    }
  }
  return ret;
}

GrPolylogSpace GrFx(int dimension, const std::vector<int>& args) {
  return mapped(combinations(args, dimension), DISAMBIGUATE(G));
}

GrPolylogSpace GrL_core(
  int weight, int dimension, const std::vector<int>& args,
  bool include_one_minus_cross_ratio, int num_fixed_points
) {
  CHECK_LE(2, dimension);
  CHECK_LE(num_fixed_points, args.size());
  const auto& [main_args, fixed_p] = split_slice(args, args.size() - num_fixed_points);
  const auto& fixed_points = fixed_p;  // workaround: lambdas cannot capture structured bindings
  GrPolylogSpace ret;
  for (const auto& [bonus_p, qli_points] : index_splits(main_args, dimension - 2)) {
    const auto& bonus_points = bonus_p;  // workaround: lambdas cannot capture structured bindings
    append_vector(
      ret,
      mapped_expanding(combinations(qli_points, 4 - num_fixed_points), [&](auto p) {
        const auto qli_args = concat(p, fixed_points);
        std::vector ret = {GrQLiVec(weight, bonus_points, qli_args)};
        if (include_one_minus_cross_ratio) {
          ret.push_back(GrQLiVec(weight, bonus_points, one_minus_cross_ratio(qli_args)));
        }
        return ret;
      })
    );
  }
  return ret;
}

GrPolylogSpace GrL1(int dimension, const std::vector<int>& args) {
  return GrL_core(1, dimension, args, true, 2);  // equivalent to zero fixed points
}

GrPolylogSpace GrL2(int dimension, const std::vector<int>& args) {
  return GrL_core(2, dimension, args, false, 1);  // equivalent to zero fixed points
}

GrPolylogSpace GrL3(int dimension, const std::vector<int>& args) {
  const int weight = 3;
  GrPolylogSpace ret = GrL_core(weight, dimension, args, true, 0);
  if (dimension >= weight) {
    for (const auto& [bonus_p, qli_points] : index_splits(args, dimension - weight)) {
      const auto& bonus_points = bonus_p;  // workaround: lambdas cannot capture structured bindings
      append_vector(
        ret,
        mapped(combinations(qli_points, 6), [&](auto p) {
          return GrLiVec(bonus_points, p);
        })
      );
    }
  }
  return ret;
}

GrPolylogSpace GrL(int weight, int dimension, const std::vector<int>& args) {
  switch (weight) {
    case 1: return GrL1(dimension, args);
    case 2: return GrL2(dimension, args);
    case 3: return GrL3(dimension, args);
  }
  FATAL(absl::StrCat("Unsupported weight for GrL: ", weight));
}

PolylogNCoSpace simple_co_L(int weight, int num_coparts, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  return co_space(weight, num_coparts, [&](const int w) {
    return mapped(L(w, points), [&](const auto& expr) {
      // Precompute Lyndon basis to speed up coproduct.
      return to_lyndon_basis(normalize_remove_consecutive(expr));
    });
  });
}

GrPolylogNCoSpace simple_co_GrL(int weight, int num_coparts, int dimension, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  return co_space(weight, num_coparts, [&](const int w) {
    return mapped(GrL(w, dimension, points), [&](const auto& expr) {
      // Precompute Lyndon basis to speed up coproduct.
      return to_lyndon_basis(normalize_remove_consecutive(expr));
    });
  });
}

PolylogNCoSpace co_CL(int weight, int num_coparts, const XArgs& xargs) {
  const auto points = xargs.as_x();
  return co_space(weight, num_coparts, [&](const int w) {
    // Precompute Lyndon basis to speed up coproduct.
    return mapped(CL(w, points), DISAMBIGUATE(to_lyndon_basis));
  });
}

std::string to_string(const SpaceCharacteristics& characteristics) {
  return absl::StrCat("{w=", characteristics.weight, ", d=", characteristics.dimension, "}");
}

std::string to_string(const SpaceVennRanks& ranks) {
  return absl::StrCat(
    fmt::parens(absl::StrCat(ranks.a(), ", ", ranks.b())), ", ",
    fmt::set_intersection(), " = ", ranks.intersected()
  );
}

std::string to_string(const SpaceMappingRanks& ranks) {
  return absl::StrCat(ranks.space(), " - ", ranks.image(), " = ", ranks.kernel());
}
