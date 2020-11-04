#pragma once

#include <bitset>
#include <climits>
#include <variant>

#include "lib/bit.h"
#include "lib/check.h"
#include "lib/polylog_param.h"
#include "lib/word.h"


using EpsilonStorageType = unsigned char;

constexpr int kEpsilonControlBits = 1;
constexpr int kEpsilonDataBits = 7;
constexpr int kMaxComplementVariables = kEpsilonDataBits;

static_assert(
  kEpsilonControlBits + kEpsilonDataBits <=  // 1 for the type bit
  sizeof(EpsilonStorageType) * CHAR_BIT);


// Represents x_k
class EpsilonVariable {
public:
  EpsilonVariable() {}
  EpsilonVariable(int idx) : idx_(idx) {
    CHECK(is_valid()) << "EpsilonVariable index = " << idx_;
  }

  bool is_valid() const { return 1 <= idx_ && idx_ < (1 << kEpsilonDataBits); }
  int idx() const { return idx_; }

  bool operator==(const EpsilonVariable& other) const {
    return idx_ == other.idx_;
  }

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

  bool operator==(const EpsilonComplement& other) const {
    return indices_ == other.indices_;
  }

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

enum EpsilonPackType {
  kEpsilonPackTypeProduct = 0,
  kEpsilonPackTypeFormalSymbol = 1,
};


inline std::string to_string(const EpsilonVariable& var) {
  return fmt::var(var.idx());
}

inline std::string to_string(const EpsilonComplement& complement) {
  const auto& index_set = complement.indices();
  std::vector<int> index_list;
  for (int i = 0; i < kMaxComplementVariables; ++i) {
    if (index_set[i]) {
      index_list.push_back(i);
    }
  }
  return absl::StrCat("(1 - ", str_join(index_list, fmt::dot(), fmt::var), ")");
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
      EpsilonStorageType ret = var.idx();
      CHECK_EQ(get_bit(ret, kEpsilonTypeBit), kEpsilonTypeVariable);
      return ret;
    },
    [](const EpsilonComplement& complement) {
      EpsilonStorageType ret = complement.indices().to_ulong();
      CHECK_EQ(get_bit(ret, kEpsilonTypeBit), kEpsilonTypeVariable);
      return EpsilonStorageType(set_bit(ret, kEpsilonTypeBit, kEpsilonTypeComplement));
    },
  }, e);
}

inline Epsilon key_to_epsilon(EpsilonStorageType key) {
  const int type = get_bit(key, kEpsilonTypeBit);
  const EpsilonStorageType data = set_bit(key, kEpsilonTypeBit, 0);
  if (type == kEpsilonTypeVariable) {
    return EpsilonVariable(data);
  } else if (type == kEpsilonTypeComplement) {
    return EpsilonComplement(std::bitset<kMaxComplementVariables>(data));
  } else {
    FATAL(absl::StrCat("Bad Epsilon type: ", type));
  }
}

inline Word epsilon_pack_to_key(const EpsilonPack& pack) {
  return std::visit(overloaded{
    [](const std::vector<Epsilon>& product) {
      Word ret;
      ret.push_back(kEpsilonPackTypeProduct);
      for (const Epsilon& e : product) {
        ret.push_back(epsilon_to_key(e));
      }
      return ret;
    },
    [](const LiParam& formal_symbol) {
      Word ret;
      ret.push_back(kEpsilonPackTypeFormalSymbol);
      ret.append_word(li_param_to_key(formal_symbol));
      return ret;
    },
  }, pack);
}

inline EpsilonPackType key_to_epsilon_pack_type(const Word& key) {
  CHECK(!key.empty());
  return EpsilonPackType(key.front());
}

inline absl::Span<const unsigned char> key_to_epsilon_pack_data(const Word& key) {
  CHECK(!key.empty());
  return key.span().subspan(1);
}

inline EpsilonPack key_to_epsilon_pack(const Word& key) {
  const EpsilonPackType type = key_to_epsilon_pack_type(key);
  const auto data = key_to_epsilon_pack_data(key);
  switch (type) {
    case kEpsilonPackTypeProduct:
      return mapped(data, [](int ch){
        return key_to_epsilon(ch);
      });
    case kEpsilonPackTypeFormalSymbol:
      return key_to_li_param(Word(data));
  }
  FATAL(absl::StrCat("Bad EpsilonPack type = ", type, "; key = ", to_string(key)));
}

struct EpsilonExprParam {
  using ObjectT = EpsilonPack;
  using StorageT = Word;
  static StorageT object_to_key(const ObjectT& obj) {
    return epsilon_pack_to_key(obj);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return key_to_epsilon_pack(key);
  }
  static std::string object_to_string(const ObjectT& obj) {
    return to_string(obj);
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    const auto type = key_to_epsilon_pack_type(lhs);
    CHECK_EQ(type, key_to_epsilon_pack_type(rhs));
    CHECK_EQ(type, kEpsilonPackTypeProduct) << "Tensor product for formal symbols is not defined";
    // TODO: Don't create an intermediate word
    return concat_words(lhs, Word(key_to_epsilon_pack_data(rhs)));
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
  static StorageT shuffle_preprocess(const StorageT& key) {
    const int type = key_to_epsilon_pack_type(key);
    CHECK_EQ(type, kEpsilonPackTypeProduct) << "Lyndon for formal symbols is not defined";
    return Word(key_to_epsilon_pack_data(key));
  }
  static StorageT shuffle_postprocess(const StorageT& key) {
    Word ret;
    ret.push_back(kEpsilonPackTypeProduct);
    ret.append_word(key);
    return ret;
  }
};
}  // namespace internal

using EpsilonExpr = Linear<internal::EpsilonExprParam>;

// Whether expr is one w.r.t. shuffle multiplication.
inline bool epsilon_pack_is_unity(const EpsilonPack& pack) {
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
  for (int i = from; i <= to; ++i) {
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


// Optimization potential: make sure sorting always happens in key space.
inline bool operator<(const Epsilon& lhs, const Epsilon& rhs) {
  return internal::epsilon_to_key(lhs) < internal::epsilon_to_key(rhs);
}

EpsilonExpr epsilon_expr_substitute(
    const EpsilonExpr& expr,
    const std::vector<std::vector<int>>& new_products);

// Removes EpsilonComplement-s with more than one variable.
// TODO: Add EpsilonCoExpr overload
EpsilonExpr epsilon_expr_without_monsters(const EpsilonExpr& expr);
