#pragma once

#include <algorithm>

#include "absl/strings/str_cat.h"

#include "check.h"
#include "format.h"
#include "linear.h"
#include "util.h"
#include "word.h"
#include "x.h"


// TODO: Implement integratability criterion

// Represents (x_i - x_j).
class Delta {
public:
  Delta() {}
  Delta(X a, X b) {
    if (a == Inf || b == Inf) {
      a_ = 0;
      b_ = 0;
    } else {
      a_ = a.var();
      b_ = b.var();
      CHECK_GE(a_, 1);
      CHECK_GE(b_, 1);
      if (a_ > b_) {
        std::swap(a_, b_);
      }
    }
  }

  int a() const { return a_; }
  int b() const { return b_; }

  bool is_nil() const { return a_ == b_; }

  bool operator==(const Delta& other) const { return as_pair() == other.as_pair(); }
  bool operator!=(const Delta& other) const { return as_pair() != other.as_pair(); }
  bool operator< (const Delta& other) const { return as_pair() <  other.as_pair(); }

  std::pair<int, int> as_pair() const { return {a_, b_}; }

private:
  int a_ = 0;
  int b_ = 0;
};

inline std::string to_string(const Delta& d) {
  // return absl::StrCat("(", fmt::var(d.a()), " - ", fmt::var(d.b()), ")");
  return absl::StrCat("[", d.a(), ",", d.b(), "]");
}


class DeltaAlphabetMapping {
public:
  static constexpr int kMaxDimension = 20;

  DeltaAlphabetMapping() {
    static constexpr int kAlphabetSize = kMaxDimension * (kMaxDimension - 1) / 2;
    static_assert(kAlphabetSize <= kWordAlphabetSize);
    deltas_.resize(kAlphabetSize);
    for (int b = 1; b <= kMaxDimension; ++b) {
      for (int a = 1; a < b; ++a) {
        Delta d(a, b);
        deltas_.at(to_alphabet(d)) = d;
      }
    }
  }

  int to_alphabet(const Delta& d) const {
    CHECK(!d.is_nil());
    CHECK_LE(d.b(), kMaxDimension);
    const int za = d.a() - 1;
    const int zb = d.b() - 1;
    return zb*(zb-1)/2 + za;
  }

  Delta from_alphabet(int ch) const {
    return deltas_.at(ch);
  }

private:
  std::vector<Delta> deltas_;
};

// TODO: Consider replacing with 4 bit + 4 bit mapping
extern DeltaAlphabetMapping delta_alphabet_mapping;


namespace internal {
struct DeltaExprParam {
  using ObjectT = std::vector<Delta>;
  using StorageT = Word;
  static StorageT object_to_key(const ObjectT& obj) {
    Word ret;
    for (const Delta& d : obj) {
      ret.push_back(delta_alphabet_mapping.to_alphabet(d));
    }
    return ret;
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key.span(), [](int ch){
      return delta_alphabet_mapping.from_alphabet(ch);
    });
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat_words(lhs, rhs);
  }
  static int object_to_weight(const ObjectT& obj) {
    return obj.size();
  }
  static StorageT shuffle_preprocess(const StorageT& key) {
    return key;
  }
  static StorageT shuffle_postprocess(const StorageT& key) {
    return key;
  }
};
}  // namespace internal

using DeltaExpr = Linear<internal::DeltaExprParam>;

inline DeltaExpr D(X a, X b) {
  Delta d(a, b);
  return d.is_nil() ? DeltaExpr() : DeltaExpr::single({d});
}


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<X>& new_points);
