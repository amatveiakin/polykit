// Every instance of `X` stores a constant or a simple single-variable expression.

#pragma once

#include "absl/types/span.h"

#include "check.h"
#include "format_basic.h"
#include "poly_limits.h"


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

struct XNoCheck {};


class X {
public:
  static constexpr int kMinIndex = kZeroBasedVariables ? 0 : 1;
  static constexpr int kFakeIndex = -1000;

  // TODO(C++20): Merge `XConstant` and regular constructor. Choose between
  //   `CHECK`s and `static_assert`s based on `std::is_constant_evaluated`.
  //   Make all other constructors `constexpr`.
  constexpr X(XForm form, int idx, XNoCheck) : form_(form), idx_(idx) {};
  X(XForm form, int idx);
  X() : X(XForm::undefined, kFakeIndex) {}
  X(int idx) : X(XForm::var, idx) {}
  static X Zero() { return X(XForm::zero, kFakeIndex); }
  static X Inf() { return X(XForm::infinity, kFakeIndex); }
  static X Undefined() { return X(); }

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

struct XIllegal {};

template<XForm form, int idx, typename Enable = void>
struct XConstexpr {
  static constexpr XIllegal value;
};
template<XForm form, int idx>
struct XConstexpr<form, idx,
  std::enable_if_t<
    ((form == XForm::var || form == XForm::neg_var || form == XForm::sq_var) && (idx >= X::kMinIndex)) ||
    ((form == XForm::zero || form == XForm::infinity || form == XForm::undefined) && (idx == X::kFakeIndex))
  >
> {
  static constexpr X value = X(form, idx, XNoCheck());
};

template<XForm form, int idx>
constexpr auto XConstexprV = XConstexpr<form, idx>::value;

inline X::X(XForm form, int idx) : form_(form), idx_(idx) {
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

inline constexpr auto Zero = XConstexprV<XForm::zero, X::kFakeIndex>;
inline constexpr auto Inf = XConstexprV<XForm::infinity, X::kFakeIndex>;

inline constexpr auto x0 = XConstexprV<XForm::var, 0>;
inline constexpr auto x1 = XConstexprV<XForm::var, 1>;
inline constexpr auto x2 = XConstexprV<XForm::var, 2>;
inline constexpr auto x3 = XConstexprV<XForm::var, 3>;
inline constexpr auto x4 = XConstexprV<XForm::var, 4>;
inline constexpr auto x5 = XConstexprV<XForm::var, 5>;
inline constexpr auto x6 = XConstexprV<XForm::var, 6>;
inline constexpr auto x7 = XConstexprV<XForm::var, 7>;
inline constexpr auto x8 = XConstexprV<XForm::var, 8>;
inline constexpr auto x9 = XConstexprV<XForm::var, 9>;
inline constexpr auto x10 = XConstexprV<XForm::var, 10>;
inline constexpr auto x11 = XConstexprV<XForm::var, 11>;
inline constexpr auto x12 = XConstexprV<XForm::var, 12>;
inline constexpr auto x13 = XConstexprV<XForm::var, 13>;
inline constexpr auto x14 = XConstexprV<XForm::var, 14>;
inline constexpr auto x15 = XConstexprV<XForm::var, 15>;
inline constexpr auto x16 = XConstexprV<XForm::var, 16>;

inline constexpr auto x0s = XConstexprV<XForm::sq_var, 0>;
inline constexpr auto x1s = XConstexprV<XForm::sq_var, 1>;
inline constexpr auto x2s = XConstexprV<XForm::sq_var, 2>;
inline constexpr auto x3s = XConstexprV<XForm::sq_var, 3>;
inline constexpr auto x4s = XConstexprV<XForm::sq_var, 4>;
inline constexpr auto x5s = XConstexprV<XForm::sq_var, 5>;
inline constexpr auto x6s = XConstexprV<XForm::sq_var, 6>;
inline constexpr auto x7s = XConstexprV<XForm::sq_var, 7>;
inline constexpr auto x8s = XConstexprV<XForm::sq_var, 8>;
inline constexpr auto x9s = XConstexprV<XForm::sq_var, 9>;
inline constexpr auto x10s = XConstexprV<XForm::sq_var, 10>;
inline constexpr auto x11s = XConstexprV<XForm::sq_var, 11>;
inline constexpr auto x12s = XConstexprV<XForm::sq_var, 12>;
inline constexpr auto x13s = XConstexprV<XForm::sq_var, 13>;
inline constexpr auto x14s = XConstexprV<XForm::sq_var, 14>;
inline constexpr auto x15s = XConstexprV<XForm::sq_var, 15>;
inline constexpr auto x16s = XConstexprV<XForm::sq_var, 16>;


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
