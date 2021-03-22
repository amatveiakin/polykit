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


class SpanX {
public:
  SpanX(absl::Span<const X> points) : span_(points) {}
  SpanX(absl::Span<const int> points) : span_(points) {}
  SpanX(const std::vector<X>& points) : SpanX(absl::MakeConstSpan(points)) {}
  SpanX(const std::vector<int>& points) : SpanX(absl::MakeConstSpan(points)) {}
  SpanX(std::initializer_list<int> points) : SpanX(absl::Span<const int>(points)) {}

  std::vector<X> as_x() const {
    return std::visit(overloaded{
      [](const absl::Span<const int> s) {
        return mapped(s, [](int p) { return X(p); });
      },
      [](const absl::Span<const X> s) {
        return std::vector(s.begin(), s.end());
      },
    }, span_);
  }

  // Will crash if the list contains infinities.
  std::vector<int> as_int() const {
    return std::visit(overloaded{
      [](const absl::Span<const int> s) {
        return std::vector(s.begin(), s.end());
      },
      [](const absl::Span<const X> s) {
        return mapped(s, [](X x) { return x.var(); });
      },
    }, span_);
  }

  std::vector<std::string> as_string() const {
    return std::visit([](const auto& s) {
      return mapped_to_string(s);
    }, span_);
  }

private:
  std::variant<absl::Span<const int>, absl::Span<const X>> span_;
};
