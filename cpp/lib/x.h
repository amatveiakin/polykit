// Every instance of `X` stores a constant or a simple single-variable expression.

#pragma once

#include "absl/types/span.h"

#include "check.h"
#include "format_basic.h"


// All constants go after all variables. Order is likely to be stable.
enum class XForm {
  var,      // +x_i
  neg_var,  // -x_i
  sq_var,   // x_i^2
  zero,
  infinity,
  undefined,
};

std::string to_string(XForm form);


class X {
public:
  static constexpr int kMinIndex = 0;
  static constexpr int kFakeIndex = -1000;

  constexpr X(XForm form, int idx);
  constexpr X() : X(XForm::undefined, kFakeIndex) {}
  constexpr X(int idx) : X(XForm::var, idx) {}
  static constexpr X Zero() { return X(XForm::zero, kFakeIndex); }
  static constexpr X Inf() { return X(XForm::infinity, kFakeIndex); }
  static constexpr X Undefined() { return X(); }

  constexpr XForm form() const { return form_; }
  // It is guaranteed that `idx` for any constant if different from any valid variable index.
  // Thus `x.idx() == my_var_idx` is equivalent to `!x.is_constant() && x.idx() == my_var_idx`.
  constexpr int idx() const { return idx_; }
  int as_simple_var() const { CHECK(is(XForm::var)); return idx_; }
  constexpr int var_sign() const;  // var => +1, neg_var => -1, .. => FATAL
  constexpr bool is(XForm form) const { return form_ == form; }
  constexpr bool is_constant() const;

  X negated() const;
  X operator-() const;  // similar to `negated` but deliberately limited to variables
  X operator+() const;

  X substitution_result_0_based(const std::vector<X>& new_points);

  constexpr bool operator==(const X& other) const { return as_pair() == other.as_pair(); }
  constexpr bool operator!=(const X& other) const { return as_pair() != other.as_pair(); }
  constexpr bool operator< (const X& other) const { return as_pair() <  other.as_pair(); }
  constexpr bool operator<=(const X& other) const { return as_pair() <= other.as_pair(); }
  constexpr bool operator> (const X& other) const { return as_pair() >  other.as_pair(); }
  constexpr bool operator>=(const X& other) const { return as_pair() >= other.as_pair(); }

  template <typename H>
  friend H AbslHashValue(H h, const X& x) {
    return H::combine(std::move(h), x.as_pair());
  }

private:
  std::pair<XForm, int> as_pair() const { return {form_, idx_}; }

  XForm form_ = XForm::undefined;
  int idx_ = 0;
};

inline constexpr X::X(XForm form, int idx) : form_(form), idx_(idx) {
  // TODO(C++20): Replace CHECK-s with static_assert-s under std::is_constant_evaluated;
  //   make predefined `X` constants constexpr.
  switch (form_) {
    case XForm::var:
    case XForm::neg_var:
    case XForm::sq_var:
      CHECK_LE(kMinIndex, idx_) << "where form = " << to_string(form_);
      break;
    case XForm::zero:
    case XForm::infinity:
    case XForm::undefined:
      idx_ = kFakeIndex;
      // CHECK_EQ(kFakeIndex, idx_) << "where form = " << to_string(form_);
      break;
  }
}

std::string to_string(X x);

inline constexpr int X::var_sign() const {
  switch (form_) {
    case XForm::var: return 1;
    case XForm::neg_var: return -1;
    default: FATAL(absl::StrCat("Unexpected form: ", to_string(*this)));
  }
}

inline constexpr bool X::is_constant() const {
  SWITCH_ENUM_OR_DIE(form_, {
    case XForm::var:
    case XForm::neg_var:
    case XForm::sq_var:
      return false;
    case XForm::zero:
    case XForm::infinity:
      return true;
    case XForm::undefined:
      break;
  });
}

inline X X::negated() const {
  SWITCH_ENUM_OR_DIE_WITH_CONTEXT(form_, "negation", {
    case XForm::var:
      return X(XForm::neg_var, idx_);
    case XForm::neg_var:
      return X(XForm::var, idx_);
      return false;
    case XForm::zero:
    case XForm::infinity:
      return *this;
    case XForm::sq_var:
    case XForm::undefined:
      break;
  });
}

inline X X::operator-() const {
  CHECK(!is_constant()) << "operator- is not supported for " << to_string(form_);
  return negated();
}

inline X X::operator+() const {
  CHECK(!is_constant()) << "operator+ is not supported for " << to_string(form_);
  return *this;
}

inline X X::substitution_result_0_based(const std::vector<X>& new_points) {
  SWITCH_ENUM_OR_DIE_WITH_CONTEXT(form_, "variable substitution", {
    case XForm::var:
      return new_points.at(idx_);
    case XForm::neg_var:
      return new_points.at(idx_).negated();
    case XForm::sq_var:
      break;
    case XForm::zero:
    case XForm::infinity:
      return *this;
    case XForm::undefined:
      break;
  });
}

inline static const X Zero = X::Zero();
inline static const X Inf = X::Inf();

inline static const X x0 = X(0);
inline static const X x1 = X(1);
inline static const X x2 = X(2);
inline static const X x3 = X(3);
inline static const X x4 = X(4);
inline static const X x5 = X(5);
inline static const X x6 = X(6);
inline static const X x7 = X(7);
inline static const X x8 = X(8);
inline static const X x9 = X(9);
inline static const X x10 = X(10);
inline static const X x11 = X(11);
inline static const X x12 = X(12);
inline static const X x13 = X(13);
inline static const X x14 = X(14);
inline static const X x15 = X(15);
inline static const X x16 = X(16);

inline static const X x0s = X(XForm::sq_var, 0);
inline static const X x1s = X(XForm::sq_var, 1);
inline static const X x2s = X(XForm::sq_var, 2);
inline static const X x3s = X(XForm::sq_var, 3);
inline static const X x4s = X(XForm::sq_var, 4);
inline static const X x5s = X(XForm::sq_var, 5);
inline static const X x6s = X(XForm::sq_var, 6);
inline static const X x7s = X(XForm::sq_var, 7);
inline static const X x8s = X(XForm::sq_var, 8);
inline static const X x9s = X(XForm::sq_var, 9);
inline static const X x10s = X(XForm::sq_var, 10);
inline static const X x11s = X(XForm::sq_var, 11);
inline static const X x12s = X(XForm::sq_var, 12);
inline static const X x13s = X(XForm::sq_var, 13);
inline static const X x14s = X(XForm::sq_var, 14);
inline static const X x15s = X(XForm::sq_var, 15);
inline static const X x16s = X(XForm::sq_var, 16);


class XArgs {
public:
  XArgs() {}
  XArgs(absl::Span<const X> points) : data_(to_vector(points)) {}
  XArgs(absl::Span<const int> points) : data_(mapped(points, convert_to<X>)) {}
  XArgs(std::vector<X> points) : data_(std::move(points)) {}
  XArgs(const std::vector<int>& points) : XArgs(absl::MakeConstSpan(points)) {}
  template<size_t N>
  XArgs(const std::array<X, N>& points) : XArgs(absl::MakeConstSpan(points)) {}
  template<size_t N>
  XArgs(const std::array<int, N>& points) : XArgs(absl::MakeConstSpan(points)) {}
  XArgs(std::initializer_list<int> points) : XArgs(absl::Span<const int>(points)) {}
  XArgs(std::initializer_list<X> points) : XArgs(absl::Span<const X>(points)) {}

  size_t size() const { return data_.size(); }

  // TODO: Rename to `get`
  const std::vector<X>& as_x() const { return data_; }

private:
  std::vector<X> data_;
};
