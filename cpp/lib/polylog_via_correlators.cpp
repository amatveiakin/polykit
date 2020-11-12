#include "polylog_via_correlators.h"

#include "absl/container/flat_hash_set.h"

#include "iterated_integral.h"
#include "sequence_iteration.h"


bool contains_var_from_each_pair(const std::vector<int>& args, int num_vars) {
  absl::flat_hash_set<int> args_set(args.begin(), args.end());
  CHECK(num_vars % 2 == 0);
  for (int var = 1; var <= num_vars; var += 2) {
    if (!(args_set.contains(var) || args_set.contains(var+1))) {
      return false;
    }
  }
  return true;
}

DeltaExpr LidoViaCorr(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = (num_vars + 1) / 2;
  DeltaExpr ret;
  for (const auto& w : nondecreasing_sequences(num_vars, num_args)) {
    const auto args = mapped(w, [](int x) { return x + 1; });
    if (!contains_var_from_each_pair(args, num_vars)) {
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

DeltaExpr LidoSymmViaCorr(int weight, int num_vars) {
  const int num_args = weight + 1;
  const int total_odd_vars = (num_vars + 1) / 2;
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
