// TODO: Be consistent about converting space to Lyndon basis on construction.
//   Pro: less error-prone, can have defaulted version of "space dim" function.
//   Con: would be slower if, in fact, only comultiplication is required.
// TODO: Be consistent about parallelisation.

// Note. It might be reasonable to store polylog spaces as a collection of
// `std::shared_ptr<const Expr>` instead of raw `Expr`.
// Pros:
//   - Less copying, especially in expressions like `cartesian_product(space_a, space_b)`.
// Cons:
//   - More verbose interface due to having to pack/update shared_ptr.
//   - Might slow down parallel code if all threads are reading from the same place
//     (although it's all read-only access - how bad is that?)

#include "polylog_space.h"

#include "expr_matrix_builder.h"
#include "iterated_integral.h"
#include "itertools.h"
#include "polylog_grli.h"
#include "polylog_grqli.h"
#include "polylog_lira.h"
#include "polylog_qli.h"
#include "triangulation.h"


// TODO: Fix arg types and remove. Or at least, promote.
static std::vector<int> x_to_int(const XArgs& points) {
  return mapped(points.as_x(), [](X x) {
    CHECK(x.is(XForm::var)) << dump_to_string(x);
    return x.idx();
  });
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


PolylogSpace QL(int weight, const XArgs& xargs) {
  if (weight == 1) {
    return CB1(xargs);
  }
  const auto& args = xargs.as_x();
  PolylogSpace space;
  const int max_args = std::min<int>(args.size(), (weight / 2 + 1) * 2);
  for (int num_args = 4; num_args <= max_args; num_args += 2) {
    append_vector(space, mapped_parallel(combinations(args, num_args), [&](const auto& p) {
      return QLiVec(weight, p);
    }));
  }
  return space;
}

PolylogSpace CB1(const XArgs& xargs) {
  const auto& args = xargs.as_x();
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
PolylogSpace CB2(const XArgs& args) {
  const auto head = slice(args.as_x(), 0, 1);
  const auto tail = slice(args.as_x(), 1);
  return mapped(combinations(tail, 3), [&](const auto& p) { return QLi2(concat(head, p)); });
}
PolylogSpace CB3(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return QLi3(p); });
}
PolylogSpace CB4(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return QLi4(p); });
}
PolylogSpace CB5(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return QLi5(p); });
}

PolylogSpace CL4(const XArgs& args) {
  return concat(
    CB4(args),
    mapped(combinations(args.as_x(), 5), [](const auto& p) { return A2(p); })
  );
}
PolylogSpace CL5(const XArgs& args) {
  return concat(
    CB5(args),
    mapped(combinations(args.as_x(), 6), [](const auto& p) { return QLi5(p); })
  );
}
PolylogSpace CL5Alt(const XArgs& args) {
  return concat(
    CL5(args),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      auto a = mapped(p, [](X x) { return x.idx(); });
      return theta_expr_to_delta_expr(
        Lira3(1,1)(
          // CR(choose_indices_one_based(p, std::vector{1,2,3,4})),
          // CR(choose_indices_one_based(p, std::vector{1,4,3,2}))
          CR(a[0], a[1], a[2], a[3]),
          CR(a[0], a[3], a[2], a[1])
        )
      );
    })
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


// Optimization potential: can cartesian_combinations be used to co_CL_N?
PolylogNCoSpace co_CL_3(const XArgs& args) {
  return mapped(
    cartesian_product(CB2(args), CB1(args)),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
}

PolylogNCoSpace co_CL_4(const XArgs& args) {
  return mapped(
    concat(
      cartesian_product(CB3(args), CB1(args)),
      cartesian_product(CB2(args), CB2(args))
    ),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
}

PolylogNCoSpace co_CL_5(const XArgs& args) {
  return mapped(
    concat(
      cartesian_product(CL4(args), CB1(args)),
      cartesian_product(CB3(args), CB2(args))
    ),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
}

PolylogNCoSpace co_CL_6(const XArgs& args) {
  return mapped(
    concat(
      cartesian_product(CL5(args), CB1(args)),
      cartesian_product(CL4(args), CB2(args)),
      cartesian_product(CB3(args), CB3(args))
    ),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
}

PolylogNCoSpace co_CL_6_alt(const XArgs& args) {
  return mapped(
    concat(
      cartesian_product(CL5Alt(args), CB1(args)),
      cartesian_product(CL4(args), CB2(args)),
      cartesian_product(CB3(args), CB3(args))
    ),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
}

PolylogNCoSpace co_CL_6_via_l(const XArgs& args) {
  return mapped(
    concat(
      cartesian_product(CL5(args), CB1(args)),
      cartesian_product(LInf(4, args), CB2(args)),
      cartesian_product(LInf(3, args), LInf(3, args))
    ),
    APPLY(DISAMBIGUATE(ncoproduct))
  );
}

PolylogNCoSpace QL_wedge_QL(int weight, const XArgs& xargs) {
  PolylogNCoSpace ret;
  for (int w : range_incl(1, weight / 2)) {
    const auto space_a = QL(w, xargs.as_x());
    const auto space_b = QL(weight - w, xargs.as_x());

    // for (const auto& a : space_a) {
    //   for (const auto& b : space_b) {
    //     ret.push_back(ncoproduct(a, b));
    //   }
    // }
    // TODO: Why is there virtually no difference between `mapped` and `mapped_parallel`?
    append_vector(ret, mapped_parallel(cartesian_product(space_a, space_b), [](const auto& p) {
      const auto& [a, b] = p;
      return ncoproduct(a, b);
    }));
  }
  return ret;
}

GrPolylogSpace GrLBasic(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  CHECK_LE(5, args.size());
  GrPolylogSpace ret;
  for (const int i : range(args.size())) {
    append_vector(
      ret,
      mapped(combinations(removed_index(args, i), 4), [&](const auto& p) {
        return GrQLiVec(weight, {args[i]}, p);
      })
    );
  }
  return ret;
}

// TODO: Test against zero fixed points
GrPolylogSpace GrL1(int dimension, const XArgs& xargs) {
  const int weight = 1;
  CHECK_LE(2, dimension);
  const int num_fixed_points = 2;  // should be equivalent to zero fixed points
  auto args = xargs.as_x();
  CHECK_LE(5, args.size());
  auto fixed_points = slice(args, args.size() - num_fixed_points);
  args = slice(args, 0, args.size() - num_fixed_points);
  GrPolylogSpace ret;
  for (const auto& bonus_point_indices : combinations(to_vector(range(args.size())), dimension - 2)) {
    const auto bonus_points = choose_indices(args, bonus_point_indices);
    const auto qli_points = removed_indices(args, bonus_point_indices);
    // TODO: Add `mapped_expanding` and use it here and in other places.
    append_vector(
      ret,
      mapped(combinations(qli_points, 4 - num_fixed_points), [&](auto p) {
        return GrQLiVec(weight, bonus_points, concat(p, fixed_points));
      })
    );
    append_vector(
      ret,
      mapped(combinations(qli_points, 4 - num_fixed_points), [&](auto p) {
        append_vector(p, fixed_points);
        CHECK_EQ(4, p.size());
        std::swap(p[1], p[2]);  // 1 - cross_ratio
        return GrQLiVec(weight, bonus_points, p);
      })
    );
  }
  return ret;
}

// TODO: Test against zero fixed points
// TODO: Test against naive: `GrLBasic(2, xargs)`
GrPolylogSpace GrL2(int dimension, const XArgs& xargs) {
  const int weight = 2;
  CHECK_LE(2, dimension);
  const int num_fixed_points = 1;  // should be equivalent to zero fixed points
  auto args = xargs.as_x();
  CHECK_LE(5, args.size());
  auto fixed_points = slice(args, args.size() - num_fixed_points);
  args = slice(args, 0, args.size() - num_fixed_points);
  GrPolylogSpace ret;
  for (const auto& bonus_point_indices : combinations(to_vector(range(args.size())), dimension - 2)) {
    const auto bonus_points = choose_indices(args, bonus_point_indices);
    const auto qli_points = removed_indices(args, bonus_point_indices);
    append_vector(
      ret,
      mapped(combinations(qli_points, 4 - num_fixed_points), [&](const auto& p) {
        return GrQLiVec(weight, bonus_points, concat(p, fixed_points));
      })
    );
  }
  return ret;
}

GrPolylogSpace GrL3(int dimension, const XArgs& xargs) {
  const int weight = 3;
  CHECK_LE(2, dimension);
  auto args = xargs.as_x();
  CHECK_LE(5, args.size());
  GrPolylogSpace ret;
  for (const auto& bonus_point_indices : combinations(to_vector(range(args.size())), dimension - 2)) {
    const auto bonus_points = choose_indices(args, bonus_point_indices);
    const auto qli_points = removed_indices(args, bonus_point_indices);
    append_vector(
      ret,
      mapped(combinations(qli_points, 4), [&](auto p) {
        return GrQLiVec(weight, bonus_points, p);
      })
    );
    append_vector(
      ret,
      mapped(combinations(qli_points, 4), [&](auto p) {
        CHECK_EQ(4, p.size());
        std::swap(p[1], p[2]);  // 1 - cross_ratio
        return GrQLiVec(weight, bonus_points, p);
      })
    );
  }
  if (dimension >= weight) {
    for (const auto& bonus_point_indices : combinations(to_vector(range(args.size())), dimension - weight)) {
      const auto bonus_points = choose_indices(args, bonus_point_indices);
      const auto qli_points = removed_indices(args, bonus_point_indices);
      append_vector(
        ret,
        mapped(combinations(qli_points, 6), [&](auto p) {
          return GrLiVec(x_to_int(bonus_points), x_to_int(p));
        })
      );
    }
  }
  return ret;
}
