#pragma once

#include <algorithm>

#include "check.h"
#include "format.h"
#include "linear.h"
#include "util.h"
#include "word.h"


class Delta {
public:
  Delta() {}
  Delta(int a, int b) : a_(a), b_(b) {
    CHECK_GE(a_, 1);
    CHECK_GE(b_, 1);
    if (a_ > b_) {
      std::swap(a_, b_);
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
  return "(x" + to_string(d.a()) + " - x" + to_string(d.b()) + ")";
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

extern DeltaAlphabetMapping delta_alphabet_mapping;


struct LinearParamDeltaExpr {
  using ObjectT = std::vector<Delta>;
  using StorageT = IntWord;
  static StorageT object_to_key(const ObjectT& obj) {
    std::vector<int> as_ints(obj.size());
    std::transform(obj.begin(), obj.end(), as_ints.begin(), [](Delta d){
      return delta_alphabet_mapping.to_alphabet(d);
    });
    return IntWord(as_ints.begin(), as_ints.end());
  }
  static ObjectT key_to_object(const StorageT& key) {
    ObjectT ret(key.size());
    std::transform(key.begin(), key.end(), ret.begin(), [](int ch){
      return delta_alphabet_mapping.from_alphabet(ch);
    });
    return ret;
  }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, "*");
  }
};

using DeltaExpr = Linear<LinearParamDeltaExpr>;

inline DeltaExpr D(int a, int b) {
  Delta d(a, b);
  return d.is_nil() ? DeltaExpr() : DeltaExpr::single({d});
}


DeltaExpr delta_expr_substitute(
    const DeltaExpr& expr,
    const std::vector<int>& new_points);
