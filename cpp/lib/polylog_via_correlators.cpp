#include "polylog_via_correlators.h"

#include "absl/container/flat_hash_set.h"

#include "iterated_integral.h"
#include "sequence_iteration.h"


static bool contains_var_from_each_pair_odd(const std::vector<int>& args, int num_vars) {
  absl::flat_hash_set<int> args_set(args.begin(), args.end());
  CHECK(num_vars % 2 == 0);
  for (int var = 1; var <= num_vars; var += 2) {
    if (!(args_set.contains(var) || args_set.contains(var+1))) {
      return false;
    }
  }
  return true;
}

static bool contains_var_from_each_pair_even(const std::vector<int>& args, int num_vars) {
  absl::flat_hash_set<int> args_set(args.begin(), args.end());
  CHECK(num_vars % 2 == 0);
  for (int var = 1; var <= num_vars; var += 2) {
    if (!(args_set.contains(var+1) || args_set.contains((var+1)%num_vars+1))) {
      return false;
    }
  }
  return true;
}


CorrExpr CorrQLiImpl(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = (num_vars + 1) / 2;
  CorrExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair_odd(args, num_vars)) {
      continue;
    }
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + num_args);
      ret.add_to(CorrFSymb{args}, coeff);
    }
  }
  return ret;
}

CorrExpr CorrQLiNegAltImpl(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = div_int(num_vars, 2);
  CorrExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair_even(args, num_vars)) {
      continue;
    }
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + 1);
      ret.add_to(CorrFSymb{args}, coeff);
    }
  }
  return ret;
}

CorrExpr CorrQLiSymmImpl(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = div_int(num_vars, 2);
  CorrExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + num_args);
      ret.add_to(CorrFSymb{args}, coeff);
    }
  }
  return ret;
}


CorrExpr CorrQLi(int weight, const std::vector<int>& points) {
  return substitute_variables(
    CorrQLiImpl(
      weight,
      points.size()
    ),
    points
  ).annotate(fmt::function_num_args(
    fmt::sub_num(fmt::opname("CorrQLi"), {weight}),
    points
  ));
}

CorrExpr CorrQLiNeg(int weight, const std::vector<int>& points) {
  return -CorrQLi(
    weight, rotated_vector(points, 1)
  ).without_annotations().annotate(fmt::function_num_args(
    fmt::sub_num(fmt::super(fmt::opname("CorrQLi"), {"-"}), {weight}),
    points
  ));
}

CorrExpr CorrQLiNegAlt(int weight, const std::vector<int>& points) {
  return substitute_variables(
    CorrQLiNegAltImpl(
      weight,
      points.size()
    ),
    points
  ).annotate(fmt::function_num_args(
    fmt::sub_num(fmt::super(fmt::opname("CorrQLi"), {"-"}), {weight}),
    points
  ));
}

CorrExpr CorrQLiSymm(int weight, const std::vector<int>& points) {
  return substitute_variables(
    CorrQLiSymmImpl(
      weight,
      points.size()
    ),
    points
  ).annotate(fmt::function_num_args(
    fmt::sub_num(fmt::opname("CorrQLiSymm"), {weight}),
    points
  ));
}
