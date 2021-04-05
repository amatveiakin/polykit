#include "snowpal.h"

#include "absl/container/flat_hash_set.h"

#include "polylog_qli.h"


struct SnowLiraExprParam : LiraExprParam {
  static std::string object_to_string(const LiraParamOnes& param) {
    return object_to_string(param, nullptr);
  }
  static std::string object_to_string(
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
};

using SnowLiraExpr = Linear<SnowLiraExprParam>;


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


Snowpal& Snowpal::add_ball(std::vector<int> points) {
  CHECK(!points.empty());
  absl::c_sort(points);
  CHECK_EQ(points.size(), num_distinct_elements_unsorted(points)) << dump_to_string(points);
  auto* node = splitting_tree_.node_for_points(points);
  node->split(points, splitting_tree_);
  expr_ = lira_expr_substitute(orig_expr_, splitting_tree_);
  expr_ = without_unities(expr_);
  expr_ = fully_normalize_ratios(expr_);
  expr_ = keep_distinct_ratios(expr_);
  // expr_ = keep_independent_ratios(expr_);
  // expr_ = normalize_inverse(expr_);
  expr_ = to_lyndon_basis(expr_);
  return *this;
}

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
  to_ostream(os, expr.cast_to<SnowLiraExpr>(), std::less<>{}, &short_forms);
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

std::ostream& operator<<(std::ostream& os, const Snowpal& snowpal) {
  return to_ostream(os, snowpal.expr(), snowpal.splitting_tree());
}
