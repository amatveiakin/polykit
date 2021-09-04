// TODO: Test !!! (some reference values in https://arxiv.org/pdf/1401.6446.pdf)
// TODO: Are shared pointers a good idea? This causes sharing between threads !!!

#include "polylog_cluster.h"

#include <future>

#include "expr_matrix_builder.h"
#include "polylog_lira.h"
#include "polylog_qli.h"


ClusterSpace CB1(const XArgs& xargs) {
  const auto& args = xargs.as_x();
  const int n = args.size();
  ClusterSpace ret;
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
ClusterSpace CB2(const XArgs& args) {
  const auto head = slice(args.as_x(), 0, 1);
  const auto tail = slice(args.as_x(), 1);
  return mapped(combinations(tail, 3), [&](const auto& p) { return wrap_shared(QLi2(concat(head, p))); });
}
ClusterSpace CB3(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return wrap_shared(QLi3(p)); });
}
ClusterSpace CB4(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return wrap_shared(QLi4(p)); });
}
ClusterSpace CB5(const XArgs& args) {
  return mapped(combinations(args.as_x(), 4), [](const auto& p) { return wrap_shared(QLi5(p)); });
}

ClusterSpace CL4(const XArgs& args) {
  return concat(
    CB4(args),
    mapped(combinations(args.as_x(), 5), [](const auto& p) { return wrap_shared(A2(p)); })
  );
}
ClusterSpace CL5(const XArgs& args) {
  return concat(
    CB5(args),
    mapped(combinations(args.as_x(), 6), [](const auto& p) { return wrap_shared(QLi5(p)); })
  );
}
ClusterSpace CL5Alt(const XArgs& args) {
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

ClusterSpace L3(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi3(choose_indices_one_based(p, {1,2,3,4})));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi3(choose_indices_one_based(p, {1,3,2,4})));
    })
  );
}
ClusterSpace L4(const XArgs& args) {
  return concat(
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,2,3,4})));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,3,4,2})));
    }),
    mapped(combinations(args.as_x(), 4), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,4,2,3})));
    }),
    mapped(permutations(args.as_x(), 5), [](const auto& p) {
      return wrap_shared(QLi4(choose_indices_one_based(p, {1,2,1,3,4,5})));
    })
  );
}

ClusterCoSpace cluster_space_3(const XArgs& args) {
  ClusterCoSpace ret;
  for (const auto& s1 : CB2(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}

ClusterCoSpace cluster_space_4(const XArgs& args) {
  ClusterCoSpace ret;
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

ClusterCoSpace cluster_space_5(const XArgs& args) {
  ClusterCoSpace ret;
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

ClusterCoSpace cluster_space_6(const XArgs& args) {
  ClusterCoSpace ret;
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

ClusterCoSpace cluster_space_6_alt(const XArgs& args) {
  ClusterCoSpace ret;
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

ClusterCoSpace cluster_space_6_via_l(const XArgs& args) {
  ClusterCoSpace ret;
  for (const auto& s1 : CL5(args)) {
    for (const auto& s2 : CB1(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : L4(args)) {
    for (const auto& s2 : CB2(args)) {
      ret.push_back({s1, s2});
    }
  }
  for (const auto& s1 : L3(args)) {
    for (const auto& s2 : L3(args)) {
      ret.push_back({s1, s2});
    }
  }
  return ret;
}


Eigen::MatrixXd cluster_space_matrix(int weight, const XArgs& points, bool apply_comult) {
  ExprMatrixBuilder<DeltaNCoExpr> matrix_builder;
  std::vector<std::future<DeltaNCoExpr>> results;
  for (const auto& s : cluster_space(weight)(points)) {
    results.push_back(std::async([s, apply_comult]() {
      const auto& [s1, s2] = s;
      const auto prod = ncoproduct(*s1, *s2);
      return apply_comult ? ncomultiply(prod) : prod;
    }));
  }
  for (auto& result : results) {
    matrix_builder.add_expr(result.get());
  }
  return matrix_builder.make_matrix<double>();
}

Eigen::MatrixXd cluster_space_matrix_6_via_l(const XArgs& points, bool apply_comult) {
  ExprMatrixBuilder<DeltaNCoExpr> matrix_builder;
  std::vector<std::future<DeltaNCoExpr>> results;
  for (const auto& s : cluster_space_6_via_l(points)) {
    results.push_back(std::async([s, apply_comult]() {
      const auto& [s1, s2] = s;
      const auto prod = ncoproduct(*s1, *s2);
      return apply_comult ? ncomultiply(prod) : prod;
    }));
  }
  for (auto& result : results) {
    matrix_builder.add_expr(result.get());
  }
  return matrix_builder.make_matrix<double>();
}
