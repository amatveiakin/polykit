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
  static StorageT object_to_key(const T& obj) { return obj; }
  static ObjectT key_to_object(const T& key) { return key; }
  // can be overwritten if necessary
  static std::string object_to_string(const T& obj) { return to_string(obj); }
};


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
    return element().first.size();  // should be the same for each term
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
    set_coeff_for_key(key, coeff_for_key(key) + x);
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
  template<typename NewBasicLinearT, typename F>
  NewBasicLinearT mapped_key(F func) const {
    NewBasicLinearT ret;
    foreach_key([&](const auto& key, int coeff) {
      ret.add_to_key(func(key), coeff);
    });
    return ret;
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

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const BasicLinear<ParamT>& linear) {
  std::vector<std::pair<typename ParamT::ObjectT, int>> dump;
  int max_coeff_length = 0;
  linear.foreach([&](const auto& obj, int coeff) {
    dump.push_back({obj, coeff});
    max_coeff_length = std::max<int>(max_coeff_length, format_coeff(coeff).length());
  });
  std::sort(dump.begin(), dump.end());
  for (const auto& [obj, coeff] : dump) {
    // TODO: Add an option for this.
    //
    // std::string coeff_str = format_coeff(coeff);
    // CHECK(coeff > 0);
    // if (std::abs(coeff) > 1) {
    //   CHECK(coeff_str.back() == ' ');
    //   coeff_str.back() = '*';
    // }
    // os << "    " << pad_left(coeff_str, max_coeff_length);
    os << pad_left(format_coeff(coeff), max_coeff_length);
    os << ParamT::object_to_string(obj) << "\n";
  }
  return os;
}

using BasicLinearAnnotation = BasicLinear<SimpleLinearParam<std::string>>;


template<typename ParamT>
class Linear {
public:
  using ObjectT = typename ParamT::ObjectT;
  using StorageT = typename ParamT::StorageT;
  using BasicLinearMain = BasicLinear<ParamT>;

  Linear() {}

  // TODO: debug `template<typename OtherParamT> friend class Linear<OtherParamT>` and move to private
  Linear(BasicLinearMain main, BasicLinearAnnotation annotations)
    : main_(std::move(main)), annotations_(std::move(annotations)) {}

  static Linear single(const ObjectT& obj) {
    Linear ret;
    ret.main_ = BasicLinearMain::single(obj);
    return ret;
  }
  template<typename T>
  static Linear from_collection(const T& container) {
    Linear ret;
    for (const int& v : container) {
      ret += Linear::single(v);
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

  bool zero() const { return main_.zero(); }
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
  template<typename NewLinearT, typename F>
  NewLinearT mapped_key(F func) const {
    return NewLinearT(main_.template mapped_key<typename NewLinearT::BasicLinearMain>(func), annotations_);
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

  const BasicLinearMain& main() const { return main_; };
  const BasicLinearAnnotation& annotations() const { return annotations_; };

  Linear& annotate(const std::string& annotation) {
    annotations_ += BasicLinearAnnotation::single(annotation);
    return *this;
  }
  template<typename Arg>
  Linear& annotate_with_function(const std::string& name, const std::vector<Arg>& args) {
    return annotate(name + "(" + str_join(args, ",") + ")");
  }
  void copy_annotations(const Linear& other) {
    annotations_ += other.annotations_;
  }
  Linear without_annotations() const {
    return Linear(main_, {});
  }

  Linear operator+() const {
    return *this;
  }
  Linear operator-() const {
    return Linear() - *this;
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

  Linear& operator+=(const Linear& other) {
    main_ += other.main_;
    annotations_ += other.annotations_;
    return *this;
  }
  Linear& operator-=(const Linear& other) {
    main_ -= other.main_;
    annotations_ -= other.annotations_;
    return *this;
  }
  Linear& operator*=(int scalar) {
    main_ *= scalar;
    annotations_ *= scalar;
    return *this;
  }
  void div_int(int scalar) {
    main_.div_int(scalar);
  }

  bool operator==(const Linear& other) const {
    return main_ == other.main_ && annotations_ == other.annotations_;
  }
  bool operator!=(const Linear& other) const {
    return !(*this == other);
  }

private:
  BasicLinearMain main_;
  BasicLinearAnnotation annotations_;
};

template<typename ParamT>
Linear<ParamT> operator*(int scalar, const Linear<ParamT>& linear) {
  return linear * scalar;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const Linear<ParamT>& linear) {
  if (!linear.zero()) {
    const int size = linear.size();
    os << "# " << size << " " << en_plural(size, "term");
    os << ", |coeff| = " << linear.l1_norm() << ":\n";
    os << linear.main();
  } else {
    os << "\n" << format_coeff(0) << "\n";
  }
  if (!linear.annotations().zero()) {
    os << "~~~\n";
    os << linear.annotations();
  }
  os.flush();
  return os;
}
