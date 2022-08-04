#pragma once

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"


class Permutation {
public:
  Permutation() = default;
  Permutation(std::initializer_list<std::initializer_list<int>> cycles);
  explicit Permutation(std::vector<std::vector<int>> cycles);

  bool is_identity() const { return cycles_.empty(); }
  const std::vector<std::vector<int>>& cycles() const { return cycles_; }
  absl::flat_hash_map<int, int> substitutions() const;

  // Applies permutation to each value in `v` separately (*not* to positions).
  // Note. If applying one permutation multiple times, it is faster to construct
  //   `substitutions` once and reuse them.
  std::vector<int> applied(const std::vector<int>& v) const;

  // Permutation operator*(const Permutation& other) const;  // TODO: implement
  Permutation pow(int p) const;

private:
  std::vector<std::vector<int>> cycles_;
};


std::string to_string(const Permutation& perm);
