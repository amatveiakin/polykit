#include "mystic_algebra.h"

#include "algebra.h"
#include "epsilon.h"
#include "polylog.h"
#include "quasi_shuffle.h"
#include "zip.h"


constexpr int kForeweight = 1;

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
  CHECK_EQ(param.foreweight(), kForeweight) << to_string(param);
  return zip(param.weights(), param.points());
}

static LiParam vec_to_li_params(const std::vector<LiParamZipElement>& vec) {
  auto [weights, points] = unzip(vec);
  return LiParam(kForeweight, std::move(weights), std::move(points));
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
  if (epsilon_pack_is_unity(lhs)) {
    return EpsilonExpr::single(rhs);
  }
  if (epsilon_pack_is_unity(rhs)) {
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

static EpsilonExpr monom_key_mystic_product(
    const Word& lhs_key,
    const Word& rhs_key) {
  return monom_mystic_product(
    EpsilonExpr::Param::key_to_object(lhs_key),
    EpsilonExpr::Param::key_to_object(rhs_key));
}

EpsilonExpr mystic_product(
    const EpsilonExpr& lhs,
    const EpsilonExpr& rhs) {
  return outer_product_expanding(lhs, rhs, monom_key_mystic_product,
    AnnFunction("mystic"));
}

EpsilonExpr mystic_product(
    const absl::Span<const EpsilonExpr>& expressions) {
  return outer_product_expanding(expressions, monom_key_mystic_product,
    AnnFunction("mystic"));
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
    },
    AnnFunction("mystic")
  );
}


using LiraParamZipElement = std::pair<int, CompoundRatio>;

static std::vector<LiraParamZipElement> lira_param_to_vec(const LiraParam& param) {
  CHECK_EQ(param.foreweight(), kForeweight) << to_string(param);
  return zip(param.weights(), param.ratios());
}

static LiraParam vec_to_lira_params(const std::vector<LiraParamZipElement>& vec) {
  auto [weights, ratios] = unzip(vec);
  return LiraParam(kForeweight, std::move(weights), std::move(ratios));
}

static LiraParamZipElement glue_lira_elements(
    const LiraParamZipElement& lhs, const LiraParamZipElement& rhs) {
  CompoundRatio ratio = lhs.second;
  ratio.add(rhs.second);
  return LiraParamZipElement{lhs.first + rhs.first, ratio};
}

static ThetaExpr monom_quasi_shuffle_product(
    const ThetaPack& lhs,
    const ThetaPack& rhs) {
  CHECK(std::holds_alternative<LiraParam>(lhs) &&
        std::holds_alternative<LiraParam>(rhs));
  return ThetaExpr::from_collection(
    mapped(
      quasi_shuffle_product(
        lira_param_to_vec(std::get<LiraParam>(lhs)),
        lira_param_to_vec(std::get<LiraParam>(rhs)),
        glue_lira_elements
      ),
      [](const std::vector<LiraParamZipElement>& vec) {
        return ThetaPack(vec_to_lira_params(vec));
      }
    )
  );
}

static ThetaExpr monom_key_quasi_shuffle_product(
    const MultiWord& lhs_key,
    const MultiWord& rhs_key) {
  return monom_quasi_shuffle_product(
    ThetaExpr::Param::key_to_object(lhs_key),
    ThetaExpr::Param::key_to_object(rhs_key));
}

ThetaExpr theta_expr_quasi_shuffle_product(
    const absl::Span<const ThetaExpr>& expressions) {
  return outer_product_expanding(expressions, monom_key_quasi_shuffle_product,
    AnnFunction("quasishuffle"));
}
