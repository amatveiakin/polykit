// Generic class for Linear expressions with integer coefficients.
//
// Linear expressions are strongly typed and polymorphic. I.e. each expression stores
// objects of one particular type, but there can be many types of linear expressions.
// A linear expression is essentially a map from type T (object) to integer (coefficient)
// with a set of operation defined on it.
//
//
// # Linear expression params
//
// The type of the underlying objects and some aspects of these operations are defined
// in a special parameters structure that is passed to a linear expression as a template
// parameter. See `SimpleLinearParam` for the full list of parameters.
//
//
// # Important operations
//
// Constructing expressions:
//   * `single(x)` constructs a linear expression 1*x from monom x.
//   * `from_collection(container)` counts elements in a container, e.g.
//     turns {a, a, b, c, a, c, c} into 3*a + b + 3*c.
//
// Manipulating entire expressions:
//   * Arithmetics: addition (+, +=), subtraction(-, -=), multiplication by scalar (*, *=),
//     division by scalar (dived_int, div_int). In case of division each coefficient must
//     by divisible without a remainder or an IntegerDivisionError is thrown.
//   * `mapped(f)` replaces each term a*x with a*f(x).
//   * `mapped_expanding(f)` does the same, but f returns an expression rather than a monom.
//   * `filtered(f)` keeps only terms a*x where f(x) is true.
//   * `foreach(f)` iterates over the expression calling f(x, a) for every term a*x.
//   * `contains(f)` tells whether the expressions contains a term a*x such that f(x) is true.
//
// Manipulating terms:
//   * `operator[](x)` returns a if the expression has term a*x or 0 otherwise.
//   * `add_to(x, v)` adds v to the coefficient at x.
//      Equivalent to `expr += v * ExprType::single(x)`, but faster.
//   * `element()` returns some {term, coeff} pair assuming the expression is not zero.
//   * `pop()` removes the term returned by `element`.
//
//
// # Object space vs key space
//
// In order to optimize CPU and RAM usage linear expression stores objects in a compact
// form. The convenient full form is called `ObjectT` and referred to as "object" and the
// compact form is called `StorageT` and referred to as "key". The particular types of
// ObjectT and StorageT and conversion functions between them are defined in linear
// expression params.
//
// Most of the time it's ok to work in object space. This is true for all one-pass algorithms
// like post hoc variable substitutions, output, etc. Only the most performance-critical
// functions which do massive computations should concern themselves with key space.
// In order to work in key space one needs to use the versions of methods that end with `_key`,
// e.g. `mapped_key` instead of `mapped`, `foreach_key` instead of `foreach` etc.
//
//
// # Vector space
//
// Vector space is the third space akin to object space and key space, but, generally
// speaking, distinct from both of them. In vector space each monom must be a vector-like
// type that supports indexing, slicing and concatenating. Vector space is used by
// functions like `shuffle_product`, `to_lyndon_basis` and `comultiply` that treat the
// monom as a sequence of "characters".
//
// Having vector space support is optional. Not all linear expression types have a vector
// space and even if they do, it is not guaranteed that every particular expression of that
// type is convertible to a vector space, e.g. an expression containing formal symbols
// cannot be convrted to vector space.
//
//
// # Annotations
//
// A linear expression can be annotated with a function used to create it. Annotations are
// carried over with the expression and all linear operations (+, -, *, div_int) applied to
// the expression are applied to the annotations as well. Annotations are a convenient way
// to keep track of the functions that went into an equation. Use `annotate` in order to add
// a new annotations to an expression.

#pragma once

#include <algorithm>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"

#include "compare.h"
#include "format.h"
#include "util.h"


// TODO: Why is `std::move` needed (suggested by clang)?  (here and in IDENTITY_VECTOR_FORM below)
#define IDENTITY_STORAGE_FORM                                                  \
  using StorageT = ObjectT;                                                    \
  static const ObjectT& key_to_object(const StorageT& key) { return key; }     \
  static ObjectT key_to_object(StorageT&& key) { return std::move(key); }      \
  static const StorageT& object_to_key(const ObjectT& obj) { return obj; }     \
  static StorageT object_to_key(ObjectT&& obj) { return std::move(obj); }

template<typename T>
struct SimpleLinearParam {
  using ObjectT = T;
  IDENTITY_STORAGE_FORM
  // can be overwritten if necessary
  static std::string object_to_string(const ObjectT& obj) { return to_string(obj); }

  // === left to define (optional; if missing the corresponding functionality will be unavailable):

  // static int object_to_weight(const ObjectT& obj);
  // static int object_to_dimension(const ObjectT& obj);
  // static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs);

  // using VectorT = ...;
  // static VectorT key_to_vector(const StorageT& key);
  // static StorageT vector_to_key(const VectorT& vec);
  //
  // TODO: key_to_vector/vector_to_key conversion functions must be defined iff  VectorT != StorageT.
  //     It should be a compile-time error to have key_to_vector when VectorT == StorageT. This will
  //     allow to reliably omit conversion when it's not needed.

  // TODO: Consider uniting these into one enum CoproductType (normal, iterative, hopf)
  // static constexpr bool coproduct_is_lie_algebra = ...;
  // static constexpr bool coproduct_is_iterated = ...;
};

// TODO: Implement mixins without macros. Idea: make a template that's used like this:
// sturct MyLinearParam : LinearParam<
//   MyStorageType,  // StorageT
//   object_form_identical -OR- MyObjectType,
//   vector_form_identical -OR- vector_not_supported -OR- MyObjectType,
//   default_object_to_str | lyndon_compare_default | lyndon_compare_length_first  // Specify any subset
// > {
//   ...
// };
// Also consider: https://brevzin.github.io/c++/2019/12/02/named-arguments/

#define IDENTITY_VECTOR_FORM                                                   \
  using VectorT = StorageT;                                                    \
  static const VectorT& key_to_vector(const StorageT& key) { return key; }     \
  static VectorT key_to_vector(StorageT&& key) { return std::move(key); }      \
  static const StorageT& vector_to_key(const VectorT& vec) { return vec; }     \
  static StorageT vector_to_key(VectorT&& vec) { return std::move(vec); }

template<typename T>
bool compare_length_first(const T& lhs, const T& rhs) {
  using namespace cmp;
  return projected(lhs, rhs, [](const auto& v) {
    return std::tuple{asc_val(v.size()), asc_ref(v)};
  });
}

#define LYNDON_COMPARE_DEFAULT                                                 \
  static bool lyndon_compare(                                                  \
      const typename VectorT::value_type& lhs,                                 \
      const typename VectorT::value_type& rhs) {                               \
    return lhs < rhs;                                                          \
  }

#define LYNDON_COMPARE_LENGTH_FIRST                                            \
  static bool lyndon_compare(                                                  \
      const typename VectorT::value_type& lhs,                                 \
      const typename VectorT::value_type& rhs) {                               \
    return compare_length_first(lhs, rhs);                                     \
  }


template<typename BaseParamT>
struct VectorLinearParam : SimpleLinearParam<typename BaseParamT::VectorT> {
  static std::string object_to_string(const typename BaseParamT::VectorT& vec) {
    return BaseParamT::object_to_string(BaseParamT::key_to_object(BaseParamT::vector_to_key(vec)));
  }
};


struct LinearNoContext {};


template<typename, typename = void>
struct is_linear : std::false_type {};
template<typename T>
struct is_linear<T, std::void_t<typename T::ObjectT, typename T::StorageT, typename T::BasicLinearMain>> : std::true_type {};
template <typename T>
inline constexpr bool is_linear_v = is_linear<T>::value;


template<typename ParamT>
class BasicLinear {
public:
  using ObjectT = typename ParamT::ObjectT;
  using StorageT = typename ParamT::StorageT;
#if DISABLE_PACKING
  using ContainerT = std::unordered_map<StorageT, int, absl::Hash<StorageT>>;
#else
  using ContainerT = absl::flat_hash_map<StorageT, int>;
#endif
  using const_key_iterator = typename ContainerT::const_iterator;

  class const_iterator {
  public:
    explicit const_iterator(const_key_iterator it) : it_(std::move(it)) {}
    std::pair<ObjectT, int> operator*() const { return {ParamT::key_to_object(it_->first), it_->second}; }
    const_iterator& operator++() { ++it_; return *this; };
    bool operator==(const const_iterator& other) const { return it_ == other.it_; }
    bool operator!=(const const_iterator& other) const { return !(*this == other); }
  private:
    const_key_iterator it_;
  };

  BasicLinear() {}
  explicit BasicLinear(ContainerT data) : data_(std::move(data)) {
    for (auto it = data_.begin(); it != data_.end(); ) {
      auto next = it;
      ++next;
      if (it->second == 0) {
        data_.erase(it);
      }
      it = next;
    }
  }
  ~BasicLinear() {}

  static BasicLinear single(const ObjectT& obj) {
    return single_key(ParamT::object_to_key(obj));
  }
  static BasicLinear single_key(const StorageT& key) {
    BasicLinear ret;
    ret.data_[key] = 1;
    return ret;
  }

  bool is_zero() const { return data_.empty(); }
  int num_terms() const { return data_.size(); }
  int l0_norm() const { return num_terms(); }
  int l1_norm() const {
    int ret = 0;
    foreach_key([&](const auto&, int coeff) { ret += std::abs(coeff); });
    return ret;
  }
  int weight() const {
    CHECK(!is_zero());
    return ParamT::object_to_weight(element().first);  // must be the same for each term
  }
  int dimension() const {  // Grassmannian dimension
    CHECK(!is_zero());
    return ParamT::object_to_dimension(element().first);  // must be the same for each term
  }

  const_key_iterator begin_key() const { return data_.begin(); }
  const_key_iterator end_key() const { return data_.end(); }
  const_iterator begin() const { return const_iterator(begin_key()); }
  const_iterator end() const { return const_iterator(end_key()); }

  int operator[](const ObjectT& obj) const {
    return coeff_for_key(ParamT::object_to_key(obj));
  }
  int coeff_for_key(const StorageT& key) const {
    const auto it = data_.find(key);
    if (it != data_.end()) {
      return it->second;
    } else {
      return 0;
    }
  }
  const ContainerT& data() const { return data_; }

  void add_to(const ObjectT& obj, int x) {
    add_to_key(ParamT::object_to_key(obj), x);
  }
  void add_to_key(const StorageT& key, int x) {
    // Equivalent to `set_coeff_for_key(key, coeff_for_key(key) + x);` but faster.
    int& value = data_[key];
    value += x;
    if (value == 0) {
      data_.erase(key);
    }
  }
  std::pair<ObjectT, int> element() const {
    auto key_coeff = element_key();
    return {ParamT::key_to_object(key_coeff.first), key_coeff.second};
  }
  std::pair<StorageT, int> element_key() const {
    return *data_.begin();
  }
  std::pair<ObjectT, int> pop() {
    auto key_coeff = pop_key();
    return {ParamT::key_to_object(key_coeff.first), key_coeff.second};
  }
  std::pair<StorageT, int> pop_key() {
    auto ret = std::move(*data_.begin());
    data_.erase(data_.begin());
    return ret;
  }

  template<typename F>
  void foreach(F func) const {
    foreach_key([&func](const auto& key, int coeff) {
      func(ParamT::key_to_object(key), coeff);
    });
  }
  template<typename F>
  void foreach_key(F func) const {
    for (const auto& [key, coeff]: data_) {
      func(key, coeff);
    }
  }

  // TODO: Consider renaming to `any_of`
  template<typename F>
  bool contains(F func) const {
    for (const auto& [term, coeff] : *this) {
      if (func(term)) {
        return true;
      }
    }
    return false;
  }
  template<typename F>
  bool contains_key(F func) const {
    for (auto it = begin_key(); it != end_key(); ++it) {
      if (func(it->second)) {
        return true;
      }
    }
    return false;
  }

  template<typename NewBasicLinearT, typename F>
  NewBasicLinearT mapped(F func) const {
    NewBasicLinearT ret;
    foreach([&](const auto& obj, int coeff) {
      ret.add_to(func(obj), coeff);
    });
    return ret;
  }

  template<typename F>
  BasicLinear mapped(F func) const {
    return mapped<BasicLinear>(func);
  }
  template<typename NewBasicLinearT, typename F>
  NewBasicLinearT mapped_key(F func) const {
    NewBasicLinearT ret;
    foreach_key([&](const auto& key, int coeff) {
      ret.add_to_key(func(key), coeff);
    });
    return ret;
  }
  template<typename F>
  BasicLinear mapped_key(F func) const {
    return mapped_key<BasicLinear>(func);
  }

  template<typename NewBasicLinearT>
  NewBasicLinearT cast_to() const {
    static_assert(std::is_same_v<typename NewBasicLinearT::StorageT, StorageT>);
    return NewBasicLinearT(data_);
  }

  template<typename F>
  BasicLinear filtered(F func) const {
    BasicLinear ret;
    foreach_key([&](const auto& key, int coeff) {
      if (func(ParamT::key_to_object(key))) {
        ret.add_to_key(key, coeff);
      }
    });
    return ret;
  }
  template<typename F>
  BasicLinear filtered_key(F func) const {
    BasicLinear ret;
    foreach_key([&](const auto& key, int coeff) {
      if (func(key)) {
        ret.add_to_key(key, coeff);
      }
    });
    return ret;
  }

  BasicLinear termwise_abs() const {
    BasicLinear ret;
    foreach_key([&](const auto& key, int coeff) {
      ret.add_to_key(key, std::abs(coeff));
    });
    return ret;
  }

  BasicLinear operator+(const BasicLinear& other) const {
    BasicLinear ret = *this;
    ret += other;
    return ret;
  }
  BasicLinear operator-(const BasicLinear& other) const {
    BasicLinear ret = *this;
    ret -= other;
    return ret;
  }
  BasicLinear operator*(int scalar) const {
    BasicLinear ret = *this;
    ret *= scalar;
    return ret;
  }
  BasicLinear dived_int(int scalar) const {
    BasicLinear ret = *this;
    ret.div_int(scalar);
    return ret;
  }

  BasicLinear& operator+=(const BasicLinear& other) {
    for (const auto& [key, coeff]: other.data_) {
      set_coeff_for_key(key, coeff_for_key(key) + coeff);
    }
    return *this;
  }
  BasicLinear& operator-=(const BasicLinear& other) {
    for (const auto& [key, coeff]: other.data_) {
      set_coeff_for_key(key, coeff_for_key(key) - coeff);
    }
    return *this;
  }
  BasicLinear& operator*=(int scalar) {
    if (scalar == 0) {
      *this = BasicLinear();
    } else {
      for (auto& [key, coeff]: data_) {
        coeff *= scalar;
        ASSERT(coeff != 0);
      }
    }
    return *this;
  }
  void div_int(int scalar) {
    CHECK(scalar != 0);
    for (auto& [key, coeff]: data_) {
      coeff = ::div_int(coeff, scalar);
      ASSERT(coeff != 0);
    }
  }

  bool operator==(const BasicLinear& other) const {
    return ((*this) - other).is_zero();
  }
  bool operator!=(const BasicLinear& other) const {
    return !(*this == other);
  }

private:
  void set_coeff_for_key(const StorageT& key, int coeff) {
    if (coeff != 0) {
      data_[key] = coeff;
    } else {
      data_.erase(key);
    }
  }

  ContainerT data_;
};

template<typename ParamT>
BasicLinear<ParamT> operator*(int scalar, const BasicLinear<ParamT>& linear) {
  return linear * scalar;
}

template<typename ParamT, typename CompareF, typename ContextT>
std::ostream& to_ostream(
    std::ostream& os,
    const BasicLinear<ParamT>& linear,
    const CompareF& sorting_cmp,
    const ContextT& context) {
  std::vector<std::pair<typename ParamT::ObjectT, int>> dump;
  int max_coeff_length = 0;
  linear.foreach([&](const auto& obj, int coeff) {
    dump.push_back({obj, coeff});
    // TODO: Use "figure space" (U+2007) for Unicode.
    max_coeff_length = std::max<int>(max_coeff_length, strlen_utf8(fmt::coeff(coeff)));
  });
  std::sort(dump.begin(), dump.end(), [&](const auto& a, const auto& b) {
    return sorting_cmp(a.first, b.first);
  });
  const int line_limit = *current_formatting_config().expression_line_limit;
  if (line_limit > 0) {
    int line = 0;
    for (const auto& [obj, coeff] : dump) {
      ++line;
      if (line > line_limit) {
        os << "..." << fmt::newline();
        return os;
      }
      // TODO: Fix how padding works for parsable expressions
      os << pad_left(fmt::coeff(coeff), max_coeff_length);
      if constexpr (std::is_same_v<ContextT, LinearNoContext>) {
        os << ParamT::object_to_string(obj) << fmt::newline();
      } else {
        os << ParamT::object_to_string(obj, context) << fmt::newline();
      }
    }
  }
  return os;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const BasicLinear<ParamT>& linear) {
  return to_ostream(os, linear, std::less<>{}, LinearNoContext{});
}


using BasicLinearAnnotation = BasicLinear<SimpleLinearParam<std::string>>;

// Stores information about the functions that created a linear expression.
struct LinearAnnotation {
  BasicLinearAnnotation expression;
  std::vector<std::string> errors;

  bool empty() const { return expression.is_zero() && errors.empty(); }
  bool has_errors() const { return !errors.empty(); }
};

std::ostream& operator<<(std::ostream& os, const LinearAnnotation& annotations);
std::string annotations_one_liner(const LinearAnnotation& annotations);


template<typename ParamT>
class Linear {
public:
  using Param = ParamT;
  using ObjectT = typename ParamT::ObjectT;
  using StorageT = typename ParamT::StorageT;
  using BasicLinearMain = BasicLinear<ParamT>;
  using const_iterator = typename BasicLinearMain::const_iterator;
  using const_key_iterator = typename BasicLinearMain::const_key_iterator;

  Linear() {}
  explicit Linear(BasicLinearMain main, LinearAnnotation annotations)
    : main_(std::move(main)), annotations_(std::move(annotations)) {}

  static Linear single(const ObjectT& obj) {
    Linear ret;
    ret.main_ = BasicLinearMain::single(obj);
    return ret;
  }
  template<typename T>
  static Linear from_collection(const T& container) {
    Linear ret;
    for (const ObjectT& obj : container) {
      ret.add_to(obj, 1);
    }
    return ret;
  }

  static Linear single_key(const StorageT& obj) {
    Linear ret;
    ret.main_ = BasicLinearMain::single_key(obj);
    return ret;
  }
  template<typename T>
  static Linear from_key_collection(const T& container) {
    Linear ret;
    for (const StorageT& key : container) {
      ret += Linear::single_key(key);
    }
    return ret;
  }

  bool is_zero() const { return main_.is_zero(); }
  bool is_blank() const { return main_.is_zero() && annotations_.empty(); }
  int num_terms() const { return main_.num_terms(); }
  int l0_norm() const { return main_.l0_norm(); }
  int l1_norm() const { return main_.l1_norm(); }
  int weight() const { return main_.weight(); }
  int dimension() const { return main_.dimension(); }

  const_key_iterator begin_key() const { return main_.begin_key(); }
  const_key_iterator end_key() const { return main_.end_key(); }
  const_iterator begin() const { return main_.begin(); }
  const_iterator end() const { return main_.end(); }

  int operator[](const ObjectT& obj) const { return main_[obj]; }
  int coeff_for_key(const StorageT& key) const { return main_.coeff_for_key(key); }
  void add_to(const ObjectT& obj, int x) { return main_.add_to(obj, x); }
  void add_to_key(const StorageT& key, int x)  { return main_.add_to_key(key, x); }
  std::pair<ObjectT, int> element() const { return main_.element(); }
  std::pair<StorageT, int> element_key() const { return main_.element_key(); }
  std::pair<ObjectT, int> pop() { return main_.pop(); }
  std::pair<StorageT, int> pop_key() { return main_.pop_key(); }

  template<typename F>
  void foreach(F func) const { return main_.foreach(func); }
  template<typename F>
  void foreach_key(F func) const { return main_.foreach_key(func); }

  template<typename F>
  bool contains(F func) const { return main_.contains(func); }
  template<typename F>
  bool contains_key(F func) const { return main_.contains_key(func); }

  template<typename NewLinearT, typename F>
  NewLinearT mapped(F func) const {
    return NewLinearT(main_.template mapped<typename NewLinearT::BasicLinearMain>(func), annotations_);
  }
  template<typename F>
  Linear mapped(F func) const {
    return mapped<Linear>(func);
  }
  template<typename NewLinearT, typename F>
  NewLinearT mapped_key(F func) const {
    return NewLinearT(main_.template mapped_key<typename NewLinearT::BasicLinearMain>(func), annotations_);
  }
  template<typename F>
  Linear mapped_key(F func) const {
    return mapped_key<Linear>(func);
  }

  // Note that there is a difference in result type deduction between `mapped`
  // and `mapped_expanding`:
  //   * `mapped` needs an explicit result type specification; it assumes that
  //     result type is the same as the source type if the template parameter
  //     is not specified.
  //   * `mapped_expanding` deduces result type from the functor.
  template<typename F>
  auto mapped_expanding(F func) const {
    using ResultT = std::invoke_result_t<F, ObjectT>;
    static_assert(is_linear_v<ResultT>, "mapped_expanding functor must return a Linear expression");
    ResultT ret;
    foreach([&](const auto& obj, int coeff) {
      ret += coeff * func(obj);
    });
    return ret.copy_annotations(*this);
  }
  template<typename F>
  auto mapped_key_expanding(F func) const {
    using ResultT = std::invoke_result_t<F, StorageT>;
    static_assert(is_linear_v<ResultT>, "mapped_expanding functor must return a Linear expression");
    ResultT ret;
    foreach_key([&](const auto& key, int coeff) {
      ret += coeff * func(key);
    });
    return ret.copy_annotations(*this);
  }

  template<typename NewLinearT>
  NewLinearT cast_to() const {
    return NewLinearT(main_.template cast_to<typename NewLinearT::BasicLinearMain>(), annotations_);
  }

  template<typename F>
  Linear filtered(F func) const {
    return Linear(main_.filtered(func), annotations_);
  }
  template<typename F>
  Linear filtered_key(F func) const {
    return Linear(main_.filtered_key(func), annotations_);
  }

  Linear termwise_abs() const {
    return Linear(main_.termwise_abs(), annotations_);
  }

  const BasicLinearMain& main() const { return main_; };
  const LinearAnnotation& annotations() const { return annotations_; };

  Linear& annotate(const std::string& annotation) {
    annotations_.expression.add_to_key(annotation, 1);
    return *this;
  }
  Linear& maybe_annotate(const std::optional<std::string>& annotation) {
    if (annotation.has_value()) {
      annotate(annotation.value());
    }
    return *this;
  }
  template<typename SourceLinearT>
  Linear& copy_annotations(const SourceLinearT& other) {
    add_annotations(other, 1, identity_function);
    return *this;
  }
  template<typename SourceLinearT, typename F>
  Linear& copy_annotations_mapped(const SourceLinearT& other, F func) {
    add_annotations(other, 1, func);
    return *this;
  }
  template<typename F>
  Linear& annotations_map(F func) {
    annotations_.expression = annotations_.expression.mapped(func);
    return *this;
  }
  Linear without_annotations() const {
    return Linear(main_, {});
  }

  Linear operator+() const {
    return *this;
  }
  Linear operator-() const {
    return -1 * (*this);
  }

  Linear operator+(const Linear& other) const {
    Linear ret = *this;
    ret += other;
    return ret;
  }
  Linear operator-(const Linear& other) const {
    Linear ret = *this;
    ret -= other;
    return ret;
  }
  Linear operator*(int scalar) const {
    Linear ret = *this;
    ret *= scalar;
    return ret;
  }
  Linear dived_int(int scalar) const {
    Linear ret = *this;
    ret.div_int(scalar);
    return ret;
  }

  Linear& operator+=(const Linear& other) {
    main_ += other.main_;
    add_annotations(other, 1, identity_function);
    return *this;
  }
  Linear& operator-=(const Linear& other) {
    main_ -= other.main_;
    add_annotations(other, -1, identity_function);
    return *this;
  }
  Linear& operator*=(int scalar) {
    main_ *= scalar;
    annotations_.expression *= scalar;
    return *this;
  }
  void div_int(int scalar) {
    main_.div_int(scalar);
    try {
      annotations_.expression.div_int(scalar);
    } catch (IntegerDivisionError error) {
      annotations_ = {{}, {error.what()}};
    }
  }

  bool operator==(const Linear& other) const {
    return main_ == other.main_;
  }
  bool operator!=(const Linear& other) const {
    return !(*this == other);
  }

private:
  template<typename SourceLinearT, typename F>
  void add_annotations(const SourceLinearT& other, int coeff, F func) {
    if (other.is_blank()) {
      return;
    }
    const LinearAnnotation other_annotations{
      other.annotations().expression.mapped(func),
      other.annotations().errors
    };
    if (is_blank()) {
      annotations_ = other_annotations;
      return;
    }
    annotations_.expression += coeff * other_annotations.expression;
    append_vector(annotations_.errors, other_annotations.errors);
    if (annotations_.empty() != other_annotations.empty()) {
      annotations_.errors.push_back("");  // will display simply "<?>"
    }
    absl::c_sort(annotations_.errors);
    keep_unique_sorted(annotations_.errors);
  }

  BasicLinearMain main_;
  // Optimization potential: wrap this into a unique_ptr. Most expressions don't have annotations,
  // so no need to carry this around.
  LinearAnnotation annotations_;
};

template<typename ParamT>
Linear<ParamT> operator*(int scalar, const Linear<ParamT>& linear) {
  return linear * scalar;
}

template<typename ParamT>
class LinearKeyView {
public:
  explicit LinearKeyView(const Linear<ParamT>& linear) : linear_(linear) {}
  explicit LinearKeyView(const Linear<ParamT>&& linear) = delete;
  using const_iterator = typename Linear<ParamT>::const_key_iterator;
  const_iterator begin() const { return linear_.begin_key(); };
  const_iterator end() const { return linear_.end_key(); };
private:
  const Linear<ParamT>& linear_;
};

template<typename ParamT>
LinearKeyView<ParamT> key_view(const Linear<ParamT>& linear) {
  return LinearKeyView<ParamT>(linear);
}
template<typename ParamT>
LinearKeyView<ParamT> key_view(const Linear<ParamT>&& linear) = delete;

template<typename ParamT, typename CompareF, typename ContextT>
std::ostream& to_ostream(
    std::ostream& os,
    const Linear<ParamT>& linear,
    const CompareF& sorting_cmp,
    const ContextT& context) {
  const int line_limit = *current_formatting_config().expression_line_limit;
  if (!linear.is_zero()) {
    const int num_terms = linear.num_terms();
    os << "# " << num_terms << " " << en_plural(num_terms, "term");
    os << ", |coeff| = " << linear.l1_norm();
    if (line_limit > 0) {
      os << ":" << fmt::newline();
      to_ostream(os, linear.main(), sorting_cmp, context);
    } else {
      os << fmt::newline();
    }
  } else {
    if (line_limit > 0) {
      os << fmt::newline();
    }
    os << fmt::coeff(0) << fmt::newline();
  }
  const auto& annotations = linear.annotations();
  if (!annotations.empty() &&
      *current_formatting_config().expression_include_annotations) {
    ScopedFormatting sf(FormattingConfig()
      .set_expression_line_limit(FormattingConfig::kNoLineLimit));
    os << "~~~" << fmt::newline();
    os << annotations;
  }
  if (*current_formatting_config().new_line_after_expression) {
    os << fmt::newline();
  }
  os.flush();
  return os;
}

template<typename ParamT, typename TermCompareF, typename GroupByF,
         typename GroupCompareF, typename GroupHeaderF, typename ContextT>
std::ostream& to_ostream_grouped(
    std::ostream& os,
    const Linear<ParamT>& linear,
    const TermCompareF& term_sorting_cmp,
    const GroupByF& group_by,
    const GroupCompareF& group_sorting_cmp,
    const GroupHeaderF& group_header,
    const ContextT& context) {
  using LinearT = Linear<ParamT>;
  using GroupT = std::invoke_result_t<GroupByF, typename ParamT::ObjectT>;
  const auto piece_to_ostream = [&](const LinearT& linear_piece) {
    ScopedFormatting sf(FormattingConfig().set_new_line_after_expression(false));
    to_ostream(os, linear_piece, term_sorting_cmp, context);
  };
  if (linear.is_zero()) {
    piece_to_ostream(linear);
    return os;
  }
  std::map<GroupT, LinearT, GroupCompareF> groups(group_sorting_cmp);
  linear.foreach([&](const auto& term, int coeff) {
    groups[group_by(term)].add_to(term, coeff);
  });
  bool first = true;
  for (const auto& [group_id, linear_piece] : groups) {
    if (!first) {
      os << "---" << fmt::newline();
    }
    first = false;
    os << group_header(group_id) << " ";
    piece_to_ostream(linear_piece);
  }
  if (*current_formatting_config().expression_include_annotations) {
    os << "~~~" << fmt::newline() << linear.annotations();
  }
  if (*current_formatting_config().new_line_after_expression) {
    os << fmt::newline();
  }
  os.flush();
  return os;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const Linear<ParamT>& linear) {
  return to_ostream(os, linear, std::less<>{}, LinearNoContext{});
}



// Optimization potential: mark IDENTITY_VECTOR_FORM as no-op and return a
// reference to the original expression; but forbid dangling reference!
template<typename LinearT>
auto to_vector_expression(const LinearT& expr) {
  return expr.template mapped_key<Linear<VectorLinearParam<typename LinearT::Param>>>([](auto vec) {
    return LinearT::Param::key_to_vector(std::move(vec));
  });
}

template<typename LinearT, typename VectorParamT>
LinearT from_vector_expression(const Linear<VectorParamT>& expr) {
  return expr.template mapped_key<LinearT>([](auto key) {
    return LinearT::Param::vector_to_key(std::move(key));
  });
}

using StringExpr = Linear<SimpleLinearParam<std::string>>;


template<typename ParamT>
struct PrintableLinear {
  Linear<ParamT> expression;
  FormattingConfig formatting_config;
};

template<typename ParamT>
PrintableLinear<ParamT> decorate_linear(
    Linear<ParamT> expression,
    FormattingConfig formatting_config) {
  return PrintableLinear<ParamT>{std::move(expression), std::move(formatting_config)};
}

template<typename ParamT>
PrintableLinear<ParamT> decorate_linear(
    PrintableLinear<ParamT> printable_expression,
    const FormattingConfig& formatting_config) {
  printable_expression.formatting_config.apply_overrides(formatting_config);
  return printable_expression;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const PrintableLinear<ParamT>& printable_linear) {
  ScopedFormatting sf(printable_linear.formatting_config);
  return os << printable_linear.expression;
}

namespace prnt {

template<typename T>
auto header_only(T expression) {
  return decorate_linear(std::move(expression), FormattingConfig()
    .set_expression_line_limit(0)
    .set_expression_include_annotations(false)
  );
}

template<typename T>
auto line_limit(int limit, T expression) {
  return decorate_linear(std::move(expression), FormattingConfig()
    .set_expression_line_limit(limit)
  );
}

template<typename T>
auto no_line_limit(T expression) {
  return line_limit(FormattingConfig::kNoLineLimit, expression);
}

}  // namespace prn
