// Represents a Grassmannian polylogarithm in Plücker coordinates.

#pragma once

#include <bitset>
#include <climits>

#include "check.h"
#include "coalgebra.h"
#include "pvector.h"
#include "x.h"


constexpr int kMaxGammaVariables = 16;

static_assert(kMaxGammaVariables <= sizeof(unsigned long) * CHAR_BIT);  // for std::bitset::to_ulong


// Represents a minor [d * d] in a matrix [d * n], n > d.
// Indices correspond to columns included into the minor.
// Generalization of `Delta` (assuming the latter uses only XForm::var).
class Gamma {
public:
  Gamma() {}
  Gamma(std::bitset<kMaxGammaVariables> indices) : indices_(std::move(indices)) {}
  Gamma(const XArgs& vars);

  bool is_nil() const { return indices_.none(); }

  const std::bitset<kMaxGammaVariables>& index_bitset() const { return indices_; }
  std::vector<int> index_vector() const;

  bool operator==(const Gamma& other) const { return indices_ == other.indices_; }
  bool operator< (const Gamma& other) const { return indices_.to_ulong() < other.indices_.to_ulong(); }

  template <typename H>
  friend H AbslHashValue(H h, const Gamma& gamma) {
    return H::combine(std::move(h), gamma.indices_);
  }

private:
  std::bitset<kMaxGammaVariables> indices_;  // 0-based
};

inline Gamma::Gamma(const XArgs& vars) {
  for (const X x : vars.as_x()) {
    if (x == Inf) {
      indices_ = {};
      return;
    }
    CHECK(x.is(XForm::var)) << to_string(x);
    const int idx = x.idx();
    CHECK_LE(1, idx);
    CHECK_LE(idx, kMaxGammaVariables);
    indices_[idx - 1] = true;  // 1-based => 0-based
  }
}

inline std::vector<int> Gamma::index_vector() const {
  std::bitset<kMaxGammaVariables> bitset = indices_;
  int current_index = 1;  // 0-based => 1-based
  std::vector<int> vector;
  while (bitset.any()) {
    if (bitset[0]) {
      vector.push_back(current_index);
    }
    bitset >>= 1;
    ++current_index;
  }
  return vector;
}

std::string to_string(const Gamma& g);


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
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
  static int object_to_weight(const ObjectT& obj) {
    return obj.size();
  }
};

struct GammaCoExprParam {
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
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), GammaExprParam::object_to_string);
  }
  static int object_to_weight(const ObjectT& obj) {
    return sum(mapped(obj, [](const auto& part) { return part.size(); }));
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct GammaNCoExprParam : GammaCoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), GammaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};
}  // namespace internal


using GammaExpr = Linear<internal::GammaExprParam>;
using GammaCoExpr = Linear<internal::GammaCoExprParam>;
using GammaNCoExpr = Linear<internal::GammaNCoExprParam>;
template<> struct CoExprForExpr<GammaExpr> { using type = GammaCoExpr; };
template<> struct NCoExprForExpr<GammaExpr> { using type = GammaNCoExpr; };

inline GammaExpr G(const XArgs& xargs) {
  Gamma g(xargs);
  return g.is_nil() ? GammaExpr{} : GammaExpr::single({g});
}


bool are_weakly_separated(const Gamma& g1, const Gamma& g2);
bool is_weakly_separated(const GammaExpr::ObjectT& term);
GammaExpr keep_weakly_separated(const GammaExpr& expr);
