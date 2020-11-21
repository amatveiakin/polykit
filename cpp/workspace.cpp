#include <iostream>
#include <regex>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/summation.h"
#include "lib/theta.h"




constexpr auto kMetaVariablesColors = std::array{
  TextColor::bright_blue,
  TextColor::bright_green,
  TextColor::bright_magenta,
  TextColor::bright_cyan,
};

constexpr int kMetaVarStart = 100;
constexpr int kMaxNeighbours = 100;

struct MetaVar {
  int node_index;
  int nbr_index;
};

int make_metavar(int node_idx, int nbr_idx) {
  return kMetaVarStart + node_idx * kMaxNeighbours + nbr_idx;
}

bool is_metavar(int var) {
  return var >= kMetaVarStart;
}

MetaVar parse_metavar(int var) {
  CHECK(is_metavar(var));
  int meta_idx = var - kMetaVarStart;
  return MetaVar{meta_idx / kMaxNeighbours, meta_idx % kMaxNeighbours};
}

std::string metavar_to_string_by_name(int var) {
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
inline std::string ratio_to_string(
    const CompoundRatio& ratio, const MetaVarPrinter& metavar_to_string) {
  return ratio.is_unity()
    ? fmt::unity()
    : str_join(
        ratio.loops(),
        "",
        [&](const std::vector<int>& loop) {
          return fmt::brackets(str_join(loop, ",", [&](int x) {
            return var_to_string(x, metavar_to_string);
          }));
        }
      );
}



class RatioOrUnity {
public:
  RatioOrUnity() {}
  RatioOrUnity(CrossRatio r) : data_(std::move(r)) {}

  static RatioOrUnity unity() { return RatioOrUnity(); }

  bool is_unity() const { return !data_.has_value(); }
  const CrossRatio& as_ratio() const { return data_.value(); }

  bool operator==(const RatioOrUnity& other) const { return data_ == other.data_; }
  bool operator!=(const RatioOrUnity& other) const { return data_ != other.data_; }
  bool operator< (const RatioOrUnity& other) const { return data_ <  other.data_; }
  bool operator<=(const RatioOrUnity& other) const { return data_ <= other.data_; }
  bool operator> (const RatioOrUnity& other) const { return data_ >  other.data_; }
  bool operator>=(const RatioOrUnity& other) const { return data_ >= other.data_; }

  template <typename H>
  friend H AbslHashValue(H h, const RatioOrUnity& r) {
    return H::combine(std::move(h), r.data_);
  }

private:
  std::optional<CrossRatio> data_;
};

CrossRatio to_cross_ratio(const CompoundRatio& compound_ratio) {
  CHECK_EQ(compound_ratio.loops().size(), 1) << "Only cross ratios are supported";
  return CrossRatio(to_array<4>(compound_ratio.loops().front()));
}

RatioOrUnity to_cross_ratio_or_unity(const CompoundRatio& compound_ratio) {
  return compound_ratio.is_unity()
    ? RatioOrUnity::unity()
    : to_cross_ratio(compound_ratio);
}

CompoundRatio to_compound_ratio(const RatioOrUnity& r) {
  return r.is_unity()
    ? CompoundRatio::unity()
    : CompoundRatio::from_cross_ratio(r.as_ratio());
}

std::string to_string(const RatioOrUnity& r) {
  return to_string(to_compound_ratio(r));
}

class IllegalTreeCutException : public std::exception {
public:
  IllegalTreeCutException(const std::string& what) : std::exception(what.c_str()) {}
};

struct ShortFormRatio {
  std::string letter;
  int node_index;
  CrossRatio normal_form;
  absl::flat_hash_map<CrossRatio, int> usage_stats;
};

class ShortFormRatioStorage {
public:
  ShortFormRatioStorage(const std::vector<ShortFormRatio>& ratios) {
    for (const ShortFormRatio& r : ratios) {
      ratios_[sorted(r.normal_form.indices())] = r;
    }
  }

  std::optional<ShortFormRatio> get_short_form_ratio(CrossRatio ratio) const {
    const auto it = ratios_.find(sorted(ratio.indices()));
    if (it != ratios_.end()) {
      return it->second;
    }
    return absl::nullopt;
  }

  void record_usage_stats(CrossRatio usage) {
    const auto it = ratios_.find(sorted(usage.indices()));
    if (it != ratios_.end()) {
      it->second.usage_stats[usage]++;
    }
  }

  void update_normal_forms() {
    for (auto& [key, ratio] : ratios_) {
      if (!ratio.usage_stats.empty()) {
        int max_usage = 0;
        CrossRatio max_usage_form;
        for (const auto& [form, usage_count] : ratio.usage_stats) {
          CHECK_GT(usage_count, 0);
          if (usage_count > max_usage) {
            max_usage = usage_count;
            max_usage_form = form;
          }
        }
        const CrossRatio new_normal_form = max_usage_form;
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

    // TODO: Make this a tree function; always return Nodes by const pointer.
    void split(const std::vector<int>& subpoints, SplittingTree* tree) {
      CHECK(!subpoints.empty());
      CHECK(absl::c_is_sorted(subpoints));
      CHECK(absl::c_includes(points, subpoints)) << list_to_string(points)
        << " does not contain " << list_to_string(subpoints);
      children.push_back(absl::make_unique<Node>(this, subpoints, tree->new_node_index()));
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

    //  Should be the same, but faster.
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
              list_to_string(child->points),
              " includes some but not all of ",
              list_to_string(subpoints)
            ));
          }
          return child.get();
        }
      }
      return nullptr;
    }

    const Node* lowest_ancestor(const std::vector<int>& points_arg) const {
      const Node* node = this;
      CHECK(set_intersection_size(node->points, points_arg) > 0);
      while (!absl::c_includes(node->points, points_arg)) {
        CHECK(node->parent != nullptr);
        node = node->parent;
      }
      return node;
    }
  };

  SplittingTree(int num_variables)
    : root_(absl::make_unique<Node>(
        nullptr, seq_incl(1, num_variables), new_node_index())) {}

  Node* root() { return root_.get(); }

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
            fmt::sub_num(letter, missing),
            node->node_index,
            CrossRatio(mapped_array(to_array<4>(points), [&](int nbr_idx) {
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
    return sorted(dump_nbr_indices_impl(root_.get()));
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

  std::vector<std::pair<int, std::vector<std::vector<int>>>> dump_nbr_indices_impl(Node* node) const {
    std::vector<std::pair<int, std::vector<std::vector<int>>>> ret;
    ret.push_back({node->node_index, node->nbr_indices()});
    for (const auto& child : node->children) {
      append_vector(ret, dump_nbr_indices_impl(child.get()));
    }
    return ret;
  }

  int next_node_index_ = 0;
  std::unique_ptr<Node> root_;
};

// Optimization potential: log(N) algorithm
const SplittingTree::Node* common_ancestor(
    const SplittingTree::Node* u, const SplittingTree::Node* v) {
  CHECK(u != nullptr);
  CHECK(v != nullptr);
  if (u == v) {
    return u;  // shortcut for speed
  }
  absl::flat_hash_set<const SplittingTree::Node*> u_ancestors;
  while (u != nullptr) {
    u_ancestors.insert(u);
    u = u->parent;
  }
  while (v != nullptr) {
    if (u_ancestors.contains(v)) {
      return v;
    }
    v = v->parent;
  }
  FATAL("No common ancestor");
}

bool is_grandparent(const SplittingTree::Node* parent, const SplittingTree::Node* child) {
  while (child != nullptr) {
    if (child == parent) {
      return true;
    }
    child = child->parent;
  }
  return false;
}


std::string short_form_to_string_impl(const ShortFormRatio& tmpl, CrossRatio value) {
  const std::string letter = tmpl.letter;
  auto is_permutation = [&](std::array<int, 4> permutation) {
    return permute(tmpl.normal_form.indices(), permutation) == value.indices();
  };
  if (is_permutation({0,1,2,3})) { return absl::Substitute("$0", letter); }
  if (is_permutation({0,2,1,3})) { return absl::Substitute("1 - $0", letter); }
  if (is_permutation({0,3,2,1})) { return absl::Substitute("1/$0", letter); }
  if (is_permutation({0,2,3,1})) { return absl::Substitute("1 - 1/$0", letter); }
  if (is_permutation({0,1,3,2})) { return absl::Substitute("$0/($0-1)", letter); }
  if (is_permutation({0,3,1,2})) { return absl::Substitute("1/(1-$0)", letter); }
  FATAL(absl::StrCat("Unknown permutation ", to_string(value), " of ", to_string(tmpl.normal_form)));
}

std::string short_form_to_string(const ShortFormRatio& tmpl, CrossRatio value) {
  return fmt::colored(short_form_to_string_impl(tmpl, value), TextColor::bright_yellow);
}


// Represents a particular case of LiraParam, namely
//   n_Li_1_1..._1(x_1, ..., x_n),  where each x_i each a single cross-ratio
class LiraParamOnes {
public:
  LiraParamOnes() {}
  explicit LiraParamOnes(std::vector<RatioOrUnity> ratios) : ratios_(std::move(ratios)) {
    CHECK_GT(ratios_.size(), 0);
  }

  int foreweight() const { return ratios_.size(); }
  std::vector<int> weights() const { return std::vector<int>(ratios_.size(), 1); }
  const std::vector<RatioOrUnity>& ratios() const { return ratios_; }

  int depth() const { return ratios().size(); }
  int total_weight() const { return ratios_.size() * 2 - 1; }
  int sign() const { return neg_one_pow(depth()); }

  bool operator==(const LiraParamOnes& other) const { return ratios_ == other.ratios_; }
  bool operator< (const LiraParamOnes& other) const { return ratios_ <  other.ratios_; }

  template <typename H>
  friend H AbslHashValue(H h, const LiraParamOnes& param) {
    return H::combine(std::move(h), param.ratios_);
  }

private:
  std::vector<RatioOrUnity> ratios_;
};

LiraParam to_lira_param(const LiraParamOnes& param) {
  return LiraParam(
    param.foreweight(),
    param.weights(),
    mapped(param.ratios(), to_compound_ratio)
  );
}



struct LiraExprParam : SimpleLinearParam<LiraParamOnes> {
  static std::string object_to_string(const LiraParamOnes& param) {
    return object_to_string(param, nullptr);
  }
  static std::string object_to_string(
      const LiraParamOnes& param, const ShortFormRatioStorage* short_forms) {
    return fmt::function(
      lira_param_function_name(to_lira_param(param)),
      mapped(param.ratios(), [&](const RatioOrUnity& ratio) {
        if (ratio.is_unity()) {
          return fmt::unity();
          // return to_string(ratio);
          // return "  " + to_string(ratio) + "  ";
        }
        if (short_forms) {
          const auto& points = ratio.as_ratio();
          auto short_form = short_forms->get_short_form_ratio(points);
          return short_form.has_value()
            ? short_form_to_string(*short_form, points)
            : ratio_to_string(to_compound_ratio(ratio), metavar_to_string_by_name);
          // auto full = ratio_to_string(to_compound_ratio(ratio), metavar_to_string_by_name);
          // return short_form.has_value()
          //   ? short_form_to_string(*short_form, points) + " " + full
          //   : full;
        } else {
          return ratio_to_string(to_compound_ratio(ratio), metavar_to_string_by_name);
        }
      }),
      HSpacing::sparse
    );
  }
};

using LiraExpr = Linear<LiraExprParam>;


int num_distinct_ratio_variables(const std::vector<RatioOrUnity>& ratios) {
  std::vector<std::array<int, 4>> ratios_sorted;
  for (const auto& r : ratios) {
    if (!r.is_unity()) {
      ratios_sorted.push_back(sorted(r.as_ratio().indices()));
    }
  };
  return num_distinct_elements(ratios);
}

int num_ratio_points(const std::vector<CrossRatio>& ratios) {
  absl::flat_hash_set<int> all_points;
  for (const auto& r : ratios) {
    all_points.insert(r.indices().begin(), r.indices().end());
  }
  return all_points.size();
}

// TODO: Check if this is a valid criterion
// Optimiation potential: try to avoid exponential explosion
bool are_ratios_independent(const std::vector<CrossRatio>& ratios) {
  if (num_ratio_points(ratios) < ratios.size() + 3) {
    return false;
  }
  if (ratios.size() > 1) {
    for (int i = 0; i < ratios.size(); ++i) {
      if (!are_ratios_independent(removed_index(ratios, i))) {
        return false;
      }
    }
  }
  return true;
}


LiraExpr to_lyndon_basis_2(const LiraExpr& expr) {
  LiraExpr ret;
  expr.foreach([&](const LiraParamOnes& formal_symbol, int coeff) {
    const auto& ratios = formal_symbol.ratios();
    CHECK_EQ(ratios.size(), 2);
    if (ratios[0] == ratios[1]) {
      // skip
    } else if (ratios[0] < ratios[1]) {
      // already ok
      ret.add_to(formal_symbol, coeff);
    } else {
      // swap:  ba -> ab
      ret.add_to(LiraParamOnes({ratios[1], ratios[0]}), -coeff);
    }
  });
  return ret;
}

LiraExpr to_lyndon_basis_3_soft(const LiraExpr& expr) {
  LiraExpr ret;
  // There are two equations in case of three ratios:
  //   * xyz == zyx
  //   * xyz + yxz + yzx == 0
  // First, apply the first one:
  LiraExpr expr_symm = expr.mapped([](const LiraParamOnes& formal_symbol) {
    auto ratios = formal_symbol.ratios();
    CHECK_EQ(ratios.size(), 3);
    sort_two(ratios[0], ratios[2]);
    return LiraParamOnes(ratios);
  });
  expr_symm.foreach([&](const LiraParamOnes& formal_symbol, int coeff) {
    const auto& ratios = formal_symbol.ratios();
    CHECK_EQ(ratios.size(), 3);
    const int distinct = num_distinct_elements(ratios);
    LiraExpr replacement;
    if (distinct == 3) {
      //  abc  acb  bac  bca  cab  cba  -- original expr
      //  abc  acb  bac  acb  bac  abc  -- expr_symm
      //  ^^^^^^^^  ^^^  ^^^^^^^^^^^^^
      //  already    |   duplicates
      //  Lyndon     |
      //             needs shuffle: (b)(ac) = bac + abc + acb

      if (ratios[1] >= ratios[0]) {
        ret.add_to(LiraParamOnes(ratios), coeff);
      } else {
        auto ratios1 = choose_indices(ratios, {1,0,2});
        auto ratios2 = choose_indices(ratios, {1,2,0});
        replacement.add_to(LiraParamOnes(ratios1), -coeff);
        replacement.add_to(LiraParamOnes(ratios2), -coeff);
      }
    } else if (distinct == 2) {
      //  aab  abb  aba  bab  baa  bba  -- original expr
      //  aab  abb  aba  bab  aab  abb  -- expr_symm
      //  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^
      //  already   |         duplicates
      //  Lyndon    |
      //            needs shuffle: (ab)(a) = aba + 2*aab
      //                           (b)(ab) = bab + 2*abb

      if (ratios[0] != ratios[2]) {
        ret.add_to(LiraParamOnes(ratios), coeff);
      } else if (ratios[1] >= ratios[0]) {
        auto ratios1 = choose_indices(ratios, {0,2,1});
        replacement.add_to(LiraParamOnes(ratios1), -2 * coeff);
      } else {
        auto ratios1 = choose_indices(ratios, {1,0,2});
        replacement.add_to(LiraParamOnes(ratios1), -2 * coeff);
      }
    } else if (distinct == 1) {
      // skip: zero
    } else {
      FATAL(absl::StrCat("Bad number of distinct elements: ", distinct));
    }
    if (!replacement.zero()) {
      // Note: for a proper Lyndon basis, use replacement unconditionally.
      if ((expr_symm + replacement).l1_norm() <= expr_symm.l1_norm()) {
        ret += replacement;
      } else {
        ret.add_to(LiraParamOnes(ratios), coeff);
      }
    }
  });
  return ret;
}

LiraExpr without_unities(const LiraExpr& expr) {
  return expr.filtered([](const LiraParamOnes& formal_symbol) {
    return absl::c_none_of(formal_symbol.ratios(), [](const RatioOrUnity& r) {
      return r.is_unity();
    });
  });
}

LiraExpr keep_distinct_ratios(const LiraExpr& expr) {
  return expr.filtered([](const LiraParamOnes& formal_symbol) {
    return num_distinct_ratio_variables(formal_symbol.ratios()) ==
        formal_symbol.ratios().size();
  });
}

LiraExpr keep_independent_ratios(const LiraExpr& expr) {
  return expr.filtered([](const LiraParamOnes& formal_symbol) {
    std::vector<CrossRatio> ratios;
    for (const auto& r : formal_symbol.ratios()) {
      if (r.is_unity()) {
        return false;
      }
      ratios.push_back(r.as_ratio());
    }
    return are_ratios_independent(ratios);
  });
}

// Applies rule:
//   {x_1, ..., x_n} = (-1)^n * {1/x_1, ..., 1/x_n}
LiraExpr normalize_inverse(const LiraExpr& expr) {
  LiraExpr ret;
  expr.foreach([&](const LiraParamOnes& formal_symbol, int coeff) {
    static auto is_normal = [](const RatioOrUnity& r) {
      return r.is_unity() || r.as_ratio()[1] <= r.as_ratio()[3];
    };
    auto ratios = formal_symbol.ratios();
    // Idea: choose a marker independently from position, so that the choice
    //   wouldn't be affected by Lyndon.
    // Note: if ratios.size() is odd, this would work as well:
    //   const int normal_ratios = absl::c_count_if(ratios, is_normal);
    //   if (normal_ratios <= ratios.size() / 2) { ... }
    const auto marker_it = absl::c_max_element(
      ratios,
      [](const RatioOrUnity& r1, const RatioOrUnity& r2) {
        if (r2.is_unity()) {
          return false;
        }
        if (r1.is_unity()) {
          return true;
        }
        return sorted(r1.as_ratio().indices()) < sorted(r2.as_ratio().indices());
      }
    );
    CHECK(marker_it != ratios.end());
    if (is_normal(*marker_it)) {
      for (auto& r : ratios) {
        if (!r.is_unity()) {
          r = CrossRatio::inverse(r.as_ratio());
        }
      }
      ret.add_to(LiraParamOnes(ratios), neg_one_pow(ratios.size()) * coeff);
    } else {
      ret.add_to(formal_symbol, coeff);
    }
  });
  return ret;
}


using Substitution = std::array<const SplittingTree::Node*, 2>;

struct ZeroOrInf {};

using RatioSubstitutionResult = std::variant<ZeroOrInf, RatioOrUnity>;


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
    const SplittingTree::Node* node) {
  CHECK_EQ(points.size(), 4);
  CHECK_GE(set_intersection_size(points, node->points), 3);
  for (const auto& child : node->children) {
    const auto points_in_child = set_intersection(points, child->points);
    if (points_in_child.size() >= 3) {
      return find_central_node(points, child.get());
    } else if (points_in_child.size() == 2) {
      return to_array<2>(points_in_child);
    }
  }
  // Neither child has more than one point => this is the central point.
  return node;
}

RatioSubstitutionResult ratio_substitute(
    const RatioOrUnity& ratio,
    SplittingTree* tree) {
  if (ratio.is_unity()) {
    return ratio;
  }
  const auto& old_points = ratio.as_ratio().indices();
  // TODO: Avoid converting to vector
  const auto central_node_or_bad_pair =
    find_central_node(to_vector(sorted(old_points)), tree->root());
  return std::visit(overloaded{
    [&](const SplittingTree::Node* central_node) -> RatioSubstitutionResult {
      CHECK_GE(central_node->valency(), 4);
      auto new_points = mapped_array(old_points, [&](int p) {
        return central_node->metavar_for_point(p);
      });
      return RatioOrUnity(CrossRatio(new_points));
    },
    [&](std::array<int, 2> bad_pair) -> RatioSubstitutionResult {
      const int dist = std::abs(
        element_index(old_points, bad_pair[0]) -
        element_index(old_points, bad_pair[1])
      );
      CHECK(1 <= dist && dist <= 3) << dist;
      if (dist == 2) {
        return RatioOrUnity::unity();
      } else {
        return ZeroOrInf{};
      }
    },
  }, central_node_or_bad_pair);
}



LiraExpr theta_expr_to_lira_expr_without_products(const ThetaExpr& expr) {
  LiraExpr ret;
  expr.foreach([&](const ThetaPack& term, int coeff) {
    std::visit(overloaded{
      [&](const std::vector<Theta>& term_product) {
        FATAL("Unexpected std::vector<Theta> when converting to LiraExpr");
      },
      [&](const LiraParam& formal_symbol) {
        if (absl::c_all_of(formal_symbol.weights(), [](int w) { return w == 1; })) {
          CHECK_EQ(formal_symbol.foreweight(), formal_symbol.weights().size());
          LiraParamOnes new_formal_symbol(mapped(formal_symbol.ratios(), to_cross_ratio_or_unity));
          ret.add_to(new_formal_symbol, coeff);
        } else {
          // ignore all products
        }
      },
    }, term);
  });
  return ret;
}

LiraExpr lira_expr_substitute(
    const LiraExpr& expr,
    SplittingTree* tree) {
  LiraExpr ret;
  expr.foreach([&](const LiraParamOnes& formal_symbol, int coeff) {
    std::vector<RatioOrUnity> new_ratios;
    for (const RatioOrUnity& ratio: formal_symbol.ratios()) {
      auto ratio_subst = ratio_substitute(ratio, tree);
      if (std::holds_alternative<RatioOrUnity>(ratio_subst)) {
        new_ratios.push_back(std::get<RatioOrUnity>(ratio_subst));
      } else {
        return;
      }
    }
    ret += coeff * LiraExpr::single(LiraParamOnes(std::move(new_ratios)));
  });
  return ret;
}

template<typename Container>
std::vector<Container> all_permutations(Container c) {
  std::vector<int> indices(c.size());
  absl::c_iota(indices, 0);
  std::vector<Container> ret;
  do {
    ret.push_back(choose_indices(c, indices));
  } while (absl::c_next_permutation(indices));
  return ret;
}

template<typename Container>
bool all_equal(Container c) {
  return absl::c_adjacent_find(c, std::not_equal_to<>()) == c.end();
}

template<typename T>
bool all_equal(std::initializer_list<T> c) {
  return absl::c_adjacent_find(c, std::not_equal_to<>()) == c.end();
}

// Alternative to Lyndon basis.
/*
std::pair<LiraExpr, LiraExpr> lira_expr_cancel_shuffle(const LiraExpr& expr) {
  using Key = std::array<RatioOrUnity, 3>;
  struct Value {
    LiraParam term;
    int coeff;
  };
  static auto lira_param_to_key = [](const LiraParam& term) {
    return to_array<3>(mapped(term.ratios(), to_cross_ratio_or_unity));
  };
  absl::flat_hash_map<Key, std::vector<Value>> sorted_ratios;
  expr.foreach([&](const LiraParam& term, int coeff) {
    const Key key = sorted(lira_param_to_key(term));
    sorted_ratios[key].push_back({term, coeff});
  });

  LiraExpr no_shuffle_expr;
  LiraExpr shuffle_expr;
  for (const auto& [key, value] : sorted_ratios) {
    // Consider: use Lyndon basis
    // CHECK_LE(value.size(), 3);  // this is probably possible, but not supported by shuffle finder yet
    if (value.size() > 3) {
      std::cout << "WARNING: some shuffle relations may have been missed\n";
    }
    bool is_shuffle = false;
    if (value.size() == 3) {
      if (num_distinct_elements(mapped(value, [](const Value& v) { return v.coeff; })) == 1) {
        auto keys = mapped(value, [](const Value& v) {
          return lira_param_to_key(v.term);
        });
        for (const auto& k : all_permutations(keys)) {
          // Check  0 == ABC + BAC + BCA == A ⧢ BC
          if (all_equal({k[0][0], k[1][1], k[2][2]}) &&
              all_equal({k[0][1], k[1][0], k[2][0]}) &&
              all_equal({k[0][2], k[1][2], k[2][1]})) {
            is_shuffle = true;
            break;
          }
        }
      }
    }
    if (value.size() == 2) {
      auto keys = to_array<2>(mapped(value, [](const Value& v) {
        return lira_param_to_key(v.term);
      }));
      // Check  0 == ABC - CBA
      if (keys == reversed(keys) && value[0].coeff == -value[1].coeff) {
        is_shuffle = true;
      }
    }
    for (const auto& [term, coeff] : value) {
      if (is_shuffle) {
        shuffle_expr.add_to(term, coeff);
      } else {
        no_shuffle_expr.add_to(term, coeff);
      }
    }
  }
  return {no_shuffle_expr, shuffle_expr};
}
*/


class Snowpal {
public:
  Snowpal(LiraExpr expr, int num_variables)
    : splitting_tree_(num_variables), orig_expr_(std::move(expr)) {
    expr_ = orig_expr_;
    CHECK_LT(num_variables, kMetaVarStart);
  }

  const LiraExpr& expr() const { return expr_; };
  const SplittingTree& splitting_tree() const { return splitting_tree_; }

  Snowpal& add_ball(std::vector<int> points) {
    CHECK(!points.empty());
    absl::c_sort(points);
    CHECK_EQ(points.size(), num_distinct_elements(points)) << list_to_string(points);
    auto* node = splitting_tree_.node_for_points(points);
    node->split(points, &splitting_tree_);
    expr_ = lira_expr_substitute(orig_expr_, &splitting_tree_);
    expr_ = without_unities(expr_);
    // expr_ = keep_distinct_ratios(expr_);
    // expr_ = keep_independent_ratios(expr_);
    expr_ = normalize_inverse(expr_);
    expr_ = to_lyndon_basis_2(expr_);
    // expr_ = to_lyndon_basis_3_soft(expr_);
    return *this;
  }

private:
  SplittingTree splitting_tree_;
  LiraExpr orig_expr_;
  LiraExpr expr_;
};

std::ostream& to_ostream(std::ostream& os, const LiraExpr& expr, const SplittingTree& splitting_tree) {
  ShortFormRatioStorage short_forms{splitting_tree.generate_short_form_ratios()};
  expr.foreach([&](const LiraParamOnes& param, int) {
    for (const auto& r : param.ratios()) {
      if (!r.is_unity()) {
        short_forms.record_usage_stats(r.as_ratio());
      }
    }
  });
  short_forms.update_normal_forms();
  to_ostream(os, expr, &short_forms);
  const auto nbr_indices_per_vertex = splitting_tree.dump_nbr_indices();
  if (nbr_indices_per_vertex.size() >= 2) {
    os << "^^^\n";
    for (const auto& [node_index, nbr_indices] : nbr_indices_per_vertex) {
      std::vector<std::string> index_strs;
      for (int i = 0; i < nbr_indices.size(); ++i) {
        const std::vector<int>& indices = nbr_indices[i];
        index_strs.push_back(absl::StrCat(
          metavar_to_string_by_name(make_metavar(node_index, i+1)),
          "=",
          fmt::braces(str_join(indices, ","))
        ));
      }
      os << " " << str_join(index_strs, " ") << "\n";
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Snowpal& snowpal) {
  return to_ostream(os, snowpal.expr(), snowpal.splitting_tree());
}



int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  Profiler profiler;

  ScopedFormatting sf(FormattingConfig()
    .set_formatter(Formatter::unicode)
    // .set_rich_text_format(RichTextFormat::plain_text)
    .set_rich_text_format(RichTextFormat::console)
    .set_expression_line_limit(FormattingConfig::kNoLineLimit)
  );

  const int num_points = 7;
  const int num_args = num_points / 2 - 1;
  auto source = sum_looped_vec(
    [&](const std::vector<X>& args) {
      return LiQuad(
        num_points / 2 - 1,
        mapped(args, [](X x) { return x.var(); })
      );
    },
    num_points,
    seq_incl(1, num_points - 1)
  );

  auto expr = theta_expr_to_lira_expr_without_products(source.without_annotations());



  StringExpr stats;

  auto analyze_snowpal = [&](const Snowpal& snowpal) {
    absl::flat_hash_set<std::array<int, 4>> ratios;
    snowpal.expr().foreach([&](const LiraParamOnes& formal_symbol, int) {
      for (const RatioOrUnity& r : formal_symbol.ratios()) {
        if (!r.is_unity()) {
          ratios.insert(sorted(r.as_ratio().indices()));
        }
      }
    });
    stats.add_to(absl::StrCat("vars ", absl::Dec(ratios.size(), absl::kZeroPad2)), 1);
    if (ratios.size() == num_args) {
    // if (num_args <= ratios.size() && ratios.size() <= num_args + 2) {
      std::cout << snowpal << "\n";
    }
  };



  // The expression is symmetric w.r.t to rotations, so let's fix one arg.
  // This still doesn't cover all symmetries.

  // const int a = 1;
  // for (int b = a+1; b <= num_points; ++b) {
  //   for (int c = b+1; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       Snowpal snowpal(expr, num_points);
  //       snowpal.add_ball({a, b, c, d});
  //       analyze_snowpal(snowpal);
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = a+1; b <= num_points; ++b) {
  //   for (int c = b+1; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           try {
  //             Snowpal snowpal(expr, num_points);
  //             snowpal.add_ball({a, b, c, d}).add_ball({e, f});
  //             analyze_snowpal(snowpal);
  //           } catch (const IllegalTreeCutException&) {}
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = a+1; b <= num_points; ++b) {
  //   for (int c = b+1; c <= num_points; ++c) {
  //     for (int d = 2; d <= num_points; ++d) {
  //       for (int e = d+1; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           try {
  //             Snowpal snowpal(expr, num_points);
  //             snowpal.add_ball({a, b, c}).add_ball({d, e, f});
  //             analyze_snowpal(snowpal);
  //           } catch (const IllegalTreeCutException&) {}
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // const int b = 4;
  // for (int b = 2; b <= num_points; ++b) {
  //   for (int c = 2; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           for (int g = f+1; g <= num_points; ++g) {
  //             try {
  //               Snowpal snowpal(expr, num_points);
  //               snowpal.add_ball({a, b}).add_ball({c, d}).add_ball({e, f, g});
  //               analyze_snowpal(snowpal);
  //             } catch (const IllegalTreeCutException&) {}
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = 2; b <= num_points; ++b) {
  //   for (int c = 2; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           try {
  //             Snowpal snowpal(expr, num_points);
  //             snowpal.add_ball({a, b}).add_ball({c, d}).add_ball({e, f});
  //             analyze_snowpal(snowpal);
  //           } catch (const IllegalTreeCutException&) {}
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = 3; b <= num_points; ++b) {
  //   for (int c = 2; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           for (int g = 2; g <= num_points; ++g) {
  //             for (int h = g+1; h <= num_points; ++h) {
  //               try {
  //                 Snowpal snowpal(expr, num_points);
  //                 snowpal.add_ball({a, b}).add_ball({c, d}).add_ball({e, f}).add_ball({g, h});
  //                 analyze_snowpal(snowpal);
  //               } catch (const IllegalTreeCutException&) {}
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

  for (int num_groups = 1; num_groups <= num_points / 2; ++num_groups) {  // group = 2 or more elements
    for (const auto& seq : all_sequences(num_groups + 1, num_points - 1)) {
      // Group 0 means "no group". Point 1 is always in group 1. Because symmetry.
      std::vector<std::vector<int>> groups(num_groups);
      groups[1 - 1].push_back(1);
      for (int i = 0; i < seq.size(); ++i) {
        const int point = i + 2;  // numeration starts from 1; skip point 1 (see above)
        const int group = seq[i];
        if (group > 0) {
          groups.at(group - 1).push_back(point);
        }
      }
      if (absl::c_any_of(groups, [](const auto& gr) { return gr.size() < 2; })) {
        continue;
      }
      try {
        Snowpal snowpal(expr, num_points);
        for (const auto& gr : groups) {
          snowpal.add_ball(gr);
        }
        analyze_snowpal(snowpal);
      } catch (const IllegalTreeCutException&) {}
    }
  }

  std::cout << stats;



/*
  constexpr char kInvalidInput[] = "Invalid input: ";
  std::cout << "Functional\n" << source.annotations() << "\n";
  while (true) {
    std::vector<std::vector<int>> balls;
    std::unique_ptr<Snowpal> snowpal;
    auto reset_snowpal = [&]() {
      snowpal = absl::make_unique<Snowpal>(expr, num_points);
      for (const auto& b : balls) {
        snowpal->add_ball(b);
      }
    };
    auto add_ball = [&](const std::vector<int>& ball) {
      snowpal->add_ball(ball);
      balls.push_back(ball);
    };
    reset_snowpal();
    std::cout << "Original " << expr << "\n";
    while (true) {
      std::cout << "> ";
      std::string input;
      std::getline(std::cin, input);
      trim(input);
      if (input.empty()) {
        continue;
      } else if (input == "q" || input == "quit") {
        return 0;
      } else if (input == "r" || input == "reset") {
        break;
      } else if (input == "b" || input == "back") {
        if (balls.empty()) {
          std::cout << kInvalidInput << "nothing to remove\n";
          continue;
        }
        balls.pop_back();
        reset_snowpal();
        std::cout << "Reverted " << *snowpal << "\n";
        continue;
      }
      std::vector<int> ball;
      try {
        // for (const auto& var_str : absl::StrSplit(input, " ", absl::SkipEmpty())) {
        //   const int var = std::stoi(std::string(var_str));
        //   if (var < 1 || var > num_points) {
        //     throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
        //   }
        //   ball.push_back(var);
        // }
        for (const char ch : input) {
          if (std::isspace(ch)) {
            continue;
          }
          const int var = std::stoi(std::string(1, ch));
          if (var < 1 || var > num_points) {
            throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
          }
          ball.push_back(var);
        }
      } catch (const std::exception& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        continue;
      }
      try {
        snowpal->add_ball(ball);
        balls.push_back(ball);
        std::cout << "Substituted " << *snowpal << "\n";
      } catch (const IllegalTreeCutException& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        reset_snowpal();
      }
    }
  }
*/


  // Snowpal snowpal(expr, 7);
  // snowpal.add_ball({1,2,3}).add_ball({1,3});  // => {1,x} + {x,1} == 0  // superseeded by the one below
  // snowpal.add_ball({1,3,7});  // => {x,y} == {1/x,1/y}
  // snowpal.add_ball({1,2,3}).add_ball({6,7});  // => {x,y} + {y,x} == 0
  // snowpal.add_ball({1,3,5}).add_ball({2,4});  // => {1,x} + {1,x/(x-1)} == 0
  // snowpal.add_ball({1,3,5}).add_ball({2,7});  // => {1,x} == 0
  // snowpal.add_ball({1,3,5});  // could be explored more
  // snowpal.add_ball({1,3}).add_ball({2,7});  // => {x,y} + {1/(1-y),1-1/x)} == 0
  // snowpal.add_ball({1,3}).add_ball({2,5});  // => {x,y} + {x/(x-1),y} == 0   ???


  // {1,3,6}{2,5,8}{4,7}  -- one var, first arg changes
  // {1,2,5}{3,6,9}
  // {1,3,5}{2,6,7}       -- two vars
  // {1,3,6}{2,4,9}{5,7}  -- close to (<1>, <1>, var)
  // {1,3,4,8}            -- {x, y, z} + {1/x, 1/y, 1/z} == 0
  // {1,2,4,6}            -- {x, y, z} + {1/z, 1/y, 1/x} == 0  (note: this is the one above + shuffle)
  // {1,2,4,7}{3,6}       -- two vars

  // Below are equation modulo <1>
  // {1,3}{5,8}{2,4,9}    -- {x, y, z} + {1-x, y/(y-1), 1-z} == 0

  // BAD: uses dependent variables
  // {1,4,6,8}{3,9}       -- {x, y, z} + {x, y, 1-z} == {with <= 2 args}
  // {1,3,5,7}{2,8}       -- {x, y, z} + {x/(x-1), y, z} == {with <= 2 args}
}
