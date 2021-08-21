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
  static constexpr int kFakeIndex = -1000;

  constexpr X(XForm form, int idx);
  constexpr X() : X(XForm::undefined, kFakeIndex) {}
  constexpr X(int idx) : X(XForm::var, idx) {}
  static constexpr X Var(int idx) { return X(idx); }
  static constexpr X Zero() { return X(XForm::zero, kFakeIndex); }
  static constexpr X Inf() { return X(XForm::infinity, kFakeIndex); }

  constexpr XForm form() const { return form_; }
  // It is guaranteed that `idx` for any constant if different from any valid variable index.
  // Thus `x.idx() == my_var_idx` is equivalent to `!x.is_constant() && x.idx() == my_var_idx`.
  constexpr int idx() const { return idx_; }
  constexpr bool is(XForm form) const { return form_ == form; }
  constexpr bool is_constant() const;

  X negated() const;
  X operator-() const;  // similar to `negated` but deliberately limited to variables
  X operator+() const;

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
      CHECK_LT(0, idx_) << "where form = " << to_string(form_);
      break;
    case XForm::zero:
    case XForm::infinity:
    case XForm::undefined:
      idx_ = kFakeIndex;
      // CHECK_EQ(kFakeIndex, idx_) << "where form = " << to_string(form_);
      break;
  }
}

inline constexpr bool X::is_constant() const {
  switch (form_) {
    case XForm::var:
    case XForm::neg_var:
    case XForm::sq_var:
      return false;
    case XForm::zero:
    case XForm::infinity:
      return true;
    case XForm::undefined:
      break;
  }
  FATAL(absl::StrCat("Unknown form ", to_string(form_)));
}

inline X X::negated() const {
  switch (form_) {
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
  }
  FATAL(absl::StrCat("Negation not supported for ", to_string(form_)));
}

inline X X::operator-() const {
  CHECK(!is_constant()) << "operator- is not supported for " << to_string(form_);
  return negated();
}

inline X X::operator+() const {
  CHECK(!is_constant()) << "operator+ is not supported for " << to_string(form_);
  return *this;
}

std::string to_string(X x);

inline static const X Zero = X::Zero();
inline static const X Inf = X::Inf();

inline static const X x1 = X(1);
inline static const X x2 = X(2);
inline static const X x3 = X(3);
inline static const X x4 = X(4);
inline static const X x5 = X(5);
inline static const X x6 = X(6);
inline static const X x7 = X(7);
inline static const X x8 = X(8);

inline static const X x1s = X(XForm::sq_var, 1);
inline static const X x2s = X(XForm::sq_var, 2);
inline static const X x3s = X(XForm::sq_var, 3);
inline static const X x4s = X(XForm::sq_var, 4);
inline static const X x5s = X(XForm::sq_var, 5);
inline static const X x6s = X(XForm::sq_var, 6);
inline static const X x7s = X(XForm::sq_var, 7);
inline static const X x8s = X(XForm::sq_var, 8);


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
  XArgs(std::initializer_list<X> points) : XArgs(absl::Span<const X>(points)) {}

  size_t size() const { return data_.size(); }

  // TODO: Rename to `get`
  const std::vector<X>& as_x() const { return data_; }

private:
  std::vector<X> data_;
};
