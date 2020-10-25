#pragma once

#include <bitset>
#include <climits>
#include <variant>

#include "lib/bit.h"
#include "lib/check.h"
#include "lib/word.h"


using EpsilonStorageType = unsigned char;

static constexpr int kEpsilonControlBits = 1;
static constexpr int kEpsilonDataBits = 7;
static constexpr int kMaxMonsterVariables = kEpsilonDataBits;

static_assert(
  kEpsilonControlBits + kEpsilonDataBits <=  // 1 for the type bit
  sizeof(EpsilonStorageType) * CHAR_BIT);


// Represents x_k
class EpsilonVariable {
public:
  EpsilonVariable() {}
  EpsilonVariable(int idx) : idx_(idx) {
    CHECK(is_valid());
  }

  bool is_valid() const { return 1 <= idx_ && idx_ < (1 << kEpsilonDataBits); }
  int idx() const { return idx_; }

private:
  int idx_ = 0;
};

// Represents (1 - x_{i_1} * ... * x_{i_k})
// Optimization potential: Inline vector and/or smaller int type or Word.
// TODO: Rename 'Monster' to smth, as it also covers (1 - x_i).
class EpsilonMonster {
public:
  EpsilonMonster() {}
  EpsilonMonster(std::bitset<kMaxMonsterVariables> indices) : indices_(std::move(indices)) {
    CHECK(is_valid());
  }

  bool is_valid() const { return indices_.any(); }
  const std::bitset<kMaxMonsterVariables>& indices() const { return indices_; }

private:
  // TODO: Convert to 0-based and back.
  std::bitset<kMaxMonsterVariables> indices_;
};

// Represents one of the two:
//   * x_k
//   * 1 - x_{i_1} * ... * x_{i_k}
// Idea behind the name: it's fancier than Delta, thus the next letter.
using Epsilon = std::variant<EpsilonVariable, EpsilonMonster>;


inline std::string to_string(const EpsilonVariable& var) {
  return absl::StrCat("x", var.idx());
}

inline std::string to_string(const EpsilonMonster& monster) {
  const auto& index_set = monster.indices();
  std::vector<int> index_list;
  for (int i = 0; i < kMaxMonsterVariables; ++i) {
    if (index_set[i]) {
      index_list.push_back(i);
    }
  }
  return absl::StrCat("(1 - ", str_join(index_list, ".", [](int x){
    return absl::StrCat("x", x);
  }), ")");
}

inline std::string to_string(const Epsilon& e) {
  return std::visit([](auto&& arg) { return to_string(arg); }, e);
}

namespace internal {
static constexpr int kEpsilonTypeBit = kEpsilonDataBits;

static constexpr int kEpsilonTypeVariable = 0;
static constexpr int kEpsilonTypeMonster = 1;

inline EpsilonStorageType epsilon_to_key(const Epsilon& e) {
  return std::visit(overloaded{
    [](const EpsilonVariable& var) {
      EpsilonStorageType ret = var.idx();
      CHECK_EQ(get_bit(ret, kEpsilonTypeBit), kEpsilonTypeVariable);
      return ret;
    },
    [](const EpsilonMonster& monster) {
      EpsilonStorageType ret = monster.indices().to_ulong();
      CHECK_EQ(get_bit(ret, kEpsilonTypeBit), kEpsilonTypeVariable);
      return EpsilonStorageType(set_bit(ret, kEpsilonTypeBit, kEpsilonTypeMonster));
    },
  }, e);
}

inline Epsilon key_to_epsilon(EpsilonStorageType key) {
  const int type = get_bit(key, kEpsilonTypeBit);
  const EpsilonStorageType data = set_bit(key, kEpsilonTypeBit, 0);
  if (type == kEpsilonTypeVariable) {
    return EpsilonVariable(data);
  } else if (type == kEpsilonTypeMonster) {
    return EpsilonMonster(std::bitset<kMaxMonsterVariables>(data));
  } else {
    FAIL(absl::StrCat("Bad Epsilon type: ", type));
  }
}

struct EpsilonExprParam {
  using ObjectT = std::vector<Epsilon>;
  using StorageT = Word;
  static StorageT object_to_key(const ObjectT& obj) {
    Word ret;
    for (const Epsilon& e : obj) {
      ret.push_back(epsilon_to_key(e));
    }
    return ret;
  }
  static ObjectT key_to_object(const StorageT& key) {
    ObjectT ret(key.size());
    std::transform(key.begin(), key.end(), ret.begin(), [](int ch){
      return key_to_epsilon(ch);
    });
    return ret;
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, "*");
  }
};
}  // namespace internal

using EpsilonExpr = Linear<internal::EpsilonExprParam>;

inline EpsilonExpr EVar(int idx) {
  return EpsilonExpr::single({EpsilonVariable(idx)});
}

inline EpsilonExpr EMonsterIndexSet(std::bitset<kMaxMonsterVariables> index_set) {
  return EpsilonExpr::single({EpsilonMonster(std::move(index_set))});
}

inline EpsilonExpr EMonsterIndexList(const std::vector<int> index_list) {
  std::bitset<kMaxMonsterVariables> index_set;
  for (const int i : index_list) {
    CHECK_LT(i, kMaxMonsterVariables);
    index_set[i] = 1;
  }
  return EMonsterIndexSet(std::move(index_set));
}

inline EpsilonExpr EMonsterRangeInclusive(int from, int to) {
  CHECK_LE(from, to);
  CHECK_LT(to, kMaxMonsterVariables);
  std::bitset<kMaxMonsterVariables> index_set;
  for (int i = from; i <= to; ++i) {
    index_set[i] = 1;
  }
  return EMonsterIndexSet(std::move(index_set));
}


// Optimization potential: make sure sorting always happens in key space.
inline bool operator<(const Epsilon& lhs, const Epsilon& rhs) {
  return internal::epsilon_to_key(lhs) < internal::epsilon_to_key(rhs);
}

EpsilonExpr epsilon_expr_substitute(
    const EpsilonExpr& expr,
    const std::vector<std::vector<int>>& new_products);

EpsilonExpr epsilon_expr_without_monsters(const EpsilonExpr& expr);
