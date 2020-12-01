#pragma once

#include "check.h"
#include "format.h"


class X {
public:
  static constexpr int kMaxVariableIndex = 15;  // max 4-bit unsigned int value
  static constexpr int kInf = 1000;

  constexpr X() {}
  constexpr X(int idx) : idx_(idx) {
    CHECK_LT(0, idx_);
    CHECK_LE(idx_, kMaxVariableIndex);
  }
  static constexpr X Var(int idx) {
    return X(idx);
  }
  static constexpr X Inf() {
    X x;
    x.idx_ = kInf;
    return x;
  }

  int var() const {
    CHECK_LE(idx_, kMaxVariableIndex);
    return idx_;
  }

  bool operator==(const X& other) const { return idx_ == other.idx_; }
  bool operator!=(const X& other) const { return idx_ != other.idx_; }
  bool operator< (const X& other) const { return idx_ <  other.idx_; }
  bool operator<=(const X& other) const { return idx_ <= other.idx_; }
  bool operator> (const X& other) const { return idx_ >  other.idx_; }
  bool operator>=(const X& other) const { return idx_ >= other.idx_; }

private:
  int idx_ = 0;
};

static constexpr X Inf = X::Inf();

inline std::string to_string(const X& x) {
  return (x == Inf) ? fmt::inf() : to_string(x.var());
}
