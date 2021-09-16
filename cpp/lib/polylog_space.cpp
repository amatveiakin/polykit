// TODO: Are shared pointers a good idea? This causes sharing between threads !!!
// TODO: Be consistent about converting space to Lyndon basis on construction.
//   Pro: less error-prone, can have defaulted version of "space dim" function.
//   Con: would be slower if, in fact, only comultiplication is required.
// TODO: Be consistent about parallelisation.

#include "polylog_space.h"

#include "expr_matrix_builder.h"
#include "iterated_integral.h"
#include "itertools.h"
#include "polylog_lira.h"
#include "polylog_qli.h"
#include "triangulation.h"


template<typename SpaceT>
std::string space_to_string(const SpaceT& space) {
  return absl::StrCat("<", str_join(space, ", ", [](const auto& s) {
    const auto& annotations = s->annotations();
    CHECK(annotations.errors.empty());
    CHECK_EQ(annotations.expression.num_terms(), 1);
    return annotations.expression.element().first;
  }), ">");
}

std::string dump_to_string_impl(const PolylogSpace& space) { return space_to_string(space); }
std::string dump_to_string_impl(const PolylogNCoSpace& space) { return space_to_string(space); }


PolylogSpace QL(int weight, const XArgs& xargs) {
  if (weight == 1) {
    return CB1(xargs);
  }
  const auto& args = xargs.as_x();
  PolylogSpace space;
  const int max_args = std::min<int>(args.size(), (weight / 2 + 1) * 2);
  for (int num_args = 4; num_args <= max_args; num_args += 2) {
    append_vector(space, mapped(combinations(args, num_args), [&](const auto& p) {
      return wrap_shared(QLiVec(weight, p));
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
        ret.push_back(wrap_shared(Log(args[i], args[j], args[ip], args[jp])));
      }
    }
  }
  return ret;
}
PolylogSpace CB2(const XArgs& args) {
  const auto head = slice(args.as_x(), 0, 1);
  const auto tail = slice(args.as_x(), 1);
  return mapped(combinations(tail, 3), [&](const auto& p) { return wrap_shared(QLi2(concat(head, p))); });
}
PolylogSpace CB3(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return wrap_shared(QLi3(p)); });
}
PolylogSpace CB4(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return wrap_shared(QLi4(p)); });
}
PolylogSpace CB5(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return wrap_shared(QLi5(p)); });
}

PolylogSpace CL4(const XArgs& args) {
  return concat(
    CB4(args),
    mapped(combinations(args.as_x(), 5), [](const auto& p) { return wrap_shared(A2(p)); })
  );
}
PolylogSpace CL5(const XArgs& args) {
  return concat(
    CB5(args),
    mapped(combinations(args.as_x(), 6), [](const auto& p) { return wrap_shared(QLi5(p)); })
  );
}
PolylogSpace CL5Alt(const XArgs& args) {
  return concat(
    CL5(args),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      auto a = mapped(p, [](X x) { return x.idx(); });
      return wrap_shared(theta_expr_to_delta_expr(
        Lira3(1,1)(
          // CR(choose_indices_one_based(p, std::vector{1,2,3,4})),
          // CR(choose_indices_one_based(p, std::vector{1,4,3,2}))
          CR(a[0], a[1], a[2], a[3]),
          CR(a[0], a[3], a[2], a[1])
        )
      ));
    })
  );
}

// PolylogSpace H(int weight, const XArgs& xargs) {
//   auto args = xargs.as_x();
//   const X special_point = args.back();
//   args.pop_back();
//   PolylogSpace ret;
//   for (const auto& seq : cartesian_product(args, weight + 2)) {
//     auto expr = IAlt(concat({special_point}, seq));
//     CHECK(expr.is_zero() || expr.weight() == weight);
//     ret.push_back(wrap_shared(std::move(expr)));
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
        ret.push_back(wrap_shared(shuffle_product_expr(*a, *b)));
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
  PolylogSpace ret;
  for (const int alphabet_size : range(2, args.size() - 1)) {
    const X last_arg = args[alphabet_size];
    append_vector(
      ret,
      mapped_parallel(
        get_lyndon_words(slice(args, 0, alphabet_size), weight),
        [last_arg](const auto& word) {
          return wrap_shared(Corr(concat(word, {last_arg})));
        }
      )
    );
  }
  return ret;
}

// TODO: Rename to `LInf`; use the old definition for testing and maybe as an optimization.
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
          return wrap_shared(CorrAlt(concat(
            std::vector{special_point},
            word,
            std::vector{last_arg}
          )));
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
          return wrap_shared(tensor_product(absl::MakeConstSpan(
            mapped(word, DISAMBIGUATE(cross_ratio))
          )));
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
          return wrap_shared(tensor_product(absl::MakeConstSpan(
            mapped(word, [](const auto& d) { return D(d.first, d.second); })
          )));
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
    for (const auto& word : cartesian_product(triangulation, weight)) {
      ret.push_back(wrap_shared(tensor_product(absl::MakeConstSpan(
        mapped(word, [](const auto& d) { return D(d.first, d.second); })
      ))));
    }
  }
  return ret;
}


PolylogSpacePair polylog_space_3(const XArgs& args) {
  PolylogSpacePair ret;
  for (const auto& s1 : CB2(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

PolylogSpacePair polylog_space_4(const XArgs& args) {
  PolylogSpacePair ret;
  for (const auto& s1 : CB3(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : CB2(args)) {
    for (const auto& s2 : CB2(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

PolylogSpacePair polylog_space_5(const XArgs& args) {
  PolylogSpacePair ret;
  for (const auto& s1 : CL4(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : CB3(args)) {
    for (const auto& s2 : CB2(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

PolylogSpacePair polylog_space_6(const XArgs& args) {
  PolylogSpacePair ret;
  for (const auto& s1 : CL5(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : CL4(args)) {
    for (const auto& s2 : CB2(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : CB3(args)) {
    for (const auto& s2 : CB3(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

PolylogSpacePair polylog_space_6_alt(const XArgs& args) {
  PolylogSpacePair ret;
  for (const auto& s1 : CL5Alt(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : CL4(args)) {
    for (const auto& s2 : CB2(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : CB3(args)) {
    for (const auto& s2 : CB3(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

PolylogSpacePair polylog_space_6_via_l(const XArgs& args) {
  PolylogSpacePair ret;
  for (const auto& s1 : CL5(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : LInf(4, args)) {
    for (const auto& s2 : CB2(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : LInf(3, args)) {
    for (const auto& s2 : LInf(3, args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

PolylogNCoSpace polylog_space_ql_wedge_ql(int weight, const XArgs& xargs) {
  PolylogNCoSpace ret;
  for (int w : range(1, weight)) {
    const auto space_a = QL(w, xargs.as_x());
    const auto space_b = QL(weight - w, xargs.as_x());
    for (const auto& a : space_a) {
      for (const auto& b : space_b) {
        ret.push_back(wrap_shared(ncoproduct(*a, *b)));
      }
    }
  }
  return ret;
}


Matrix polylog_space_matrix(int weight, const XArgs& points, bool apply_comult) {
  return compute_polylog_space_matrix(
    polylog_space(weight)(points),
    [&](const auto& s) {
      const auto& [s1, s2] = s;
      const auto prod = ncoproduct(*s1, *s2);
      return apply_comult ? ncomultiply(prod) : prod;
    }
  );
}

Matrix polylog_space_matrix_6_via_l(const XArgs& points, bool apply_comult) {
  return compute_polylog_space_matrix(
    polylog_space_6_via_l(points),
    [&](const auto& s) {
      const auto& [s1, s2] = s;
      const auto prod = ncoproduct(*s1, *s2);
      return apply_comult ? ncomultiply(prod) : prod;
    }
  );
}

std::string polylog_spaces_kernel_describe(const PolylogNCoSpace& space) {
  Profiler profiler(false);
  const auto whole_space = compute_polylog_space_matrix(space, [](const auto& s) { return *s; });
  profiler.finish("whole space");
  const int whole_dim = matrix_rank(whole_space);
  profiler.finish("whole dim");
  const auto image_space = compute_polylog_space_matrix(space, [](const auto& s) { return ncomultiply(*s); });
  profiler.finish("image space");
  const int image_dim = matrix_rank(image_space);
  profiler.finish("image dim");
  const int kernel_dim = whole_dim - image_dim;
  return absl::StrCat(whole_dim, " - ", image_dim, " = ", kernel_dim);
}
