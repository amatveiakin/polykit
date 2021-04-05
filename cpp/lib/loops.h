#pragma once

#include "format.h"
#include "lira_ones.h"
#include "sequence_iteration.h"
#include "util.h"


// TODO: Strong typing and proper group operations for `cycles`.

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

class LoopsNames {
public:
  int loops_index(const Loops& loops);
  std::string loops_name(const Loops& loops);
  int total_names() const { return next_idx_; }

private:
  int next_idx_ = 1;
  absl::flat_hash_map<LoopsInvariant, int> indices_;
};

extern LoopsNames loops_names;


// TODO: Rename LoopExpr -> LoopsExpr (also function names including `loop_expr`)
struct LoopExprParam : public SimpleLinearParam<Loops> {
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  static std::string object_to_string(const ObjectT& loops);
};

using LoopExpr = Linear<LoopExprParam>;

// TODO: Force using this, disable on-the-fly name generation (?)
void generate_loops_names(const std::vector<LoopExpr>& expressions);

LoopExpr lira_expr_to_loop_expr(const LiraExpr& expr);  // will NOT group five-term relations
LiraExpr loop_expr_to_lira_expr(const LoopExpr& expr);

// Sorts elements within each loop, adjusting signs accordingly.
LoopExpr fully_normalize_loops(const LoopExpr& expr);

// Removes terms where one loop is equal to or included in another one.
LoopExpr remove_duplicate_loops(const LoopExpr& expr);

LoopExpr loop_expr_substitute(const LoopExpr& expr, const absl::flat_hash_map<int, int>& substitutions);
LoopExpr loop_expr_substitute(const LoopExpr& expr, const std::vector<int>& new_indices);
LoopExpr loop_expr_cycle(const LoopExpr& expr, const std::vector<std::vector<int>>& cycles);
LoopExpr loop_expr_degenerate(const LoopExpr& expr, const std::vector<std::vector<int>>& groups);

// Returns variable that are common for all given loop, but not for a larger subset of loops.
std::vector<int> loops_unique_common_variable(const Loops& loops, std::vector<int> loop_indices);

// Lists variables accoring to which loops they belong to.
std::vector<int> decompose_loops(const Loops& loops);

// Reorders variables within each term in a way which is consistent for each equivalence
// class w.r.t. permutations.
LoopExpr to_canonical_permutation(const LoopExpr& expr);

// Sorts elements within each function argument, adjusting signs accordingly.
LiraExpr lira_expr_sort_args(const LiraExpr& expr);

LoopExpr cut_loops(const std::vector<int>& points);

LoopExpr reverse_loops(const LoopExpr& expr);

// There must be 5 vars: 1,2,3,4,5. Each loop must be sorted.
LoopExpr loops_var5_shuffle_internally(const LoopExpr& expr);


// TODO: Clean up this super ad hoc function.
LoopExpr arg9_semi_lyndon(const LoopExpr& expr);
