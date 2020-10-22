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


struct LinearParamAnnotation {
  using ObjectT = std::string;
  using StorageT = std::string;
  static StorageT object_to_key(const ObjectT& obj) { return obj; }
  static ObjectT key_to_object(const StorageT& key) { return key; }
  static std::string object_to_string(const ObjectT& obj) { return obj; }
};


template<typename ParamT>
class BasicLinear {
public:
  using ObjectT = typename ParamT::ObjectT;
  using StorageT = typename ParamT::StorageT;

  BasicLinear() {}

  static BasicLinear single(const ObjectT& obj) {
    return single_key(ParamT::object_to_key(obj));
  }
  static BasicLinear single_key(const StorageT& key) {
    BasicLinear ret;
    ret.data_[key] = 1;
    return ret;
  }

  bool empty() const { return data_.empty(); }
  int size() const { return data_.size(); }

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

  template<typename F>
  void foreach(F func) const {
    foreach_key([&func](const auto& key, int coeff){
      func(ParamT::key_to_object(key), coeff);
    });
  }
  template<typename F>
  void foreach_key(F func) const {
    for (const auto& [key, coeff]: data_) {
      func(key, coeff);
    }
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
std::ostream& operator<<(std::ostream& os, const BasicLinear<ParamT>& linear)
{
  std::vector<std::pair<typename ParamT::ObjectT, int>> dump;
  linear.foreach([&dump](const auto& obj, int coeff) {
    dump.push_back({obj, coeff});
  });
  std::sort(dump.begin(), dump.end());
  for (const auto& [obj, coeff] : dump) {
    os << format_coeff(coeff) << ParamT::object_to_string(obj) << "\n";
  }
  return os;
}

using BasicLinearAnnotation = BasicLinear<LinearParamAnnotation>;


template<typename ParamT>
class Linear {
public:
  using ObjectT = typename ParamT::ObjectT;
  using StorageT = typename ParamT::StorageT;
  using BasicLinearMain = BasicLinear<ParamT>;

  Linear() {}
  Linear(BasicLinear<ParamT> main) : main_(main) {}

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

  const BasicLinearMain& main() const { return main_; };
  const BasicLinearAnnotation& annotations() const { return annotations_; };

  Linear& annotate(const std::string& annotation) {
    annotations_ += BasicLinearAnnotation::single(annotation);
    return *this;
  }
  template<typename Arg>
  Linear& annotate_with_function(const std::string& name, const std::vector<Arg>& args) {
    return annotate(name + "(" + str_join(args, ", ") + ")");
  }
  void copy_annotations(const Linear& other) {
    annotations_ += other.annotations_;
  }
  Linear without_annotations() const {
    return Linear(main_);
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

private:
  BasicLinearMain main_;
  BasicLinearAnnotation annotations_;
};

template<typename ParamT>
Linear<ParamT> operator*(int scalar, const Linear<ParamT>& linear) {
  return linear * scalar;
}

template<typename ParamT>
std::ostream& operator<<(std::ostream& os, const Linear<ParamT>& linear)
{
  if (!linear.annotations().empty()) {
    os << linear.annotations() << "=>\n";
  }
  os << linear.main();
  return os;
}
