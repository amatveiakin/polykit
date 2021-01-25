#pragma once

#include <array>
#include <variant>
#include <vector>

#include "absl/algorithm/container.h"

#include "check.h"
#include "delta_ratio.h"
#include "epsilon.h"
#include "format.h"
#include "multiword.h"
#include "polylog_cross_ratio_param.h"
#include "word.h"


class ThetaComplement {
public:
  ThetaComplement() {}
  ThetaComplement(CompoundRatio ratio) : ratio_(std::move(ratio)) {}

  const CompoundRatio& ratio() const { return ratio_; }

  bool operator==(const ThetaComplement& other) const { return ratio_ == other.ratio_; }

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


#define OLD_THETA 0
#if OLD_THETA
namespace internal {
using ThetaStorageType = Word;

enum ThetaPackType {
  kThetaPackTypeProduct = 0,
  kThetaPackTypeFormalSymbol = 1,
};

// Idea: Store just the data. We don't need a type bit, because Delta
// is always one byte and CompoundRatio is always more.
inline ThetaStorageType theta_to_key(const Theta& t) {
  return std::visit(overloaded{
    [](const Delta& d) {
      return Word({delta_alphabet_mapping.to_alphabet(d)});
    },
    [](const ThetaComplement& complement) {
      Compressor compressor;
      complement.ratio().compress(compressor);
      return Word(std::move(compressor).result());
    },
  }, t);
}

inline Theta key_to_theta(ThetaStorageType key) {
  if (key.size() == 1) {
    return delta_alphabet_mapping.from_alphabet(key.front());
  } else {
    Decompressor decompressor(key.span());
    Theta ret = ThetaComplement(CompoundRatio::from_compressed(decompressor));
    CHECK(decompressor.done());
    return ret;
  }
}

inline MultiWord theta_pack_to_key(const ThetaPack& pack) {
  return std::visit(overloaded{
    [](const std::vector<Theta>& product) {
      MultiWord ret;
      ret.append_segment(Word({kThetaPackTypeProduct}));
      for (const Theta& t : product) {
        ret.append_segment(theta_to_key(t));
      }
      return ret;
    },
    [](const LiraParam& formal_symbol) {
      MultiWord ret;
      ret.append_segment(Word({kThetaPackTypeFormalSymbol}));
      ret.append_segment(lira_param_to_key(formal_symbol));
      return ret;
    },
  }, pack);
}

inline ThetaPackType key_to_theta_pack_type(const MultiWord& key) {
  CHECK(!key.empty());
  CHECK(!key.segment(0).empty());
  return ThetaPackType(key.segment(0)[0]);
}

class ThetaPackDataRange {
public:
  ThetaPackDataRange(MultiWord::const_iterator begin, MultiWord::const_iterator end)
    : begin_(begin), end_(end) {}
  MultiWord::const_iterator begin() const { return begin_; }
  MultiWord::const_iterator end() const { return end_; }
private:
  MultiWord::const_iterator begin_;
  MultiWord::const_iterator end_;
};

inline ThetaPackDataRange key_to_theta_pack_data_range(const MultiWord& key) {
  auto begin = key.begin();
  ++begin;
  return ThetaPackDataRange{begin, key.end()};
}

inline ThetaPack key_to_theta_pack(const MultiWord& key) {
  const ThetaPackType type = key_to_theta_pack_type(key);
  const auto data_range = key_to_theta_pack_data_range(key);
  switch (type) {
    case kThetaPackTypeProduct: {
      std::vector<Theta> ret;
      for (const auto& segment : data_range) {
        ret.push_back(key_to_theta(Word(segment)));
      }
      return ret;
    }
    case kThetaPackTypeFormalSymbol: {
      auto next = data_range.begin();
      ++next;
      CHECK(next == data_range.end());
      return key_to_lira_param(Word(*data_range.begin()));
    }
  }
  FATAL(absl::StrCat("Bad ThetaPack type = ", type, "; key = ", to_string(key)));
}

struct ThetaExprParam {
  using ObjectT = ThetaPack;
  using StorageT = MultiWord;
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
    const auto type = key_to_theta_pack_type(lhs);
    CHECK_EQ(type, key_to_theta_pack_type(rhs));
    CHECK_EQ(type, kThetaPackTypeProduct) << "Tensor product for formal symbols is not defined";
    MultiWord ret = lhs;
    for (const auto& segment : key_to_theta_pack_data_range(rhs)) {
      ret.append_segment(segment);
    }
    return ret;
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
};
}  // namespace internal
#else
namespace internal {
using ThetaStorageType = std::variant<Delta, CompressedBlob>;
// Optimization potential: Compress LiraParam too.
using ThetaProductStorageType = PVector<ThetaStorageType, 4>;
using ThetaPackStorageType = std::variant<ThetaProductStorageType, LiraParam>;

inline ThetaStorageType theta_to_key(const Theta& t) {
  return std::visit(overloaded{
    [](const Delta& d) {
      return ThetaStorageType(d);
    },
    [](const ThetaComplement& complement) {
      Compressor compressor;
      complement.ratio().compress(compressor);
      return ThetaStorageType(std::move(compressor).result());
    },
  }, t);
}

inline Theta key_to_theta(ThetaStorageType key) {
  return std::visit(overloaded{
    [](const Delta& d) {
      return Theta(d);
    },
    [](const CompressedBlob& complement_compressed) {
      Decompressor decompressor(complement_compressed);
      Theta ret = ThetaComplement(CompoundRatio::from_compressed(decompressor));
      CHECK(decompressor.done());
      return ret;
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
      return ThetaPackStorageType(formal_symbol);
    },
  }, pack);
}

inline ThetaPack key_to_theta_pack(const ThetaPackStorageType& key) {
  return std::visit(overloaded{
    [](const ThetaProductStorageType& product) {
      return ThetaPack(mapped(product, key_to_theta));
    },
    [](const LiraParam& formal_symbol) {
      return ThetaPack(formal_symbol);
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
}  // namespace internal
#endif

using ThetaExpr = Linear<internal::ThetaExprParam>;

ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<CompoundRatio>& compound_ratios);

ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<std::vector<CrossRatio>>& cross_ratios);

ThetaExpr delta_expr_to_theta_expr(const DeltaExpr& expr);

DeltaExpr theta_expr_to_delta_expr(const ThetaExpr& expr);

// Whether expr is one w.r.t. shuffle multiplication.
inline bool theta_pack_is_unity(const ThetaPack& pack) {
  const auto* as_product = std::get_if<std::vector<Theta>>(&pack);
  return as_product && as_product->empty();
}

inline ThetaPack ThetaUnityElement() {
  return std::vector<Theta>{};
}

inline ThetaExpr TUnity() {
  return ThetaExpr::single(ThetaUnityElement());
}

inline ThetaExpr TRatio(const CompoundRatio& ratio) {
  ThetaExpr ret;
  for (const std::vector<int>& l : ratio.loops()) {
    ret += delta_expr_to_theta_expr(cross_ratio(l));
  }
  return ret;
}

inline ThetaExpr TRatio(const CrossRatio& ratio) {
  return TRatio(CompoundRatio::from_cross_ratio(ratio));
}

inline ThetaExpr TRatio(std::initializer_list<int> indices) {
  return TRatio(CrossRatio(indices));
}

inline ThetaExpr TComplement(const CompoundRatio& ratio) {
  auto one_minus_ratio = CompoundRatio::one_minus(ratio);
  return one_minus_ratio.has_value()
    ? TRatio(std::move(one_minus_ratio.value()))
    : ThetaExpr::single(std::vector<Theta>{ThetaComplement(std::move(ratio))});
}

inline ThetaExpr TComplement(std::initializer_list<std::initializer_list<int>> indices) {
  std::vector<CrossRatio> ratios;
  for (auto&& r : indices) {
    ratios.push_back(CrossRatio(r));
  }
  return TComplement(CompoundRatio::from_cross_ratio_product(ratios));
}

inline ThetaExpr TFormalSymbol(const LiraParam& lira_param) {
  return ThetaExpr::single(lira_param);
}


// Optimization potential: make sure sorting always happens in key space.
inline bool operator<(const Theta& lhs, const Theta& rhs) {
  return internal::theta_to_key(lhs) < internal::theta_to_key(rhs);
}

ThetaExpr theta_expr_keep_monsters(const ThetaExpr& expr);

ThetaExpr update_foreweight(
    const ThetaExpr& expr,
    int new_foreweight);

StringExpr count_functions(const ThetaExpr& expr);
