// Type D polylog: Grassmannian (3,6) including both Plücker and non-Plücker coordinates.

#pragma once

#include <bitset>
#include <climits>

#include "bitset_util.h"
#include "check.h"
#include "coalgebra.h"
#include "gamma.h"
#include "pvector.h"


#define HAS_KAPPA_EXPR 1

struct KappaX {  // 12 34 56
  auto operator<=>(const KappaX&) const = default;
  template<typename H> friend H AbslHashValue(H h, const KappaX&) { return h; }
};
struct KappaY {  // 23 45 61
  auto operator<=>(const KappaY&) const = default;
  template<typename H> friend H AbslHashValue(H h, const KappaY&) { return h; }
};

using Kappa = std::variant<Gamma, KappaX, KappaY>;

std::string to_string(const Kappa& k);


namespace internal {
struct KappaExprParam {
  using ObjectT = std::vector<Kappa>;
  using StorageT = PVector<Kappa, 10>;
  static StorageT object_to_key(const ObjectT& obj) {
    return to_pvector<StorageT>(obj);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return to_vector(key);
  }
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::monostate element_uniformity_marker(const Kappa&) { return {}; }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
};
struct KappaICoExprParam {
  using PartExprParam = KappaExprParam;
  using ObjectT = std::vector<std::vector<Kappa>>;
  using PartStorageT = KappaExprParam::StorageT;
  using StorageT = PVector<PartStorageT, 2>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, KappaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, KappaExprParam::key_to_object);
  }
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  CO_DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), KappaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct KappaNCoExprParam : KappaICoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), KappaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};

struct KappaACoExprParam : KappaICoExprParam {
  static bool lyndon_compare(const VectorT::value_type& lhs, const VectorT::value_type& rhs) {
    using namespace cmp;
    return projected(lhs, rhs, [](const auto& v) {
      return std::tuple{desc_val(v.size()), asc_ref(v)};
    });
  };
};
}  // namespace internal


using KappaExpr = Linear<internal::KappaExprParam>;
using KappaICoExpr = Linear<internal::KappaICoExprParam>;
using KappaNCoExpr = Linear<internal::KappaNCoExprParam>;
using KappaACoExpr = Linear<internal::KappaACoExprParam>;
template<> struct ICoExprForExpr<KappaExpr> { using type = KappaICoExpr; };
template<> struct NCoExprForExpr<KappaExpr> { using type = KappaNCoExpr; };
template<> struct ACoExprForExpr<KappaExpr> { using type = KappaACoExpr; };

inline KappaExpr K(int a, int b, int c) {
  Gamma g({a, b, c});
  return g.is_nil() ? KappaExpr{} : KappaExpr::single({g});
}
inline KappaExpr K_X() {
  return KappaExpr::single({KappaX{}});
}
inline KappaExpr K_Y() {
  return KappaExpr::single({KappaY{}});
}


bool are_weakly_separated(const Kappa& k1, const Kappa& k2);
bool is_weakly_separated(const KappaExpr::ObjectT& term);
bool is_weakly_separated(const KappaNCoExpr::ObjectT& term);

KappaExpr gamma_expr_to_kappa_expr(const GammaExpr& expr);

KappaExpr NLog(int weight, const std::array<KappaExpr, 3>& triple);
