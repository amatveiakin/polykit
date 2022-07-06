// Represents a Grassmannian polylogarithm in Plücker coordinates.

#pragma once

#include <bitset>
#include <climits>

#include "bitset_util.h"
#include "check.h"
#include "coalgebra.h"
#include "delta.h"
#include "pvector.h"


#define HAS_GAMMA_EXPR 1

constexpr int kMaxGammaVariables = 16;

static_assert(kMaxGammaVariables <= sizeof(unsigned long) * CHAR_BIT);  // for std::bitset::to_ulong


// Represents a minor [d * d] in a matrix [d * n], n > d.
// Indices correspond to columns included into the minor.
// Generalization of `Delta` (assuming the latter uses only XForm::var).
class Gamma {
public:
  using BitsetT = std::bitset<kMaxGammaVariables>;
  static constexpr int kBitsetOffset = 1;

  Gamma() {}
  explicit Gamma(BitsetT indices) : indices_(std::move(indices)) {}
  explicit Gamma(const std::vector<int>& vars);

  bool is_nil() const { return indices_.none(); }
  int dimension() const { return indices_.count(); }

  const BitsetT& index_bitset() const { return indices_; }
  std::vector<int> index_vector() const { return bitset_to_vector(indices_, kBitsetOffset); }

  bool operator==(const Gamma& other) const { return indices_ == other.indices_; }
  bool operator< (const Gamma& other) const { return indices_.to_ulong() < other.indices_.to_ulong(); }

  template <typename H>
  friend H AbslHashValue(H h, const Gamma& gamma) {
    return H::combine(std::move(h), gamma.indices_);
  }

private:
  BitsetT indices_;  // 0-based
};

inline Gamma::Gamma(const std::vector<int>& vars) {
  const auto indices_or = vector_to_bitset_or<BitsetT>(vars, kBitsetOffset);
  if (indices_or.has_value()) {
    indices_ = indices_or.value();
  } else {
    // Keep empty: this means Gamma is nil.
  }
}

struct GammaUniformityMarker {
  int dimension = 0;
  bool operator==(const GammaUniformityMarker& other) const { return dimension == other.dimension; }
  bool operator!=(const GammaUniformityMarker& other) const { return dimension != other.dimension; }
};

std::string to_string(const Gamma& g);
std::string to_string(const GammaUniformityMarker& marker);


namespace internal {
struct GammaExprParam {
  using ObjectT = std::vector<Gamma>;
  using StorageT = PVector<Gamma, 10>;
  static StorageT object_to_key(const ObjectT& obj) {
    return to_pvector<StorageT>(obj);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return to_vector(key);
  }
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static GammaUniformityMarker element_uniformity_marker(const Gamma& g) { return { g.dimension() }; }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
  static int object_to_dimension(const ObjectT& obj) {
    CHECK(!obj.empty());
    CHECK(all_equal(obj, [](const auto& v) { return v.dimension(); }));
    return obj.front().dimension();
  }
};

struct GammaICoExprParam {
  using PartExprParam = GammaExprParam;
  using ObjectT = std::vector<std::vector<Gamma>>;
  using PartStorageT = GammaExprParam::StorageT;
  using StorageT = PVector<PartStorageT, 2>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, GammaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, GammaExprParam::key_to_object);
  }
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  CO_DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), GammaExprParam::object_to_string);
  }
  static int object_to_dimension(const ObjectT& obj) {
    CHECK(!obj.empty());
    CHECK(all_equal(obj, &PartExprParam::object_to_dimension));
    return PartExprParam::object_to_dimension(obj.front());
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct GammaNCoExprParam : GammaICoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), GammaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};

struct GammaACoExprParam : GammaICoExprParam {
  static bool lyndon_compare(const VectorT::value_type& lhs, const VectorT::value_type& rhs) {
    using namespace cmp;
    return projected(lhs, rhs, [](const auto& v) {
      return std::tuple{desc_val(v.size()), asc_ref(v)};
    });
  };
};
}  // namespace internal


using GammaExpr = Linear<internal::GammaExprParam>;
using GammaICoExpr = Linear<internal::GammaICoExprParam>;
using GammaNCoExpr = Linear<internal::GammaNCoExprParam>;
using GammaACoExpr = Linear<internal::GammaACoExprParam>;
template<> struct ICoExprForExpr<GammaExpr> { using type = GammaICoExpr; };
template<> struct NCoExprForExpr<GammaExpr> { using type = GammaNCoExpr; };
template<> struct ACoExprForExpr<GammaExpr> { using type = GammaACoExpr; };

inline GammaExpr G(const std::vector<int>& vars) {
  Gamma g(vars);
  return g.is_nil() ? GammaExpr{} : GammaExpr::single({g});
}
inline GammaExpr plucker(const std::vector<int>& vars) {
  // Standard notation for Plucker coordinates.
  // TODO: Factor out fmt::bars for this.
  return G(vars).annotate(absl::StrCat("|", str_join(sorted(vars), ","), "|"));
}


GammaExpr substitute_variables(const GammaExpr& expr, const std::vector<int>& new_points);
GammaNCoExpr substitute_variables(const GammaNCoExpr& expr, const std::vector<int>& new_points);

GammaExpr project_on(int axis, const GammaExpr& expr);

bool are_weakly_separated(const Gamma& g1, const Gamma& g2);
bool is_weakly_separated(const GammaExpr::ObjectT& term);
bool is_weakly_separated(const GammaNCoExpr::ObjectT& term);

bool passes_normalize_remove_consecutive(const GammaExpr::ObjectT& term, int dimension, int num_points);
GammaExpr normalize_remove_consecutive(const GammaExpr& expr, int dimension, int num_points);
GammaExpr normalize_remove_consecutive(const GammaExpr& expr);

// Requires that each term is a simple variable difference, i.e. terms like (x_i + x_j)
// or (x_i + 0) are not allowed.
GammaExpr delta_expr_to_gamma_expr(const DeltaExpr& expr);

// Requires that expression is dimension 2.
DeltaExpr gamma_expr_to_delta_expr(const GammaExpr& expr);

GammaExpr pullback(const DeltaExpr& expr, const std::vector<int>& bonus_points);
GammaExpr pullback(const GammaExpr& expr, const std::vector<int>& bonus_points);
GammaNCoExpr pullback(const GammaNCoExpr& expr, const std::vector<int>& bonus_points);

GammaExpr plucker_dual(const DeltaExpr& expr, const std::vector<int>& point_universe);
GammaExpr plucker_dual(const GammaExpr& expr, const std::vector<int>& point_universe);
GammaNCoExpr plucker_dual(const GammaNCoExpr& expr, const std::vector<int>& point_universe);

GammaExpr symmetrize_double(const GammaExpr& expr, int num_points);
GammaExpr symmetrize_loop(const GammaExpr& expr, int num_points);

std::vector<int> common_vars(const GammaExpr::ObjectT& term);
std::vector<int> all_vars(const GammaExpr::ObjectT& term);
