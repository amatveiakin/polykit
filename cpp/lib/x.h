#pragma once

#include "absl/types/span.h"

#include "check.h"
#include "format_basic.h"


class X {
public:
  static constexpr int kMaxVariableIndex = 15;  // max 4-bit unsigned int value
  static constexpr int kInf = 1000;
  static_assert(kInf > kMaxVariableIndex);

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


// TODO: Pass this by const reference everywhere.
class XArgs {
public:
  XArgs(absl::Span<const X> points) : data_(to_vector(points)) {}
  XArgs(absl::Span<const int> points) : data_(mapped(points, X::Var)) {}
  XArgs(std::vector<X> points) : data_(std::move(points)) {}
  XArgs(const std::vector<int>& points) : XArgs(absl::MakeConstSpan(points)) {}
  template<size_t N>
  XArgs(const std::array<X, N>& points) : XArgs(absl::MakeConstSpan(points)) {}
  template<size_t N>
  XArgs(const std::array<int, N>& points) : XArgs(absl::MakeConstSpan(points)) {}
  XArgs(std::initializer_list<int> points) : XArgs(absl::Span<const int>(points)) {}

  const std::vector<X>& as_x() const { return data_; }

  // Will crash if the list contains infinities.
  std::vector<int> as_int() const {
    return mapped(data_, [](X x) { return x.var(); });
  }

  std::vector<std::string> as_string() const {
    return mapped_to_string(data_);
  }

private:
  std::vector<X> data_;
};
