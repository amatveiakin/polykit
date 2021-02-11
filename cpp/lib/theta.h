#pragma once

#include <array>
#include <variant>
#include <vector>

#include "absl/algorithm/container.h"

#include "check.h"
#include "delta_ratio.h"
#include "epsilon.h"
#include "format.h"
#include "polylog_lira_param.h"


class ThetaComplement {
public:
  ThetaComplement() {}
  ThetaComplement(CompoundRatio ratio) : ratio_(std::move(ratio)) {}

  const CompoundRatio& ratio() const { return ratio_; }

  bool operator==(const ThetaComplement& other) const { return ratio_ == other.ratio_; }
  bool operator< (const ThetaComplement& other) const { return ratio_ <  other.ratio_; }

private:
  CompoundRatio ratio_;
};

// Represents one of the two:
//
//   >  (x_i - x_j)
//
//            (x_{i_1} - x_{j_1}) * ... * (x_{i_n} - x_{j_n})
//   >  1  -  -----------------------------------------------  ,  n >= 2
//            (x_{k_1} - x_{l_1}) * ... * (x_{k_n} - x_{l_n})
//
using Theta = std::variant<Delta, ThetaComplement>;

using ThetaPack = std::variant<std::vector<Theta>, LiraParam>;


inline std::string to_string(const ThetaComplement& complement) {
  return fmt::parens(fmt::diff("1", to_string(complement.ratio())));
}

inline std::string to_string(const Theta& t) {
  return std::visit([](auto&& arg) { return to_string(arg); }, t);
}

inline std::string to_string(const ThetaPack& pack) {
  return std::visit(overloaded{
    [](const std::vector<Theta>& product) {
      return product.empty() ? fmt::unity() : str_join(product, fmt::tensor_prod());
    },
    [](const LiraParam& formal_symbol) {
      return to_string(formal_symbol);
    },
  }, pack);
}


constexpr int kThetaCoExprComponents = 2;

namespace internal {
using ThetaStorageType = std::variant<Delta, CompoundRatioCompressed>;
using ThetaProductStorageType = PVector<ThetaStorageType, 4>;
using ThetaPackStorageType = std::variant<ThetaProductStorageType, LiraParamCompressed>;

inline ThetaStorageType theta_to_key(const Theta& t) {
  return std::visit(overloaded{
    [](const Delta& d) {
      return ThetaStorageType(d);
    },
    [](const ThetaComplement& complement) {
      return ThetaStorageType(compress_compound_ratio(complement.ratio()));
    },
  }, t);
}

inline Theta key_to_theta(ThetaStorageType key) {
  return std::visit(overloaded{
    [](const Delta& d) {
      return Theta(d);
    },
    [](const CompoundRatioCompressed& complement_compressed) {
      return Theta(ThetaComplement(uncompress_compound_ratio(complement_compressed)));
    },
  }, key);
}

inline ThetaPackStorageType theta_pack_to_key(const ThetaPack& pack) {
  return std::visit(overloaded{
    [](const std::vector<Theta>& product) {
      return ThetaPackStorageType(
        mapped_to_pvector<ThetaProductStorageType>(product, theta_to_key)
      );
    },
    [](const LiraParam& formal_symbol) {
      return ThetaPackStorageType(lira_param_to_key(formal_symbol));
    },
  }, pack);
}

inline ThetaPack key_to_theta_pack(const ThetaPackStorageType& key) {
  return std::visit(overloaded{
    [](const ThetaProductStorageType& product) {
      return ThetaPack(mapped(product, key_to_theta));
    },
    [](const LiraParamCompressed& compressed) {
      return ThetaPack(key_to_lira_param(compressed));
    },
  }, key);
}

struct ThetaExprParam {
  using ObjectT = ThetaPack;
  using ProductT = ThetaProductStorageType;
  using StorageT = ThetaPackStorageType;
  using VectorT = ProductT;
  static StorageT object_to_key(const ObjectT& obj) {
    return theta_pack_to_key(obj);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return key_to_theta_pack(key);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    CHECK(std::holds_alternative<ProductT>(lhs) && std::holds_alternative<ProductT>(rhs))
        << "Tensor product for formal symbols is not defined";
    return concat(std::get<ProductT>(lhs), std::get<ProductT>(rhs));
  }
  static int object_to_weight(const ObjectT& obj) {
    return std::visit(overloaded{
      [](const std::vector<Theta>& product) -> int {
        return product.size();
      },
      [](const LiraParam& formal_symbol) -> int {
        return formal_symbol.total_weight();
      },
    }, obj);
  }
  static VectorT key_to_vector(const StorageT& key) {
    CHECK(std::holds_alternative<ProductT>(key)) << "Vector form is not defined for formal symbols";
    return std::get<ProductT>(key);
  }
  static StorageT vector_to_key(const VectorT& vec) {
    return vec;
  }
};

struct ThetaCoExprParam {
  using ObjectT = std::array<ThetaPack, kThetaCoExprComponents>;
  using StorageT = std::array<ThetaExprParam::StorageT, kThetaCoExprComponents>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_array(obj, ThetaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped_array(key, ThetaExprParam::key_to_object);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_hopf());
  }
  static constexpr bool coproduct_is_lie_algebra = false;
};
}  // namespace internal


using ThetaExpr = Linear<internal::ThetaExprParam>;
using ThetaCoExpr = Linear<internal::ThetaCoExprParam>;


// Whether expr is one w.r.t. shuffle multiplication.
inline bool theta_pack_is_unity(const ThetaPack& pack) {
  const auto* as_product = std::get_if<std::vector<Theta>>(&pack);
  return as_product && as_product->empty();
}

inline ThetaPack ThetaUnityElement() { return std::vector<Theta>{}; }
inline ThetaExpr TUnity() { return ThetaExpr::single(ThetaUnityElement()); }

ThetaExpr TRatio(const CompoundRatio& ratio);
inline ThetaExpr TRatio(std::initializer_list<int> indices) { return TRatio(CrossRatio(indices)); }

ThetaExpr TComplement(const CompoundRatio& ratio);
inline ThetaExpr TComplement(std::initializer_list<std::initializer_list<int>> indices);

inline ThetaExpr TFormalSymbol(const LiraParam& lira_param) { return ThetaExpr::single(lira_param); }


ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<CompoundRatio>& compound_ratios);
ThetaCoExpr epsilon_coexpr_to_theta_coexpr(
    const EpsilonCoExpr& expr,
    const std::vector<CompoundRatio>& ratios);

ThetaExpr delta_expr_to_theta_expr(const DeltaExpr& expr);
DeltaExpr theta_expr_to_delta_expr(const ThetaExpr& expr);

ThetaExpr update_foreweight(
    const ThetaExpr& expr,
    int new_foreweight);

StringExpr count_functions(const ThetaExpr& expr);

// "Monster" is something that cannot be decomposed into a product of Plücker coordinates.
ThetaExpr theta_expr_without_monsters(const ThetaExpr& expr);
ThetaExpr theta_expr_keep_monsters(const ThetaExpr& expr);
ThetaCoExpr theta_coexpr_without_monsters(const ThetaCoExpr& expr);
ThetaCoExpr theta_coexpr_keep_monsters(const ThetaCoExpr& expr);
