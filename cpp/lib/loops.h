#pragma once

#include "format.h"
#include "snowpal.h"
#include "util.h"


// TODO: Rename to ~set_intersection and move to utils (or separate file)
inline std::vector<int> common_elements(std::vector<std::vector<int>> sets) {
  std::vector<int> ret(sets.front().begin(), sets.front().end());
  absl::c_sort(ret);
  for (auto one_set : sets) {
    absl::c_sort(one_set);
    std::vector<int> new_ret;
    absl::c_set_intersection(ret, one_set, std::back_inserter(new_ret));
    ret = std::move(new_ret);
  }
  return ret;
}

inline int num_common_elements(std::vector<std::vector<int>> sets) {
  return common_elements(sets).size();
}


using Loops = std::vector<std::vector<int>>;
// TODO: Strong typing (here and for invariant types in general)
using LoopsInvariant = std::vector<int>;


std::string loops_description(const Loops& loops);
LoopsInvariant loops_invariant(Loops loops);

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


struct LoopExprParam : public SimpleLinearParam<Loops> {
  static std::string object_to_string(const ObjectT& loops);
};

using LoopExpr = Linear<LoopExprParam>;

// TODO: Force using this, disable on-the-fly name generation (?)
void generate_loops_names(const std::vector<LoopExpr>& expressions);

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


// TODO: Clean up this super ad hoc function.
LoopExpr arg9_semi_lyndon(const LoopExpr& expr);
