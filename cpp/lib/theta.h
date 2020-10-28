#pragma once

#include <array>
#include <variant>
#include <vector>

#include "absl/algorithm/container.h"

#include "check.h"
#include "epsilon.h"
#include "format.h"
#include "multiword.h"
#include "polylog_cross_ratio_param.h"
#include "word.h"


using ThetaStorageType = Word;

// Represents (1 - [a1,b1,c1,d1] * ... * [an,bn,cn,dn]),  n >= 2
class ThetaComplement {
public:
  ThetaComplement() {}
  ThetaComplement(std::vector<CrossRatio> ratios) : ratios_(std::move(ratios)) {
    absl::c_sort(ratios_);
    CHECK(is_valid()) << list_to_string(ratios_);
  }

  bool is_valid() const {
    if (ratios_.size() < 2) {
      return false;  // A single cross ratio complement must be simplified
    }
    return absl::c_all_of(ratios_, [](const CrossRatio& ratio) {
      return ratio.is_valid();
    });
  }
  const std::vector<CrossRatio>& ratios() const { return ratios_; }

  bool operator==(const ThetaComplement& other) const {
    return ratios_ == other.ratios_;
  }

private:
  std::vector<CrossRatio> ratios_;
};

// Represents one of the two:
//   * [a,b,c,d]
//   * 1 - [a1,b1,c1,d1] * ... * [an,bn,cn,dn],  n >= 2
using Theta = std::variant<CrossRatio, ThetaComplement>;

using ThetaPack = std::variant<std::vector<Theta>, LiraParam>;

enum ThetaPackType {
  kThetaPackTypeProduct = 0,
  kThetaPackTypeFormalSymbol = 1,
};


inline std::string to_string(const ThetaComplement& complement) {
  return absl::StrCat("(1 - ", str_join(complement.ratios(), ""), ")");
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


namespace internal {
// Idea: Store just the cross-ratios. We don't need a type bit, because
// ThetaComplement can never contain just one cross ratio.
inline ThetaStorageType theta_to_key(const Theta& t) {
  return std::visit(overloaded{
    [](const CrossRatio& ratio) {
      return Word(ratio.compressed());
    },
    [](const ThetaComplement& complement) {
      CHECK_GT(complement.ratios().size(), 1);
      Word ret;
      for (const CrossRatio& ratio : complement.ratios()) {
        ret.append_data(ratio.compressed());
      }
      return ret;
    },
  }, t);
}

inline Theta key_to_theta(ThetaStorageType key) {
  CHECK(key.size() % kCrossRatioElementsCompressed == 0) << to_string(key);
  const int num_ratios = key.size() / kCrossRatioElementsCompressed;
  if (num_ratios == 1) {
    return CrossRatio::from_compressed(key.span());
  } else if (num_ratios > 1) {
    std::vector<CrossRatio> ratios;
    for (int i = 0; i < num_ratios; ++i) {
      ratios.push_back(CrossRatio::from_compressed(
        key.span().subspan(i * kCrossRatioElementsCompressed, kCrossRatioElementsCompressed)));
    }
    return ThetaComplement(std::move(ratios));
  } else {
    FAIL(absl::StrCat("Bad num_ratios: ", num_ratios));
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
  FAIL(absl::StrCat("Bad ThetaPack type = ", type, "; key = ", to_string(key)));
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
  // static StorageT shuffle_preprocess(const StorageT& key) {
  //   const int type = key_to_theta_pack_type(key);
  //   CHECK_EQ(type, kThetaPackTypeProduct) << "Lyndon for formal symbols is not defined";
  //   return Word(key_to_theta_pack_data(key));
  // }
  // static StorageT shuffle_postprocess(const StorageT& key) {
  //   Word ret;
  //   ret.push_back(kThetaPackTypeProduct);
  //   ret.append_word(key);
  //   return ret;
  // }
};
}  // namespace internal

using ThetaExpr = Linear<internal::ThetaExprParam>;

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

inline ThetaExpr TRatio(const CrossRatio& ratio) {
  return ThetaExpr::single(std::vector<Theta>{ratio});
}

inline ThetaExpr TRatio(std::initializer_list<int> indices) {
  return TRatio(CrossRatio(indices));
}

inline ThetaExpr TComplement(std::vector<CrossRatio> ratios) {
  CHECK(!ratios.empty());
  return ratios.size() == 1
    ? TRatio(CrossRatio::one_minus(ratios.front()))
    : ThetaExpr::single(std::vector<Theta>{ThetaComplement(std::move(ratios))});
}

inline ThetaExpr TComplement(std::initializer_list<std::initializer_list<int>> indices) {
  std::vector<CrossRatio> ratios;
  for (auto&& r : indices) {
    ratios.push_back(CrossRatio(r));
  }
  return TComplement(std::move(ratios));
}

inline ThetaExpr TFormalSymbolPositive(const LiraParam& lira_param) {
  return ThetaExpr::single(lira_param);
}

inline ThetaExpr TFormalSymbolSigned(const LiraParam& lira_param) {
  return lira_param.sign() * TFormalSymbolPositive(lira_param);
}


// Optimization potential: make sure sorting always happens in key space.
inline bool operator<(const Theta& lhs, const Theta& rhs) {
  return internal::theta_to_key(lhs) < internal::theta_to_key(rhs);
}

ThetaExpr epsilon_expr_to_theta_expr(
    const EpsilonExpr& expr,
    const std::vector<std::vector<CrossRatio>>& ratios);
