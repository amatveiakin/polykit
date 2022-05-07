#include "polylog_type_ac_space.h"

#include "iterated_integral.h"
#include "itertools.h"
#include "parallel_util.h"
#include "polylog_qli.h"
#include "triangulation.h"
#include "vector_space.h"


TypeAC_Space CB_naive_via_QLi_fours(int weight, const XArgs& xargs) {
  return mapped(combinations(xargs.as_x(), 4), [&](const auto& p) { return QLiVec(weight, p); });
}

TypeAC_Space CB(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  switch (weight) {
    case 1: {
      const int n = args.size();
      TypeAC_Space ret;
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

TypeAC_Space CL(int weight, const XArgs& xargs) {
  if (weight <= 2) {
    return CB(weight, xargs);
  }
  const auto& args = xargs.as_x();
  TypeAC_Space space;
  const int max_args = std::min<int>(args.size(), (weight / 2 + 1) * 2);
  for (int num_args = 4; num_args <= max_args; num_args += 2) {
    append_vector(space, mapped(combinations(args, num_args), [&](const auto& p) {
      return QLiVec(weight, p);
    }));
  }
  return space;
}

TypeAC_Space old_CL4_via_A2(const XArgs& args) {
  return concat(
    CB4(args),
    mapped(combinations(args.as_x(), 5), [](const auto& p) { return A2(p); })
  );
}


template<typename LSpace>
TypeAC_Space typeC_space(int weight, const XArgs& xargs, const LSpace& lspace) {
  auto args = xargs.as_x();
  CHECK(inv_points_are_central_symmetric(args)) << dump_to_string(args);
  if (args.size() == 4) {
    return lspace(weight, args);
  }
  TypeAC_Space space;
  CHECK(args.size() % 2 == 0);
  CHECK_GT(args.size(), 4);
  const int half_num_points = args.size() / 2;
  // Cut into two halves, take one size (the other size is the same).
  for (const int i : range(half_num_points)) {
    const int n = half_num_points + 1;
    const auto points = slice(args, i, i + n);
    CHECK_EQ(points.size(), n);
    append_vector(space, lspace(weight, points));
  }
  // Discard opposite points and define recursively.
  for (const int i : range(half_num_points)) {
    const auto points = removed_indices(args, {i, i + half_num_points});
    append_vector(space, typeC_space(weight, points, lspace));
  }
  return space;
}

TypeAC_Space typeC_CL(int weight, const XArgs& xargs) {
  return typeC_space(weight, xargs, DISAMBIGUATE(CL));
}

TypeAC_Space typeC_CB(int weight, const XArgs& xargs) {
  return typeC_space(weight, xargs, DISAMBIGUATE(CB));
}


// TypeAC_Space H(int weight, const XArgs& xargs) {
//   auto args = xargs.as_x();
//   const X special_point = args.back();
//   args.pop_back();
//   TypeAC_Space ret;
//   for (const auto& seq : cartesian_power(args, weight + 2)) {
//     auto expr = IAlt(concat({special_point}, seq));
//     CHECK(expr.is_zero() || expr.weight() == weight);
//     ret.push_back(std::move(expr));
//   }
//   return ret;
// }

TypeAC_Space H(int weight, const XArgs& xargs) {
  TypeAC_Space ret = L(weight, xargs);
  for (int w : range_incl(1, weight / 2)) {
    TypeAC_Space space_a = H(w, xargs);
    TypeAC_Space space_b = H(weight - w, xargs);
    for (const auto& a : space_a) {
      for (const auto& b : space_b) {
        ret.push_back(shuffle_product_expr(a, b));
      }
    }
  }
  return ret;
}

TypeAC_Space Fx(const XArgs& xargs) {
  return mapped(combinations(xargs.as_x(), 2), [](const auto& p) {
    const auto [a, b] = to_array<2>(p);
    return D(a, b);
  });
}

TypeAC_Space LInf(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  // Note: See tests for alternative definitions that support arbitrary arguments, but have duplicates.
  // Note: See L for alternative definitions that support arbitrary arguments.
  CHECK(!args.empty() && args.back() == Inf) << dump_to_string(args);
  // TODO: Is this how it should be defined for weight 1?
  if (weight == 1) {
    return CB1(xargs);
  }
  TypeAC_Space ret;
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

TypeAC_Space L(int weight, const XArgs& xargs) {
  if (weight == 1) {
    return CB1(xargs);
  }
  auto args = xargs.as_x();
  const X special_point = args.back();
  args.pop_back();
  TypeAC_Space ret;
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

TypeAC_Space XCoords(int weight, const XArgs& args) {
  const auto cluster_coordinates_set = get_triangulation_quadrangles(args.as_x());
  TypeAC_Space ret;
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

TypeAC_Space ACoords(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  const auto triangulations = get_triangulations(args);
  TypeAC_Space ret;
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

TypeAC_Space ACoordsHopf(int weight, const XArgs& xargs) {
  const auto& args = xargs.as_x();
  const auto triangulations = get_triangulations(args);
  TypeAC_Space ret;
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


TypeAC_NCoSpace simple_co_L(int weight, int num_coparts, int num_points) {
  const auto points = to_vector(range_incl(1, num_points));
  return co_space(weight, num_coparts, [&](const int w) {
    return mapped(L(w, points), [&](const auto& expr) {
      // Precompute Lyndon basis to speed up coproduct.
      return to_lyndon_basis(normalize_remove_consecutive(expr));
    });
  });
}

TypeAC_NCoSpace co_CL(int weight, int num_coparts, const XArgs& xargs) {
  const auto points = xargs.as_x();
  return co_space(weight, num_coparts, [&](const int w) {
    // Precompute Lyndon basis to speed up coproduct.
    return mapped(CL(w, points), DISAMBIGUATE(to_lyndon_basis));
  });
}
