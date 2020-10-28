#include "mystic_algebra.h"

#include <sstream>

#include "algebra.h"
#include "epsilon.h"
#include "polylog.h"
#include "quasi_shuffle.h"


static EpsilonExpr to_expression(const EpsilonPack& pack) {
  return std::visit(overloaded{
    [](const std::vector<Epsilon>& product) {
      return EpsilonExpr::single(product);
    },
    [](const LiParam& formal_symbol) {
      return LiVec(formal_symbol);
    },
  }, pack);
}

using LiParamZipElement = std::pair<int, std::vector<int>>;

static std::vector<LiParamZipElement> li_param_to_vec(const LiParam& param) {
  return zip(param.weights(), param.points());
}

static LiParam vec_to_li_params(const std::vector<LiParamZipElement>& vec) {
  auto [weights, points] = unzip(vec);
  return LiParam(std::move(weights), std::move(points));
}

static LiParamZipElement glue_li_elements(
    const LiParamZipElement& lhs, const LiParamZipElement& rhs) {
  return LiParamZipElement{
    lhs.first + rhs.first,
    concat(lhs.second, rhs.second)
  };
}

static EpsilonExpr monom_mystic_product(
    const EpsilonPack& lhs,
    const EpsilonPack& rhs) {
  if (epsilon_pack_is_one(lhs)) {
    return EpsilonExpr::single(rhs);
  }
  if (epsilon_pack_is_one(rhs)) {
    return EpsilonExpr::single(lhs);
  }
  if (std::holds_alternative<std::vector<Epsilon>>(lhs) ||
      std::holds_alternative<std::vector<Epsilon>>(rhs)) {
    return shuffle_product_expr(
      to_expression(lhs),
      to_expression(rhs)
    );
  } else {
    CHECK(std::holds_alternative<LiParam>(lhs) &&
          std::holds_alternative<LiParam>(rhs));
    return EpsilonExpr::from_collection(
      mapped(
        quasi_shuffle_product(
          li_param_to_vec(std::get<LiParam>(lhs)),
          li_param_to_vec(std::get<LiParam>(rhs)),
          glue_li_elements
        ),
        [](const std::vector<LiParamZipElement>& vec) {
          return EpsilonPack(vec_to_li_params(vec));
        }
      )
    );
  }
}

// TODO: Move to util and apply to other porducts
template<typename LinearT>
static std::string short_linear_description(const LinearT& expr) {
  const auto& annotations = expr.annotations();
  if (annotations.size() == 0) {
    return "<0>";
  } else if (annotations.size() == 1) {
    std::stringstream ss;
    ss << annotations;
    return trimed(ss.str());
  } else {
    return absl::StrCat("<", annotations.size(), " ",
        en_plural(annotations.size(), "term"), ">");
  }
}
template<typename LinearT>
static std::string product_annotation(
    const std::string& product_name,
    const LinearT& lhs,
    const LinearT& rhs) {
  return absl::StrCat(product_name,
    "(", short_linear_description(lhs),
    ", ", short_linear_description(rhs), ")");
}

EpsilonCoExpr mystic_product(
    const EpsilonCoExpr& lhs,
    const EpsilonCoExpr& rhs) {
  return outer_product_expanding(
    lhs, rhs,
    [](const MultiWord& lhs_key, const MultiWord& rhs_key) -> EpsilonCoExpr {
      // TODO: Optimize: Don't pack/unpack things meaninglessly.
      const std::vector<EpsilonPack> lhs_term =
          EpsilonCoExpr::Param::key_to_object(lhs_key);
      const std::vector<EpsilonPack> rhs_term =
          EpsilonCoExpr::Param::key_to_object(rhs_key);
      CHECK_EQ(lhs_term.size(), rhs_term.size());
      CHECK_EQ(lhs_term.size(), 2) << "more coproduct component are not supported yet";
      return coproduct<EpsilonCoExpr>(
        monom_mystic_product(lhs_term[0], rhs_term[0]),
        monom_mystic_product(lhs_term[1], rhs_term[1])
      );
    }
  ).annotate(product_annotation("mystic", lhs, rhs));
}
