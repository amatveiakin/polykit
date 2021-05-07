// EpsilonExpr is a linear expression where each term is a tensor product with each factor
//   > either equals to x_i,
//   > or equals to (1 - x_{j_1}*...*x_{j_m}).
//
// Alternatively, an entire term may be a formal symbol corresponding to a Li function.

#pragma once

#include <bitset>
#include <climits>
#include <variant>

#include "bit.h"
#include "check.h"
#include "coalgebra.h"
#include "polylog_li_param.h"
#include "pvector.h"


constexpr int kMaxComplementVariables = 15;

namespace internal {
constexpr int kEpsilonControlBits = 1;
constexpr int kEpsilonDataBits = kMaxComplementVariables;
constexpr int kEpsilonTotalBits = kEpsilonControlBits + kEpsilonDataBits;
static_assert(kEpsilonTotalBits <= sizeof(unsigned long) * CHAR_BIT);  // for std::bitset::to_ulong
using EpsilonStorageType = std::bitset<kEpsilonTotalBits>;
}  // namespace internal


// Represents x_k
class EpsilonVariable {
public:
  EpsilonVariable() {}
  EpsilonVariable(int idx) : idx_(idx) {
    CHECK(is_valid()) << "EpsilonVariable index = " << idx_;
  }

  bool is_valid() const { return 1 <= idx_ && idx_ < (1 << internal::kEpsilonDataBits); }
  int idx() const { return idx_; }

  bool operator==(const EpsilonVariable& other) const { return idx_ == other.idx_; }
  bool operator< (const EpsilonVariable& other) const { return idx_ <  other.idx_; }

private:
  int idx_ = 0;
};

// Represents (1 - x_{i_1} * ... * x_{i_k})
class EpsilonComplement {
public:
  EpsilonComplement() {}
  EpsilonComplement(std::bitset<kMaxComplementVariables> indices) : indices_(std::move(indices)) {
    CHECK(is_valid());
  }

  bool is_valid() const { return indices_.any(); }
  const std::bitset<kMaxComplementVariables>& indices() const { return indices_; }

  bool operator==(const EpsilonComplement& other) const { return indices_ == other.indices_; }
  bool operator< (const EpsilonComplement& other) const { return indices_.to_ulong() < other.indices_.to_ulong(); }

private:
  // TODO: Convert to 0-based and back.
  std::bitset<kMaxComplementVariables> indices_;
};

// Represents one of the two:
//   * x_k
//   * 1 - x_{i_1} * ... * x_{i_k}
// Idea behind the name: it's fancier than Delta, thus the next letter.
using Epsilon = std::variant<EpsilonVariable, EpsilonComplement>;

using EpsilonPack = std::variant<std::vector<Epsilon>, LiParam>;


inline std::string to_string(const EpsilonVariable& var) {
  return fmt::var(var.idx());
}

inline std::string to_string(const EpsilonComplement& complement) {
  const auto& index_set = complement.indices();
  std::vector<int> index_list;
  for (int i : range(kMaxComplementVariables)) {
    if (index_set[i]) {
      index_list.push_back(i);
    }
  }
  return fmt::parens(fmt::diff(
    "1",
    str_join(index_list, fmt::dot(), fmt::var),
    HSpacing::sparse
  ));
}

inline std::string to_string(const Epsilon& e) {
  return std::visit([](auto&& arg) { return to_string(arg); }, e);
}

inline std::string to_string(const EpsilonPack& pack) {
  return std::visit(overloaded{
    [](const std::vector<Epsilon>& product) {
      return product.empty() ? fmt::unity() : str_join(product, fmt::tensor_prod());
    },
    [](const LiParam& formal_symbol) {
      return to_string(formal_symbol);
    },
  }, pack);
}


namespace internal {
constexpr int kEpsilonTypeBit = kEpsilonDataBits;

constexpr int kEpsilonTypeVariable = 0;
constexpr int kEpsilonTypeComplement = 1;

inline EpsilonStorageType epsilon_to_key(const Epsilon& e) {
  return std::visit(overloaded{
    [](const EpsilonVariable& var) {
      EpsilonStorageType ret;
      ret.set(var.idx(), 1);
      CHECK(ret[kEpsilonTypeBit] == static_cast<bool>(0));
      ret[kEpsilonTypeBit] = kEpsilonTypeVariable;
      return ret;
    },
    [](const EpsilonComplement& complement) {
      EpsilonStorageType ret = convert_bitset<kEpsilonTotalBits>(complement.indices());
      CHECK(ret[kEpsilonTypeBit] == static_cast<bool>(0));
      ret[kEpsilonTypeBit] = kEpsilonTypeComplement;
      return ret;
    },
  }, e);
}

inline Epsilon key_to_epsilon(EpsilonStorageType key) {
  const int type = key[kEpsilonTypeBit];
  key[kEpsilonTypeBit] = 0;
  if (type == kEpsilonTypeVariable) {
    return EpsilonVariable(which_power_of_two(key.to_ulong()));
  } else if (type == kEpsilonTypeComplement) {
    return EpsilonComplement(convert_bitset<kMaxComplementVariables>(key));
  } else {
    FATAL(absl::StrCat("Bad Epsilon type: ", type));
  }
}

struct EpsilonExprParam {
  using ObjectT = EpsilonPack;
  using ProductT = PVector<EpsilonStorageType, 8>;
  using StorageT = std::variant<ProductT, LiParamCompressed>;
  using VectorT = ProductT;
  LYNDON_COMPARE_DEFAULT
  static StorageT object_to_key(const ObjectT& obj) {
    return std::visit(overloaded{
      [](const std::vector<Epsilon>& product) -> StorageT {
        return mapped_to_pvector<ProductT>(product, epsilon_to_key);
      },
      [](const LiParam& formal_symbol) -> StorageT {
        return li_param_to_key(formal_symbol);
      },
    }, obj);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return std::visit(overloaded{
      [](const ProductT& product) -> ObjectT {
        return mapped(product, key_to_epsilon);
      },
      [](const LiParamCompressed& compressed) -> ObjectT {
        return key_to_li_param(compressed);
      },
    }, key);
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
      [](const std::vector<Epsilon>& product) -> int {
        return product.size();
      },
      [](const LiParam& formal_symbol) -> int {
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

struct EpsilonCoExprParam {
  using ObjectT = std::vector<EpsilonPack>;
  using PartStorageT = EpsilonExprParam::StorageT;
  using StorageT = PVector<PartStorageT, 2>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, EpsilonExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, EpsilonExprParam::key_to_object);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_hopf());
  }
  static constexpr bool coproduct_is_lie_algebra = false;
};
}  // namespace internal


using EpsilonExpr = Linear<internal::EpsilonExprParam>;
using EpsilonCoExpr = Linear<internal::EpsilonCoExprParam>;
template<> struct CoExprForExpr<EpsilonExpr> { using type = EpsilonCoExpr; };

// Whether expr is one w.r.t. shuffle multiplication.
inline bool is_unity(const EpsilonPack& pack) {
  const auto* as_product = std::get_if<std::vector<Epsilon>>(&pack);
  return as_product && as_product->empty();
}

inline EpsilonPack EpsilonUnityElement() {
  return std::vector<Epsilon>{};
}

inline EpsilonExpr EUnity() {
  return EpsilonExpr::single(EpsilonUnityElement());
}

inline EpsilonExpr EVar(int idx) {
  return EpsilonExpr::single(std::vector<Epsilon>{EpsilonVariable(idx)});
}

inline EpsilonExpr EComplementIndexSet(std::bitset<kMaxComplementVariables> index_set) {
  return EpsilonExpr::single(std::vector<Epsilon>{EpsilonComplement(std::move(index_set))});
}

inline EpsilonExpr EComplementIndexList(const std::vector<int> index_list) {
  std::bitset<kMaxComplementVariables> index_set;
  for (const int i : index_list) {
    CHECK_LT(i, kMaxComplementVariables);
    index_set[i] = 1;
  }
  return EComplementIndexSet(std::move(index_set));
}

inline EpsilonExpr EComplementRangeInclusive(int from, int to) {
  CHECK_LE(from, to);
  CHECK_LT(to, kMaxComplementVariables);
  std::bitset<kMaxComplementVariables> index_set;
  for (int i : range_incl(from, to)) {
    index_set[i] = 1;
  }
  return EComplementIndexSet(std::move(index_set));
}

inline EpsilonExpr EFormalSymbolPositive(const LiParam& li_param) {
  return EpsilonExpr::single(li_param);
}

inline EpsilonExpr EFormalSymbolSigned(const LiParam& li_param) {
  return li_param.sign() * EFormalSymbolPositive(li_param);
}


// Gets a list {{y_1_1, y_1_2, ...}, ..., {y_n_1, y_n_2, ...}}, replaces
// each variable x_i in expr with a product (y_i_1 * y_i_2 * ...)
EpsilonExpr substitute_variables(
    const EpsilonExpr& expr,
    const std::vector<std::vector<int>>& new_products);

// Here "monster" means "EpsilonComplement with more than one variable".
// Motivation: we call a term "monster" if it cannot be reduced to a cross ratio
// after we substitute a cross ratio for each variable. One minus cross ratio
// is always a cross ratio. One minus a product of cross ratio is usually not
// and here we assume that it never is.
EpsilonExpr without_monsters(const EpsilonExpr& expr);
EpsilonExpr keep_monsters(const EpsilonExpr& expr);
EpsilonCoExpr without_monsters(const EpsilonCoExpr& expr);
EpsilonCoExpr keep_monsters(const EpsilonCoExpr& expr);
