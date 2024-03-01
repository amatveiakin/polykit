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
#include "poly_vector.h"
#include "util.h"
#include "x.h"


#define HAS_DELTA_EXPR 1

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
  bool is_var_diff() const { return a_.is(XForm::var) && b_.is(XForm::var); }

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
  CHECK(!a_.is(XForm::undefined) && !b_.is(XForm::undefined));
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

// Use named type to get better output in `PVectorStats`.
struct DeltaStorage {
  using Value = unsigned char;

  Value value;

  bool operator==(const DeltaStorage& other) const { return value == other.value; }
  bool operator< (const DeltaStorage& other) const { return value <  other.value; }

  template <typename H>
  friend H AbslHashValue(H h, const DeltaStorage& d) {
    return H::combine(std::move(h), d.value);
  }
};

inline MemoryUsage estimated_heap_usage(const Delta&) { return {0, 0}; }
inline MemoryUsage estimated_heap_usage(const DeltaStorage&) { return {0, 0}; }


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
  static constexpr int kNegVarCodeStart = kAllowNegativeVariables ? (kZeroCode + 1) / 2 : kZeroCode;
  static constexpr int kNegVarCodeEnd = kZeroCode;
  static constexpr int kMaxNegVars = kNegVarCodeEnd - kNegVarCodeStart;
  static constexpr int kVarCodeStart = 0;
  static constexpr int kVarCodeEnd = kNegVarCodeStart;
  static constexpr int kMaxVars = kVarCodeEnd - kVarCodeStart;

  // TODO: Support as many variables as necessary. Idea: split DeltaExpr into two classes:
  //   - Simple (x_i - x_j) diffs, which could encode more variables in one byte.
  //   - Advanced Delta which also supportes (x_i + x_j) and requires two bytes.
  // Another benefit would be stronger typing, since it only really makes sense to convert
  // simple `DeltaExpr`s to `GammaExpr`s.
  //
  // static_assert(kMaxVars >= kMaxVariables);

  static X alphabet_to_x(int ch);
  static int x_to_alphabet(X x) {
    const int idx = x.idx() - X::kMinIndex;
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
  // Prevent messing up the functions above using `X(int)` implicit constructor.
  static void alphabet_to_x(X ch) = delete;
  static void x_to_alphabet(int x) = delete;

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
  using StorageT = WeightVector<DeltaStorage>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to<StorageT>(obj, [](const Delta& d) {
      return DeltaStorage { static_cast<DeltaStorage::Value>(delta_alphabet_mapping.to_alphabet(d)) };
    });
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, [](DeltaStorage s) {
      return delta_alphabet_mapping.from_alphabet(s.value);
    });
  }
#endif
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_DEFAULT
  DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::monostate element_uniformity_marker(const Delta&) { return {}; }
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::tensor_prod());
  }
  static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs) {
    return concat(lhs, rhs);
  }
};

struct DeltaICoExprParam {
  using PartExprParam = DeltaExprParam;
  using ObjectT = std::vector<std::vector<Delta>>;
#if DISABLE_PACKING
  IDENTITY_STORAGE_FORM
#else
  using PartStorageT = DeltaExprParam::StorageT;
  using StorageT = CopartVector<PartStorageT>;
  static StorageT object_to_key(const ObjectT& obj) {
    return mapped_to<StorageT>(obj, DeltaExprParam::object_to_key);
  }
  static ObjectT key_to_object(const StorageT& key) {
    return mapped(key, DeltaExprParam::key_to_object);
  }
#endif
  IDENTITY_VECTOR_FORM
  LYNDON_COMPARE_LENGTH_FIRST
  CO_DERIVE_WEIGHT_AND_UNIFORMITY_MARKER
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_iterated(), DeltaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_lie_algebra = true;
  static constexpr bool coproduct_is_iterated = true;
};

struct DeltaNCoExprParam : DeltaICoExprParam {
  static std::string object_to_string(const ObjectT& obj) {
    return str_join(obj, fmt::coprod_normal(), DeltaExprParam::object_to_string);
  }
  static constexpr bool coproduct_is_iterated = false;
};

struct DeltaACoExprParam : DeltaICoExprParam {
  static bool lyndon_compare(const VectorT::value_type& lhs, const VectorT::value_type& rhs) {
    using namespace cmp;
    return projected(lhs, rhs, [](const auto& v) {
      return std::tuple{desc_val(v.size()), asc_ref(v)};
    });
  };
};

static_assert(DeltaExprParam::StorageT::kCanInline);
// static_assert(DeltaNCoExprParam::StorageT::kCanInline);
}  // namespace internal


using DeltaExpr = Linear<internal::DeltaExprParam>;
using DeltaICoExpr = Linear<internal::DeltaICoExprParam>;
using DeltaNCoExpr = Linear<internal::DeltaNCoExprParam>;
using DeltaACoExpr = Linear<internal::DeltaACoExprParam>;
template<> struct ICoExprForExpr<DeltaExpr> { using type = DeltaICoExpr; };
template<> struct NCoExprForExpr<DeltaExpr> { using type = DeltaNCoExpr; };
template<> struct ACoExprForExpr<DeltaExpr> { using type = DeltaACoExpr; };

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


DeltaExpr substitute_variables_0_based(const DeltaExpr& expr, const XArgs& new_points);
DeltaExpr substitute_variables_1_based(const DeltaExpr& expr, const XArgs& new_points);
DeltaNCoExpr substitute_variables_0_based(const DeltaNCoExpr& expr, const XArgs& new_points);
DeltaNCoExpr substitute_variables_1_based(const DeltaNCoExpr& expr, const XArgs& new_points);

// Expects: points.size() == 6
// Eliminates terms (x5-x6), (x4-x6), (x2-x6) using involution x1<->x4, x2<->x5, x3<->x6.
// DEPRECATED. Use x4=-x1, x5=-x2, x6=-x3 instead.
// TODO: Is it really superseeded by substituting -x_i? If so, delete.
DeltaExpr involute(const DeltaExpr& expr, const std::vector<int>& points);

DeltaExpr sort_term_multiples(const DeltaExpr& expr);
DeltaExpr terms_with_unique_multiples(const DeltaExpr& expr);
DeltaExpr terms_with_nonunique_multiples(const DeltaExpr& expr);

DeltaExpr terms_with_num_distinct_variables(const DeltaExpr& expr, int num_distinct);
DeltaExpr terms_with_min_distinct_variables(const DeltaExpr& expr, int min_distinct);
DeltaExpr terms_containing_only_variables(const DeltaExpr& expr, const std::vector<int>& indices);
DeltaExpr terms_without_variables(const DeltaExpr& expr, const std::vector<int>& indices);

bool are_weakly_separated(const Delta& d1, const Delta& d2);
bool is_weakly_separated(const DeltaExpr::ObjectT& term);
bool is_weakly_separated(const DeltaNCoExpr::ObjectT& term);

// HACK: "Unglues" points assuming (x_i-0) compues only from (x_i-(-x_i)).
// TODO: Proper solution that doesn't silently give wrong results with `Zero`.
std::array<X, 2> delta_points_inv(const Delta& d);

// Functions ending with `_inv` work on type C spaces, i.e. 2n points in involution.
// They support only polylogs of +x_i, -x_i and Inf. Feeding these function an expression
// produced from Zero arguments will silenty yield incorrect results (!) due the ungluing
// trick (see `delta_points_inv`).
// TODO: Strong typing for space kinds. Split DeltaExpr into:
//   * Type A: only +x_i (and Inf);
//   * Type C: +x_i and -x_i (and Inf);
//   * Free expressions including Zero, x_i^2, etc.
bool are_weakly_separated_inv(const Delta& d1, const Delta& d2);
bool is_weakly_separated_inv(const DeltaExpr::ObjectT& term);
bool is_weakly_separated_inv(const DeltaNCoExpr::ObjectT& term);
bool is_totally_weakly_separated_inv(const DeltaExpr& expr);
bool is_totally_weakly_separated_inv(const DeltaNCoExpr& expr);
DeltaExpr keep_non_weakly_separated_inv(const DeltaExpr& expr);
DeltaNCoExpr keep_non_weakly_separated_inv(const DeltaNCoExpr& expr);

// TODO: Allow circular normalization when the number of points is odd.
bool passes_normalize_remove_consecutive(const DeltaExpr::ObjectT& term);
DeltaExpr normalize_remove_consecutive(const DeltaExpr& expr);

// Checks that points [q_1, ..., q_2n] are such that q_i == -q_{i+n}
bool inv_points_are_central_symmetric(const XArgs& points);

DeltaExpr terms_with_connected_variable_graph(const DeltaExpr& expr);

// For using together with `DeltaExpr::filter`
int count_var(const DeltaExpr::ObjectT& term, int var);
int num_distinct_variables(const std::vector<Delta>& term);

void print_sorted_by_num_distinct_variables(std::ostream& os, const DeltaExpr& expr);
