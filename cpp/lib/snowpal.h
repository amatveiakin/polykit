#pragma once

#include "format.h"
#include "lira_ones.h"
#include "ratio.h"
#include "sequence_iteration.h"
#include "set_util.h"


class IllegalTreeCutException : public std::exception {
public:
  IllegalTreeCutException(std::string msg) : msg_(msg) {}
  const char* what() const noexcept override { return msg_.c_str(); }
private:
  std::string msg_;
};


struct ShortFormRatio {
  std::string letter;
  int node_index;
  CrossRatioN normal_form;
  absl::flat_hash_map<CrossRatioN, int> usage_stats;
};

class ShortFormRatioStorage {
public:
  ShortFormRatioStorage(const std::vector<ShortFormRatio>& ratios);
  std::optional<ShortFormRatio> get_short_form_ratio(CrossRatioN ratio) const;
  void record_usage_stats(CrossRatioN usage);
  void update_normal_forms();

private:
  // key is sorted form
  absl::flat_hash_map<std::array<int, 4>, ShortFormRatio> ratios_;
};


class SplittingTree {
public:
  struct Node {
    const Node* const parent = nullptr;
    const std::vector<int> points;
    const int node_index = 0;
    std::vector<std::unique_ptr<Node>> children;

    Node(Node* parent_arg, std::vector<int> points_arg, int node_index_arg)
      : parent(parent_arg), points(std::move(points_arg)), node_index(node_index_arg) {}

    void split(const std::vector<int>& subpoints, SplittingTree& tree);
    int valency() const;  // == `nbr_indices().size()`
    std::vector<std::vector<int>> nbr_indices() const;
    int nbr_idx_for_point(int point) const;
    int metavar_for_point(int point) const;
    std::vector<int> complement_to_children() const;
    Node* descend(const std::vector<int>& subpoints) const;
  };

  SplittingTree(int num_variables);

  Node*       root()       { return root_.get(); }
  const Node* root() const { return root_.get(); }

  Node* node_for_points(const std::vector<int>& points);
  int new_node_index();
  ShortFormRatioStorage generate_short_form_ratios() const;
  std::vector<std::pair<int, std::vector<std::vector<int>>>> dump_nbr_indices() const;

  template<typename Func>
  void foreach_node(const Func& func) const {
    foreach_node_impl(func, root_.get());
  }

private:
  template<typename Func>
  void foreach_node_impl(const Func& func, const Node* node) const {
    func(node);
    for (const auto& child : node->children) {
      foreach_node_impl(func, child.get());
    }
  }

  int next_node_index_ = 0;
  std::unique_ptr<Node> root_;
};


using Substitution = std::array<const SplittingTree::Node*, 2>;

struct ZeroOrInf {};

using RatioSubstitutionResult = std::variant<ZeroOrInf, CrossRatioNOrUnity>;


// Returns one of the two things:
//
//     *       *
//       \   /
//         *    <--- this vertex
//       /   \         if the graph looks like this
//     *       *
//
//                OR
//
//   *               *  <---
//     \           /       |
//       * ----- *         |--- these points
//     /           \       |      if the graph look like this
//   *               *  <---
//
std::variant<const SplittingTree::Node*, std::array<int, 2>> find_central_node(
    const std::vector<int>& points,
    const SplittingTree::Node* node);

RatioSubstitutionResult ratio_substitute(
    const CrossRatioNOrUnity& ratio,
    const SplittingTree& tree);

LiraExpr lira_expr_substitute(
    const LiraExpr& expr,
    const SplittingTree& tree);


class Snowpal {
public:
  Snowpal(LiraExpr expr, int num_variables);

  const LiraExpr& expr() const { return expr_; };
  const SplittingTree& splitting_tree() const { return splitting_tree_; }

  Snowpal& add_ball(std::vector<int> points);

private:
  SplittingTree splitting_tree_;
  LiraExpr orig_expr_;
  LiraExpr expr_;
};

std::ostream& to_ostream(
    std::ostream& os, const LiraExpr& expr, const SplittingTree& splitting_tree,
    bool short_form_ratios);
std::ostream& to_ostream(std::ostream& os, const Snowpal& snowpal, bool short_form_ratios);
std::ostream& operator<<(std::ostream& os, const Snowpal& snowpal);
