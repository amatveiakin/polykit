// DeltaExpr is a linear expression where each term is a tensor product of residuals of
// two variable forms. See `x.h` for the list of supported variable forms.
//
// Example:
//   -2 (x1 - x2)*(x1 - x2)*(x1 - x2)
//    + (x1 - x2)*(x2 - x3)*(x1 - x4)
//
// The expression is always normalized so that in each residual the variable with smaller
// index goes first. The order of variables does not affect the sign: (x2 - x1) == (x1 - x2).
// If any factor contains two equal variable or contains at least one variable that is
// equal to infinity, the entire term is discarded.

#pragma once

#include <algorithm>
#include <limits>

#include "absl/strings/str_cat.h"

#include "check.h"
#include "coalgebra.h"
#include "format.h"
#include "linear.h"
#include "pvector.h"
#include "util.h"
#include "x.h"


// TODO: Implement integratability criterion

// Represents (x_i - x_j).
class Delta {
public:
  Delta() {}
  Delta(X a, X b) : a_(a), b_(b) {
    simplify();
  }

  X a() const { return a_; }
  X b() const { return b_; }

  bool is_nil() const { return a_ == b_; }

  bool operator==(const Delta& other) const { return as_pair() == other.as_pair(); }
  bool operator!=(const Delta& other) const { return as_pair() != other.as_pair(); }
  bool operator< (const Delta& other) const { return as_pair() <  other.as_pair(); }
  bool operator<=(const Delta& other) const { return as_pair() <= other.as_pair(); }
  bool operator> (const Delta& other) const { return as_pair() >  other.as_pair(); }
  bool operator>=(const Delta& other) const { return as_pair() >= other.as_pair(); }

  // Put larger point first to synchronize ordering with DeltaAlphabetMapping.
  std::pair<X, X> as_pair() const { return {b_, a_}; }

  template <typename H>
  friend H AbslHashValue(H h, const Delta& delta) {
    return H::combine(std::move(h), delta.as_pair());
  }

private:
  static X negate_x(X x);  // similar to `-x`, but supports all forms
  void simplify();

  X a_;
  X b_;
};

std::string dump_to_string_impl(const Delta& d);
std::string to_string(const Delta& d);

inline X Delta::negate_x(X x) {
  SWITCH_ENUM_OR_DIE(x.form(), {
    case XForm::var:
    case XForm::neg_var:
      return -x;
    case XForm::sq_var:
      FATAL("Cannot negate sq_var");
    case XForm::zero:
    case XForm::infinity:
    case XForm::undefined:
      return x;
  });
}

inline void Delta::simplify() {
  sort_two(a_, b_);
  if (a_ == Inf || b_ == Inf) {
    a_ = Inf;
    b_ = Inf;
  } else if (a_.is(XForm::sq_var) || b_.is(XForm::sq_var)) {
    // Will be simplified later
    CHECK_EQ(a_.form(), XForm::sq_var);
    CHECK_EQ(b_.form(), XForm::sq_var);
  } else {
    if (a_.is(XForm::neg_var)) {
      a_ = negate_x(a_);
      b_ = negate_x(b_);
    }
    if (b_.is(XForm::neg_var)) {
      // TODO: Consider storing deltas as sums (not diffs) to avoid things like these.
      CHECK_EQ(a_.form(), XForm::var);
      if (b_.idx() < a_.idx()) {
        const X new_a = X(XForm::var, b_.idx());
        const X new_b = X(XForm::neg_var, a_.idx());
        a_ = new_a;
        b_ = new_b;
      }
    }
    if (a_.is_constant()) {
      CHECK_EQ(a_, Zero);
      CHECK_EQ(b_, Zero);
    }
    if (a_ == b_) {
      a_ = Inf;
      b_ = Inf;
    } else if (negate_x(a_) == b_) {
      // "q - (-q)" = "2*q" => "2" + "q" => "q"
      // throw away terms with constant "2" since they are uninteresting (?)
      b_ = Zero;
    }
  }
  CHECK_LE(a_, b_);
}

namespace internal {
using DeltaDiffT = unsigned char;
}  // namespace internal


class DeltaAlphabetMapping {
public:
  static constexpr int kMaxDimension = 23;  // allowed characters are [0, kMaxDimension)

  DeltaAlphabetMapping();

  int to_alphabet(const Delta& d) const {
    const int za = x_to_alphabet(d.a());
    const int zb = x_to_alphabet(d.b());
    CHECK_LE(za, zb);
    return zb*(zb-1)/2 + za;
  }

  Delta from_alphabet(int ch) const {
    return deltas_.at(ch);
  }

private:
  static constexpr int kZeroCode = kMaxDimension - 1;
// Flip to support more vars at the expense of losing XForm::neg_var.
// TODO: Turn this into a proper compilation flag or find another way to support more vars.
#if 1
  static constexpr int kNegVarCodeStart = (kZeroCode + 1) / 2;
#else
  static constexpr int kNegVarCodeStart = kZeroCode;
#endif
  static constexpr int kNegVarCodeEnd = kZeroCode;
  static constexpr int kMaxNegVars = kNegVarCodeEnd - kNegVarCodeStart;
  static constexpr int kVarCodeStart = 0;
  static constexpr int kVarCodeEnd = kNegVarCodeStart;
  static constexpr int kMaxVars = kVarCodeEnd - kVarCodeStart;

  static X alphabet_to_x(int ch);
  static int x_to_alphabet(X x) {
    const int idx = x.idx() - 1;
    switch (x.form()) {
      case XForm::var:
        CHECK_LE(0, idx);
        CHECK_LT(idx, kMaxVars);
        return kVarCodeStart + idx;
      case XForm::neg_var:
        CHECK_LE(0, idx);
        CHECK_LT(idx, kMaxNegVars);
        return kNegVarCodeStart + idx;
      case XForm::zero:
        return kZeroCode;
      default:
        break;
    }
    FATAL_BAD_ENUM(x.form());
  }

  std::vector<Delta> deltas_;
};

// Idea: replace with PArray<uint4_t, 2>
extern DeltaAlphabetMapping delta_alphabet_mapping;

namespace internal {
struct DeltaExprParam {
  using ObjectT = std::vector<Delta>;
#if DISABLE_PACKING
  IDENTITY_STORAGE_FORM
#else
  using StorageT = PVector<DeltaDiffT, 10>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, [](const Delta& d) -> DeltaDiffT {
      return delta_alphabet_mapping.to_alphabet(d);
    });
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, [](int ch) {
      return delta_alphabet_mapping.from_alphabet(ch);
    });
  }
#endif
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
  static int object_to_weight(const ObjectT& obj) {
    return obj.size();
  }
};

struct DeltaCoExprParam {
  using PartExprParam = DeltaExprParam;
  using ObjectT = std::vector<std::vector<Delta>>;
#if DISABLE_PACKING
  IDENTITY_STORAGE_FORM
#else
  using PartStorageT = DeltaExprParam::StorageT;
  using StorageT = PVector<PartStorageT, 2>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to_pvector<StorageT>(obj, DeltaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, DeltaExprParam::key_to_object);
  }
#endif
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), DeltaExprParam::object_to_string);
  }
  static int object_to_weight(const ObjectT& obj) {
    return sum(mapped(obj, [](const auto& part) { return part.size(); }));
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct DeltaNCoExprParam : DeltaCoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), DeltaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};
}  // namespace internal


using DeltaExpr = Linear<internal::DeltaExprParam>;
using DeltaCoExpr = Linear<internal::DeltaCoExprParam>;
using DeltaNCoExpr = Linear<internal::DeltaNCoExprParam>;
template<> struct CoExprForExpr<DeltaExpr> { using type = DeltaCoExpr; };
template<> struct NCoExprForExpr<DeltaExpr> { using type = DeltaNCoExpr; };

inline DeltaExpr delta_to_expr(Delta d) {
  if (d.is_nil()) {
    return DeltaExpr();
  } else if (d.a().is(XForm::sq_var)) {
    CHECK_EQ(d.b().form(), XForm::sq_var);
    const auto a_idx = d.a().idx();
    const auto b_idx = d.b().idx();
    return (
      + DeltaExpr::single({Delta(X(XForm::var, a_idx), X(XForm::var, b_idx))})
      + DeltaExpr::single({Delta(X(XForm::var, a_idx), X(XForm::neg_var, b_idx))})
    );
  } else {
    return DeltaExpr::single({d});
  }
}

inline DeltaExpr D(X a, X b) {
  return delta_to_expr(Delta(a, b));
}


DeltaExpr substitute_variables(const DeltaExpr& expr, const XArgs& new_points);

// Expects: points.size() == 6
// Eliminates terms (x5-x6), (x4-x6), (x2-x6) using involution x1<->x4, x2<->x5, x3<->x6.
DeltaExpr involute(const DeltaExpr& expr, const std::vector<int>& points);

DeltaExpr sort_term_multiples(const DeltaExpr& expr);
DeltaExpr terms_with_unique_muptiples(const DeltaExpr& expr);
DeltaExpr terms_with_nonunique_muptiples(const DeltaExpr& expr);

DeltaExpr terms_with_num_distinct_variables(const DeltaExpr& expr, int num_distinct);
DeltaExpr terms_with_min_distinct_variables(const DeltaExpr& expr, int min_distinct);
DeltaExpr terms_containing_only_variables(const DeltaExpr& expr, const std::vector<int>& indices);
DeltaExpr terms_without_variables(const DeltaExpr& expr, const std::vector<int>& indices);

bool are_weakly_separated(const Delta& d1, const Delta& d2);
bool is_weakly_separated(const DeltaExpr::ObjectT& term);
bool is_weakly_separated(const DeltaNCoExpr::ObjectT& term);
bool is_totally_weakly_separated(const DeltaExpr& expr);
bool is_totally_weakly_separated(const DeltaNCoExpr& expr);
DeltaExpr keep_non_weakly_separated(const DeltaExpr& expr);
DeltaNCoExpr keep_non_weakly_separated(const DeltaNCoExpr& expr);

// TODO: Allow circular normalization when the number of points is odd.
bool passes_normalize_remove_consecutive(const DeltaExpr::ObjectT& term);
DeltaExpr normalize_remove_consecutive(const DeltaExpr& expr);

DeltaExpr terms_with_connected_variable_graph(const DeltaExpr& expr);

// For using together with `DeltaExpr::filter`
int count_var(const DeltaExpr::ObjectT& term, int var);

void print_sorted_by_num_distinct_variables(std::ostream& os, const DeltaExpr& expr);
