#include "polylog_via_correlators.h"

#include "absl/container/flat_hash_set.h"

#include "iterated_integral.h"
#include "sequence_iteration.h"


// TODO: Try to reduce the amount of copy-paste

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

DeltaExpr QLiViaCorr(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = (num_vars + 1) / 2;
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair_odd(args, num_vars)) {
      continue;
    }
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + num_args);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
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

// TODO: Update definition and cyclic shift!
DeltaExpr QLiNegViaCorr(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = div_int(num_vars, 2);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair_even(args, num_vars)) {
      continue;
    }
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + 1);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}

DeltaExpr QLiNegAltViaCorr(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_even_vars = div_int(num_vars, 2);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair_odd(args, num_vars)) {
      continue;
    }
    const auto even_args = even_elements(args);
    if (even_args.size() == num_distinct_elements(even_args)) {
      const int missing_even = total_even_vars - even_args.size();
      const int coeff = neg_one_pow(missing_even + 1);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}


DeltaExpr QLiSymmViaCorr(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = div_int(num_vars, 2);
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    const auto odd_args = odd_elements(args);
    if (odd_args.size() == num_distinct_elements(odd_args)) {
      const int missing_odd = total_odd_vars - odd_args.size();
      const int coeff = neg_one_pow(missing_odd + num_args);
      ret += coeff * CorrVec(mapped(args, [](int x) { return X(x); }));
    }
  }
  return ret;
}



// TODO: remove duplicate code
CorrExpr QLiViaCorrFSymb(int weight, int num_vars) {
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

CorrExpr QLiNegViaCorrFSymb(int weight, int num_vars) {
  return -corr_expr_substitute(
    QLiViaCorrFSymb(weight, num_vars),
    concat(seq_incl(2, num_vars), {1})
  );
}

CorrExpr PosCorrFSymb(int weight, const std::vector<int>& points) {
  return corr_expr_substitute(
    QLiViaCorrFSymb(
      weight,
      points.size()
    ),
    points
  ).annotate(fmt::function_num_args(
    fmt::sub_num(fmt::opname("PosCorr"), {weight}),
    points
  ));
}

CorrExpr NegCorrFSymb(int weight, const std::vector<int>& points) {
  return corr_expr_substitute(
    QLiNegViaCorrFSymb(
      weight,
      points.size()
    ),
    points
  ).annotate(fmt::function_num_args(
    fmt::sub_num(fmt::opname("NegCorr"), {weight}),
    points
  ));
}
