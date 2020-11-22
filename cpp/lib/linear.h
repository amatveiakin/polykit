#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"

#include "format.h"
#include "util.h"


template<typename T>
struct SimpleLinearParam {
  using ObjectT = T;
  using StorageT = T;
  static StorageT object_to_key(const ObjectT& obj) { return obj; }
  static ObjectT key_to_object(const StorageT& key) { return key; }
  // can be overwritten if necessary
  static std::string object_to_string(const ObjectT& obj) { return to_string(obj); }
  // left to define (optional; if missing the corresponding functionality will be unavailable):
  // static StorageT monom_tensor_product(const StorageT& lhs, const StorageT& rhs);
  // static int object_to_weight(const ObjectT& obj);
  // static StorageT shuffle_preprocess(const StorageT& key);
  // static StorageT shuffle_postprocess(const StorageT& key);
  // static constexpr bool coproduct_is_lie_algebra = ...;
  // TODO: Try to find clearer structure that doesn't require shuffle_preprocess/shuffle_postprocess
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

  BasicLinear() {}
  // TODO: debug `template<typename OtherParamT> friend class BasicLinear<OtherParamT>` and move to private
  BasicLinear(absl::flat_hash_map<StorageT, int> data) : data_(std::move(data)) {}
  ~BasicLinear() {}

  static BasicLinear single(const ObjectT& obj) {
    return single_key(ParamT::object_to_key(obj));
  }
  static BasicLinear single_key(const StorageT& key) {
    BasicLinear ret;
    ret.data_[key] = 1;
    return ret;
  }

  bool zero() const { return data_.empty(); }
  int size() const { return data_.size(); }
  int l1_norm() const {
    int ret = 0;
    foreach_key([&](const auto&, int coeff) { ret += std::abs(coeff); });
    return ret;
  }
  int weight() const {
    CHECK(!zero());
    return ParamT::object_to_weight(element().first);  // must be the same for each term
  }

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
        assert(coeff != 0);
      }
    }
    return *this;
  }
  void div_int(int scalar) {
    assert(scalar != 0);
    for (auto& [key, coeff]: data_) {
      coeff = ::div_int(coeff, scalar);
      assert(coeff != 0);
    }
  }

  bool operator==(const BasicLinear& other) const {
    return ((*this) - other).zero();
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

  absl::flat_hash_map<StorageT, int> data_;
};

template<typename ParamT>
BasicLinear<ParamT> operator*(int scalar, const BasicLinear<ParamT>& linear) {
  return linear * scalar;
}

template<typename ParamT, typename ContextT>
std::ostream& to_ostream(
    std::ostream& os,
    const BasicLinear<ParamT>& linear,
    const ContextT& context) {
  std::vector<std::pair<typename ParamT::ObjectT, int>> dump;
  int max_coeff_length = 0;
  linear.foreach([&](const auto& obj, int coeff) {
    dump.push_back({obj, coeff});
    if (*current_formatting_config().formatter != Formatter::unicode) {
      max_coeff_length = std::max<int>(max_coeff_length, fmt::coeff(coeff).length());
    }
  });
  std::sort(dump.begin(), dump.end());
  const int line_limit = *current_formatting_config().expression_line_limit;
  if (line_limit > 0) {
    int line = 0;
    for (const auto& [obj, coeff] : dump) {
      ++line;
      if (line > line_limit) {
        os << fmt::box("...");
        return os;
      }
      // TODO: Fix how padding works for parsable expressions
      os << pad_left(fmt::coeff(coeff), max_coeff_length);
      if constexpr (std::is_same_v<ContextT, LinearNoContext>) {
        os << fmt::box(ParamT::object_to_string(obj));
      } else {
        os << fmt::box(ParamT::object_to_string(obj, context));
      }
    }
  }
  return os;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const BasicLinear<ParamT>& linear) {
  return to_ostream(os, linear, LinearNoContext{});
}


using BasicLinearAnnotation = BasicLinear<SimpleLinearParam<std::string>>;

// Stores information about the functions that created a linear expression.
struct LinearAnnotation {
  BasicLinearAnnotation expression;
  std::vector<std::string> errors;

  bool empty() const { return expression.zero() && errors.empty(); }
  bool has_errors() const { return !errors.empty(); }
};

inline std::ostream& operator<<(std::ostream& os, const LinearAnnotation& annotations) {
  os << annotations.expression;
  for (const auto& err : annotations.errors) {
    os << fmt::coeff(1) << "<?> " << err << "\n";
  }
  return os;
}


template<typename ParamT>
class Linear {
public:
  using Param = ParamT;
  using ObjectT = typename ParamT::ObjectT;
  using StorageT = typename ParamT::StorageT;
  using BasicLinearMain = BasicLinear<ParamT>;

  Linear() {}

  // TODO: debug `template<typename OtherParamT> friend class Linear<OtherParamT>` and move to private
  Linear(BasicLinearMain main, LinearAnnotation annotations)
    : main_(std::move(main)), annotations_(std::move(annotations)) {}

  // TODO: Replace "[+-]=.*::single" with `add_to`
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

  bool zero() const { return main_.zero(); }  // TODO: rename to `is_zero`
  bool blank() const { return main_.zero() && annotations_.empty(); }  // TODO: rename to `is_blank`
  int size() const { return main_.size(); }
  int l1_norm() const { return main_.l1_norm(); }
  int weight() const { return main_.weight(); }

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
    return ret;
  }
  template<typename F>
  auto mapped_key_expanding(F func) const {
    using ResultT = std::invoke_result_t<F, StorageT>;
    static_assert(is_linear_v<ResultT>, "mapped_expanding functor must return a Linear expression");
    ResultT ret;
    foreach_key([&](const auto& key, int coeff) {
      ret += coeff * func(key);
    });
    return ret;
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
    annotations_.expression += BasicLinearAnnotation::single(annotation);
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
    add_annotations(other, 1);
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
    add_annotations(other, 1);
    return *this;
  }
  Linear& operator-=(const Linear& other) {
    main_ -= other.main_;
    add_annotations(other, -1);
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
  template<typename SourceLinearT>
  void add_annotations(const SourceLinearT& other, int coeff) {
    if (other.blank()) {
      return;
    }
    if (blank()) {
      annotations_ = other.annotations();
      return;
    }
    annotations_.expression += coeff * other.annotations().expression;
    append_vector(annotations_.errors, other.annotations().errors);
    if (annotations_.empty() != other.annotations().empty()) {
      annotations_.errors.push_back("");  // will display simply "<?>"
    }
    absl::c_sort(annotations_.errors);
    keep_unique(annotations_.errors);
  }

  BasicLinearMain main_;
  LinearAnnotation annotations_;
};

template<typename ParamT>
Linear<ParamT> operator*(int scalar, const Linear<ParamT>& linear) {
  return linear * scalar;
}

template<typename ParamT, typename ContextT>
std::ostream& to_ostream(
    std::ostream& os,
    const Linear<ParamT>& linear,
    const ContextT& context) {
  const int line_limit = *current_formatting_config().expression_line_limit;
  if (!linear.zero()) {
    const int size = linear.size();
    os << "# " << size << " " << en_plural(size, "term");
    os << ", |coeff| = " << linear.l1_norm();
    if (line_limit > 0) {
      os << ":\n";
      to_ostream(os, linear.main(), context);
    } else {
      os << "\n";
    }
  } else {
    if (line_limit > 0) {
      os << "\n";
    }
    os << fmt::coeff(0) << "\n";
  }
  const auto& annotations = linear.annotations();
  if (!annotations.empty() &&
      *current_formatting_config().expression_include_annotations) {
    ScopedFormatting sf(FormattingConfig()
      .set_expression_line_limit(FormattingConfig::kNoLineLimit));
    os << "~~~\n";
    os << annotations;
  }
  os.flush();
  return os;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const Linear<ParamT>& linear) {
  return to_ostream(os, linear, LinearNoContext{});
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
