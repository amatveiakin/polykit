// TODO: Reduce the number of normalizations and organize them properly.

#pragma once

#include "format.h"
#include "lira_ones.h"
#include "permutation.h"
#include "sequence_iteration.h"
#include "util.h"


using Loops = std::vector<std::vector<int>>;

class LoopsInvariant {
public:
  LoopsInvariant(std::vector<int> data) : data_(std::move(data)) {}
  bool operator==(const LoopsInvariant& other) const { return data_ == other.data_; }
  template <typename H>
  friend H AbslHashValue(H h, const LoopsInvariant& v) { return H::combine(std::move(h), v.data_); }
private:
  std::vector<int> data_;
};


std::string loops_description(const Loops& loops);
LoopsInvariant loops_invariant(const Loops& loops);

struct LoopKindInfo {
  Loops representative;
  bool killed_by_symmetrization = false;
  bool killed_by_antisymmetrization = false;
  int index = 0;
  bool index_is_stable = false;
};

class LoopKinds {
public:
  // Thread-unsafe: Generates loop kinds on the fly.
  const LoopKindInfo& loops_kind(const Loops& loops);
  int loops_index(const Loops& loops);
  std::string loops_name(const Loops& loops);
  int total_kinds() const { return kinds_.size(); }
  const std::vector<LoopKindInfo>& kinds() const { return kinds_; }
  void list_all_kinds(std::ostream& os) const;

  // Do not use directly. Use `this.loops_kind` or `generate_loops_names` instead.
  const LoopKindInfo& generate_loops_kind(const Loops& loops, bool index_is_stable);

private:
  std::vector<LoopKindInfo> kinds_;
  absl::flat_hash_map<LoopsInvariant, int> invariant_to_kind_;
};

extern LoopKinds loop_kinds;

std::string pretty_print_loop_kind_index(const LoopKindInfo& kind);
std::string pretty_print_loop_kind_index(int index, bool index_is_stable);


// TODO: Rename LoopExpr -> LoopsExpr (also function names including `loop_expr`)
struct LoopExprParam : public SimpleLinearParam<Loops> {
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::monostate element_uniformity_marker(const std::vector<int>&) { return {}; }
  static std::string object_to_string(const ObjectT& loops);
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
};

using LoopExpr = Linear<LoopExprParam>;

struct LoopIndexExprParam : SimpleLinearParam<int> {
  static std::string object_to_string(const ObjectT& index) {
    return pretty_print_loop_kind_index(index, true);
  }
};

using LoopIndexExpr = Linear<LoopIndexExprParam>;

void generate_loops_names(const std::vector<LoopExpr>& expressions);

LoopExpr lira_expr_to_loop_expr(const LiraExpr& expr);  // will NOT group five-term relations
LiraExpr loop_expr_to_lira_expr(const LoopExpr& expr);

LoopIndexExpr loop_expr_to_index_symmetrized(const LoopExpr& expr);
LoopIndexExpr loop_expr_to_index_antisymmetrized(const LoopExpr& expr);

// Sorts elements within each loop, adjusting signs accordingly.
LoopExpr fully_normalize_loops(const LoopExpr& expr);

// Removes terms containing a loop with two copies of the same variable.
LoopExpr remove_loops_with_duplicates(const LoopExpr& expr);

// Removes terms where one loop is equal to or included in another one.
LoopExpr remove_duplicate_loops(const LoopExpr& expr);

LoopExpr loop_expr_simplify(const LoopExpr& expr);
LoopExpr loop_expr_substitute(const LoopExpr& expr, const absl::flat_hash_map<int, int>& substitutions);
LoopExpr loop_expr_substitute(const LoopExpr& expr, const std::vector<int>& new_indices);
LoopExpr loop_expr_cycle(const LoopExpr& expr, const Permutation& perm);  // TODO: Rename "cycle" part
LoopExpr loop_expr_degenerate(const LoopExpr& expr, const std::vector<std::vector<int>>& groups);

LoopExpr loop_expr_expand_len_6_loop_into_sum(const Loops& loops);

// Returns length of each loop (e.g. {{1,2,3,4}, {1,2,5,3}, {1,2,5,6,7}} -> {4, 4, 5}).
std::vector<int> loop_lengths(const Loops& loops);

// Returns variable that are common for all given loop, but not for a larger subset of loops.
std::vector<int> loops_unique_common_variable(const Loops& loops, std::vector<int> loop_indices);

// Lists variables accoring to which loops they belong to.
std::vector<int> decompose_loops(const Loops& loops);

// Reorders variables within each term in a way which is consistent for each equivalence
// class w.r.t. permutations.
LoopExpr to_canonical_permutation(const LoopExpr& expr);

// Keeps only terms of a specific type based on `loops_names`.
LoopExpr loop_expr_keep_term_type(const LoopExpr& expr, int type);

// Returns the substitution that turns `from` to `to`, or `nullopt` if it doesn't exist.
std::optional<absl::flat_hash_map<int, int>> loop_expr_recover_substitution(const Loops& from, const Loops& to);

// Sign of the permutation obtained by flattening the loops and deduplicating variables,
//   e.g. {{2,1,4,3}, {2,1,5,4}, {2,1,5,7,6,4}} -> {2,1,4,3,5,7,6} -> -1
int loops_united_permutation_sign(const Loops& loops);

// Sorts elements within each function argument, adjusting signs accordingly.
LiraExpr lira_expr_sort_args(const LiraExpr& expr);

// TODO: Dedup against loops_Q / loops_S.
LoopExpr cut_loops(const std::vector<int>& points);

LoopExpr reverse_loops(const LoopExpr& expr);

LoopExpr loops_Q(const std::vector<int>& points);
LoopExpr loops_S(const std::vector<int>& points);

// There must be 5 vars: 1,2,3,4,5. Each loop must be sorted.
LoopExpr loops_var5_shuffle_internally(const LoopExpr& expr);

// TODO: Clean up this ad hoc function.
// Generalization idea: find a way to apply shuffle relations in order to get a
// deterministic basis with minimal (or close to minimal) number of terms.
LoopExpr arg9_semi_lyndon(const LoopExpr& expr);

// Generates ways of gluing variables 1, 2, ..., `num_vars` where:
//   - Variable pairs are glued exactly `num_gluings` times. The gluings could be applied to
//     different variables or to the same variable repeatedly.
//     For example `num_gluings` == 2 could produce both {{1,3,7}} and {{1,3}, {4,6}}.
//   - Neighbor variables (1 and 2, 2 and 3, ..., `num_vars` and 1) are never glued.
//   - Degenerations are factored by dihedral symmetries. Each cluster is represented by the
//     lexicographically smallest degeneration.
std::vector<std::vector<std::vector<int>>> make_degenerations(int num_vars, int num_gluings);
