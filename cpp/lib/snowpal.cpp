#include "snowpal.h"

#include "absl/container/flat_hash_set.h"

#include "polylog_qli.h"


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

static int make_metavar(int node_idx, int nbr_idx) {
  return kMetaVarStart + node_idx * kMaxNeighbours + nbr_idx;
}

static bool is_metavar(int var) {
  return var >= kMetaVarStart;
}

static MetaVar parse_metavar(int var) {
  CHECK(is_metavar(var));
  int meta_idx = var - kMetaVarStart;
  return MetaVar{meta_idx / kMaxNeighbours, meta_idx % kMaxNeighbours};
}

static std::string metavar_to_string_by_name(int var) {
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
static std::string var_to_string(int var, const MetaVarPrinter& metavar_to_string) {
  return is_metavar(var)
    ? metavar_to_string(var)
    : to_string(var);
}

template<typename MetaVarPrinter>
static std::string ratio_to_string(const CrossRatioNOrUnity& r, const MetaVarPrinter& metavar_to_string) {
  return r.is_unity()
    ? fmt::unity()
    : fmt::brackets(str_join(r.as_ratio().indices(), ",", [&](int x) {
        return var_to_string(x, metavar_to_string);
      }));
}

static std::string short_form_to_string(const ShortFormRatio& tmpl, CrossRatioN value) {
  return fmt::colored(
    dependent_cross_ratio_formula(tmpl.normal_form, tmpl.letter, value),
    TextColor::orange  // Idea: color should depend on the group element order
  );
}

static std::string snow_line_to_string(
    const LiraParamOnes& param, const ShortFormRatioStorage* short_forms) {
  return fmt::function(
    lira_param_function_name(param.foreweight(), param.weights()),
    mapped(param.ratios(), [&](const CrossRatioNOrUnity& ratio) {
      if (ratio.is_unity()) {
        return fmt::unity();
      }
      if (short_forms) {
        const auto& points = ratio.as_ratio();
        auto short_form = short_forms->get_short_form_ratio(points);
        return short_form.has_value()
          ? short_form_to_string(*short_form, points)
          : ratio_to_string(ratio, metavar_to_string_by_name);
      } else {
        return ratio_to_string(ratio, metavar_to_string_by_name);
      }
    }),
    HSpacing::sparse
  );
}

struct SnowLiraExprParam : LiraExprParam {
  static std::string object_to_string(const LiraParamOnes& param) {
    return snow_line_to_string(param, nullptr);
  }
};

using SnowLiraExpr = Linear<SnowLiraExprParam>;


ShortFormRatioStorage::ShortFormRatioStorage(const std::vector<ShortFormRatio>& ratios) {
  for (const ShortFormRatio& r : ratios) {
    ratios_[sorted(r.normal_form.indices())] = r;
  }
}

std::optional<ShortFormRatio> ShortFormRatioStorage::get_short_form_ratio(CrossRatioN ratio) const {
  const auto it = ratios_.find(sorted(ratio.indices()));
  if (it != ratios_.end()) {
    return it->second;
  }
  return absl::nullopt;
}

void ShortFormRatioStorage::record_usage_stats(CrossRatioN usage) {
  const auto it = ratios_.find(sorted(usage.indices()));
  if (it != ratios_.end()) {
    it->second.usage_stats[usage]++;
  }
}

void ShortFormRatioStorage::update_normal_forms() {
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


void SplittingTree::Node::split(const std::vector<int>& subpoints, SplittingTree& tree) {
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

int SplittingTree::Node::valency() const {
  return (parent != nullptr ? 1 : 0) + complement_to_children().size() + children.size();
}

std::vector<std::vector<int>> SplittingTree::Node::nbr_indices() const {
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

int SplittingTree::Node::nbr_idx_for_point(int point) const {
  const auto indices = nbr_indices();
  const auto it = absl::c_find_if(indices, [&](const auto& sub_indices) {
    return contains_naive(sub_indices, point);
  });
  CHECK(it != indices.end());
  return it - indices.begin() + 1;
}

// Optimization potential: Use this. Should be the same, but faster.
// int SplittingTree::Node::nbr_idx_for_point(int point) const {
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

int SplittingTree::Node::metavar_for_point(int point) const {
  return make_metavar(node_index, nbr_idx_for_point(point));
}

std::vector<int> SplittingTree::Node::complement_to_children() const {
  std::vector<int> complement = points;
  for (const auto& child : children) {
    std::vector<int> new_complement;
    absl::c_set_difference(complement, child->points, std::back_inserter(new_complement));
    complement = std::move(new_complement);
  }
  return complement;
}

SplittingTree::Node* SplittingTree::Node::descend(const std::vector<int>& subpoints) const {
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

SplittingTree::SplittingTree(int num_variables)
  : root_(absl::make_unique<Node>(
      nullptr, seq_incl(1, num_variables), new_node_index())) {}

SplittingTree::Node* SplittingTree::node_for_points(const std::vector<int>& points) {
  SplittingTree::Node* current = root();
  SplittingTree::Node* next = current;
  while (next != nullptr) {
    current = next;
    next = current->descend(points);
  }
  return current;
}

int SplittingTree::new_node_index() {
  return next_node_index_++;
}

ShortFormRatioStorage SplittingTree::generate_short_form_ratios() const {
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
          })),
          {},
        });
      }
    }
    ++ratio_index;
  });
  return {ret};
}

std::vector<std::pair<int, std::vector<std::vector<int>>>> SplittingTree::dump_nbr_indices() const {
  std::vector<std::pair<int, std::vector<std::vector<int>>>> ret;
  foreach_node([&](const Node* node) {
    ret.push_back({node->node_index, node->nbr_indices()});
  });
  return sorted(ret);
}


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
    const CrossRatioNOrUnity& ratio,
    const SplittingTree& tree) {
  if (ratio.is_unity()) {
    return ratio;
  }
  const auto& old_points = ratio.as_ratio().indices();
  const auto central_node_or_bad_pair =
    find_central_node(to_vector(sorted(old_points)), tree.root());
  return std::visit(overloaded{
    [&](const SplittingTree::Node* central_node) -> RatioSubstitutionResult {
      CHECK_GE(central_node->valency(), 4);
      auto new_points = mapped_array(old_points, [&](int p) {
        return central_node->metavar_for_point(p);
      });
      return CrossRatioNOrUnity(CrossRatioN(new_points));
    },
    [&](std::array<int, 2> bad_pair) -> RatioSubstitutionResult {
      const int dist = std::abs(
        element_index(old_points, bad_pair[0]) -
        element_index(old_points, bad_pair[1])
      );
      CHECK(1 <= dist && dist <= 3) << dist;
      if (dist == 2) {
        return CrossRatioNOrUnity::unity();
      } else {
        return ZeroOrInf{};
      }
    },
  }, central_node_or_bad_pair);
}


LiraExpr lira_expr_substitute(
    const LiraExpr& expr,
    const SplittingTree& tree) {
  return expr.mapped_expanding([&](const LiraParamOnes& formal_symbol) {
    std::vector<CrossRatioNOrUnity> new_ratios;
    for (const CrossRatioNOrUnity& ratio: formal_symbol.ratios()) {
      auto ratio_subst = ratio_substitute(ratio, tree);
      if (std::holds_alternative<CrossRatioNOrUnity>(ratio_subst)) {
        new_ratios.push_back(std::get<CrossRatioNOrUnity>(ratio_subst));
      } else {
        return LiraExpr{};
      }
    }
    return LiraExpr::single(LiraParamOnes(std::move(new_ratios)));
  });
}


Snowpal::Snowpal(LiraExpr expr, int num_variables)
  : splitting_tree_(num_variables), orig_expr_(std::move(expr)) {
  expr_ = orig_expr_;
  CHECK_LT(num_variables, kMetaVarStart);
}

Snowpal& Snowpal::add_ball(std::vector<int> points) {
  CHECK(!points.empty());
  absl::c_sort(points);
  CHECK_EQ(points.size(), num_distinct_elements_unsorted(points)) << dump_to_string(points);
  auto* node = splitting_tree_.node_for_points(points);
  node->split(points, splitting_tree_);
  expr_ = lira_expr_substitute(orig_expr_, splitting_tree_);
  // expr_ = without_unities(expr_);
  // expr_ = fully_normalize_ratios(expr_);
  // expr_ = keep_distinct_ratios(expr_);
  // expr_ = keep_independent_ratios(expr_);
  expr_ = normalize_inverse(expr_);
  expr_ = to_lyndon_basis(expr_);
  return *this;
}

std::ostream& to_ostream(
    std::ostream& os, const LiraExpr& expr, const SplittingTree& splitting_tree,
    bool short_form_ratios) {
  const auto& snow_expr = expr.cast_to<SnowLiraExpr>();
  ScopedFormatting sf(FormattingConfig().set_new_line_after_expression(false));
  if (short_form_ratios) {
    ShortFormRatioStorage short_forms{splitting_tree.generate_short_form_ratios()};
    expr.foreach([&](const LiraParamOnes& param, int) {
      for (const auto& r : param.ratios()) {
        if (!r.is_unity()) {
          short_forms.record_usage_stats(r.as_ratio());
        }
      }
    });
    short_forms.update_normal_forms();
    to_ostream(os, snow_expr, std::less<>{}, [&](const auto& term) {
      return snow_line_to_string(term, &short_forms);
    });
  } else {
    os << snow_expr;
  }
  const auto nbr_indices_per_vertex = splitting_tree.dump_nbr_indices();
  if (nbr_indices_per_vertex.size() >= 2) {
    os << "^^^\n";
    for (const auto& [node_index, nbr_indices] : nbr_indices_per_vertex) {
      std::vector<std::string> index_strs;
      for (int i : range(nbr_indices.size())) {
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

std::ostream& to_ostream(std::ostream& os, const Snowpal& snowpal, bool short_form_ratios) {
  return to_ostream(os, snowpal.expr(), snowpal.splitting_tree(), short_form_ratios);
}

std::ostream& operator<<(std::ostream& os, const Snowpal& snowpal) {
  return to_ostream(os, snowpal, true);
}
