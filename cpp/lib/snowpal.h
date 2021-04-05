#pragma once

#include "format.h"
#include "lira_ones.h"
#include "ratio.h"
#include "sequence_iteration.h"
#include "set_util.h"
#include "theta.h"


static constexpr auto kMetaVariablesColors = std::array{
  TextColor::blue,
  TextColor::green,
  TextColor::magenta,
  TextColor::cyan,
};

static constexpr int kMetaVarStart = 100;
static constexpr int kMaxNeighbours = 100;

struct MetaVar {
  int node_index;
  int nbr_index;
};

inline int make_metavar(int node_idx, int nbr_idx) {
  return kMetaVarStart + node_idx * kMaxNeighbours + nbr_idx;
}

inline bool is_metavar(int var) {
  return var >= kMetaVarStart;
}

inline MetaVar parse_metavar(int var) {
  CHECK(is_metavar(var));
  int meta_idx = var - kMetaVarStart;
  return MetaVar{meta_idx / kMaxNeighbours, meta_idx % kMaxNeighbours};
}

inline std::string metavar_to_string_by_name(int var) {
  MetaVar metavar = parse_metavar(var);
  CHECK_LE(metavar.node_index, 'z' - 'a');
  return fmt::colored(
    absl::StrCat(
      std::string(1, 'a' + metavar.node_index),
      metavar.nbr_index
    ),
    kMetaVariablesColors[metavar.node_index % kMetaVariablesColors.size()]
  );
}

template<typename MetaVarPrinter>
std::string var_to_string(int var, const MetaVarPrinter& metavar_to_string) {
  return is_metavar(var)
    ? metavar_to_string(var)
    : to_string(var);
}

template<typename MetaVarPrinter>
std::string ratio_to_string(const CrossRatioNOrUnity& r, const MetaVarPrinter& metavar_to_string) {
  return r.is_unity()
    ? fmt::unity()
    : fmt::brackets(str_join(r.as_ratio().indices(), ",", [&](int x) {
        return var_to_string(x, metavar_to_string);
      }));
}


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
  ShortFormRatioStorage(const std::vector<ShortFormRatio>& ratios) {
    for (const ShortFormRatio& r : ratios) {
      ratios_[sorted(r.normal_form.indices())] = r;
    }
  }

  std::optional<ShortFormRatio> get_short_form_ratio(CrossRatioN ratio) const {
    const auto it = ratios_.find(sorted(ratio.indices()));
    if (it != ratios_.end()) {
      return it->second;
    }
    return absl::nullopt;
  }

  void record_usage_stats(CrossRatioN usage) {
    const auto it = ratios_.find(sorted(usage.indices()));
    if (it != ratios_.end()) {
      it->second.usage_stats[usage]++;
    }
  }

  void update_normal_forms() {
    for (auto& [key, ratio] : ratios_) {
      if (!ratio.usage_stats.empty()) {
        int max_usage = 0;
        CrossRatioN max_usage_form;
        for (const auto& [form, usage_count] : ratio.usage_stats) {
          CHECK_GT(usage_count, 0);
          if (usage_count > max_usage) {
            max_usage = usage_count;
            max_usage_form = form;
          }
        }
        const CrossRatioN new_normal_form = max_usage_form;
        CHECK(sorted(ratio.normal_form.indices()) == sorted(new_normal_form.indices()));
        ratio.normal_form = new_normal_form;
      }
    }
  }

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

    void split(const std::vector<int>& subpoints, SplittingTree& tree) {
      CHECK(!subpoints.empty());
      CHECK(absl::c_is_sorted(subpoints));
      CHECK(absl::c_includes(points, subpoints)) << dump_to_string(points)
        << " does not contain " << dump_to_string(subpoints);
      children.push_back(absl::make_unique<Node>(this, subpoints, tree.new_node_index()));
      std::vector<int> remaining_vars = complement_to_children();
      constexpr int kMinValency = 3;
      if (valency() < kMinValency || children.back()->valency() < kMinValency) {
        throw IllegalTreeCutException("Valency too low");
      }
    }

    // Note: always equals `nbr_indices().size()`
    int valency() const {
      return (parent != nullptr ? 1 : 0) + complement_to_children().size() + children.size();
    }

    std::vector<std::vector<int>> nbr_indices() const {
      std::vector<std::vector<int>> ret;
      if (parent != nullptr) {
        const Node* root = parent;
        while (root->parent != nullptr) {
          root = root->parent;
        }
        const auto universum = root->points;
        std::vector<int> complement;
        absl::c_set_difference(universum, points, std::back_inserter(complement));
        ret.push_back(complement);
      }
      for (const auto& child : children) {
        ret.push_back(child->points);
      }
      for (const int p : complement_to_children()) {
        ret.push_back({p});
      }
      return ret;
    }

    int nbr_idx_for_point(int point) const {
      const auto indices = nbr_indices();
      const auto it = absl::c_find_if(indices, [&](const auto& sub_indices) {
        return contains_naive(sub_indices, point);
      });
      CHECK(it != indices.end());
      return it - indices.begin() + 1;
    }

    // Optimization potential: Use this. Should be the same, but faster.
    // int nbr_idx_for_point(int point) const {
    //   int nbr_idx = 1;
    //   if (parent != nullptr) {
    //     if (!vector_contains(points, point)) {
    //       // If we don't have the point, it must be in a (grand)parent
    //       return nbr_idx;
    //     }
    //     ++nbr_idx;
    //   }
    //   for (const auto& child : children) {
    //     if (vector_contains(child->points, point)) {
    //       return nbr_idx;
    //     }
    //     ++nbr_idx;
    //   }
    //   for (const int p : complement_to_children()) {
    //     if (p == point) {
    //       return nbr_idx;
    //     }
    //     ++nbr_idx;
    //   }
    //   FATAL("Point not found");
    // }

    int metavar_for_point(int point) const {
      return make_metavar(node_index, nbr_idx_for_point(point));
    }

    std::vector<int> complement_to_children() const {
      std::vector<int> complement = points;
      for (const auto& child : children) {
        std::vector<int> new_complement;
        absl::c_set_difference(complement, child->points, std::back_inserter(new_complement));
        complement = std::move(new_complement);
      }
      return complement;
    }

    Node* descend(const std::vector<int>& subpoints) const {
      for (const auto& child : children) {
        const int inter_size = set_intersection_size(child->points, subpoints);
        if (inter_size > 0) {
          if (inter_size != subpoints.size()) {
            throw IllegalTreeCutException(absl::StrCat(
              dump_to_string(child->points),
              " includes some but not all of ",
              dump_to_string(subpoints)
            ));
          }
          return child.get();
        }
      }
      return nullptr;
    }
  };

  SplittingTree(int num_variables)
    : root_(absl::make_unique<Node>(
        nullptr, seq_incl(1, num_variables), new_node_index())) {}

  Node*       root()       { return root_.get(); }
  const Node* root() const { return root_.get(); }

  Node* node_for_points(const std::vector<int>& points) {
    SplittingTree::Node* current = root();
    SplittingTree::Node* next = current;
    while (next != nullptr) {
      current = next;
      next = current->descend(points);
    }
    return current;
  }

  int new_node_index() {
    return next_node_index_++;
  }

  ShortFormRatioStorage generate_short_form_ratios() const {
    // static const std::vector<std::string> kLetters = {"λ", "μ", "ξ", "φ", "ψ"};
    static auto index_to_letter = [](int index) -> std::string {
      return std::string(1, 'A' + index);
    };
    int ratio_index = 0;
    std::vector<ShortFormRatio> ret;
    foreach_node([&](const Node* node) {
      const int valency = node->valency();
      if (4 <= valency && valency <= 6) {
        const std::string letter = index_to_letter(node->node_index);
        for (const auto& seq : increasing_sequences(valency, valency - 4)) {
          auto points = removed_indices(seq_incl(1, valency), seq);
          auto missing = mapped(seq, [](int x) { return x + 1; });
          ret.push_back(ShortFormRatio{
            // fmt::sub_num(letter, missing),
            letter + str_join(missing, ""),
            node->node_index,
            CrossRatioN(mapped_array(to_array<4>(points), [&](int nbr_idx) {
              return make_metavar(node->node_index, nbr_idx);
            }))
          });
        }
      }
      ++ratio_index;
    });
    return {ret};
  }

  std::vector<std::pair<int, std::vector<std::vector<int>>>> dump_nbr_indices() const {
    std::vector<std::pair<int, std::vector<std::vector<int>>>> ret;
    foreach_node([&](const Node* node) {
      ret.push_back({node->node_index, node->nbr_indices()});
    });
    return sorted(ret);
  }

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


inline std::string short_form_to_string(const ShortFormRatio& tmpl, CrossRatioN value) {
  return fmt::colored(
    dependent_cross_ratio_formula(tmpl.normal_form, tmpl.letter, value),
    TextColor::orange  // Idea: color should depend on the group element order
  );
}


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
  Snowpal(LiraExpr expr, int num_variables)
    : splitting_tree_(num_variables), orig_expr_(std::move(expr)) {
    expr_ = orig_expr_;
    CHECK_LT(num_variables, kMetaVarStart);
  }

  const LiraExpr& expr() const { return expr_; };
  const SplittingTree& splitting_tree() const { return splitting_tree_; }

  Snowpal& add_ball(std::vector<int> points);

private:
  SplittingTree splitting_tree_;
  LiraExpr orig_expr_;
  LiraExpr expr_;
};

std::ostream& to_ostream(std::ostream& os, const LiraExpr& expr, const SplittingTree& splitting_tree);
std::ostream& operator<<(std::ostream& os, const Snowpal& snowpal);
