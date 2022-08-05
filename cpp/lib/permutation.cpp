#include "permutation.h"

#include "absl/container/flat_hash_set.h"

#include "format.h"
#include "util.h"


Permutation::Permutation(std::initializer_list<std::initializer_list<int>> cycles)
  : Permutation(mapped_nested<2>(cycles, identity_function)) {}

Permutation::Permutation(std::vector<std::vector<int>> cycles) : cycles_(cycles) {
  std::vector<int> vars;
  for (const auto& cycle : cycles_) {
    CHECK_LT(1, cycle.size());
    append_vector(vars, cycle);
  }
  CHECK(all_unique_unsorted(vars)) << dump_to_string(cycles);
}

absl::flat_hash_map<int, int> Permutation::substitutions() const {
  absl::flat_hash_map<int, int> ret;
  for (const auto& cycle : cycles_) {
    const int n = cycle.size();
    for (const int i : range(n)) {
      const int p = cycle[i];
      const int q = cycle[(i+1) % n];
      CHECK(!ret.contains(p));
      ret[p] = q;
    }
  }
  return ret;
}

std::vector<int> Permutation::applied(const std::vector<int>& v) const {
  const auto subst = substitutions();
  return mapped(v, [&](int p) {
    const auto it = subst.find(p);
    return it != subst.end() ? it->second : p;
  });
}

static std::vector<int> choose_subsequence(const std::vector<int>& cycle, int start, int step) {
  CHECK_LT(start, step);
  std::vector<int> ret;
  const int n = cycle.size();
  for (int i = start; i < n; i += step) {
    ret.push_back(cycle[i]);
  }
  return ret;
}

static std::vector<int> cycle_coprime_pow(const std::vector<int>& cycle, int exponent) {
  std::vector<int> ret;
  const int n = cycle.size();
  for (const int i : range(n)) {
    ret.push_back(cycle[(i * exponent) % n]);
  }
  return ret;
}

Permutation Permutation::pow(int exponent) const {
  CHECK_LE(0, exponent);
  if (exponent == 0) {
    return Permutation();
  }
  std::vector<std::vector<int>> ret;
  for (const auto& cycle : cycles_) {
    const int n = cycle.size();
    if (exponent % n == 0) {
      continue;
    }
    const auto d = std::gcd(n, exponent);
    for (const int i : range(d)) {
      ret.push_back(cycle_coprime_pow(choose_subsequence(cycle, i, d), exponent/d));
    }
  }
  return Permutation(ret);
}


std::string to_string(const Permutation& perm) {
  return str_join(
    mapped(perm.cycles(), [&](const std::vector<int>& loop) {
      return fmt::parens(str_join(loop, ","));
    }),
    ""
  );
}
