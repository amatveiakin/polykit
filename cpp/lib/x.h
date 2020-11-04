#pragma once

#include "check.h"


class X {
public:
  // TODO: Reduce kMaxVariableIndex to 15 (it seems that sometimes
  // compressed pair is written here, but it shouldn't happen).
  static constexpr int kMaxVariableIndex = 255;
  static constexpr int kInf = 1000;

  constexpr X() {}
  constexpr X(int idx) : idx_(idx) {  // TODO: explicit
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

  constexpr int var() const {
    CHECK_LE(idx_, kMaxVariableIndex);
    return idx_;
  }

  bool operator==(const X& other) const { return idx_ == other.idx_; }
  bool operator< (const X& other) const { return idx_ <  other.idx_; }

private:
  int idx_ = 0;
};

static constexpr X Inf = X::Inf();

inline std::string to_string(const X& x) {
  return (x == Inf) ? "Inf" : to_string(x.var());
}
