#pragma once

#include "absl/algorithm/container.h"


namespace cmp {

template<typename T>
class ascending_value_wrapper {
public:
  ascending_value_wrapper(T value) : value_(std::move(value)) {}

  bool operator==(const ascending_value_wrapper& other) const { return value_ == other.value_; }
  bool operator!=(const ascending_value_wrapper& other) const { return value_ != other.value_; }
  bool operator< (const ascending_value_wrapper& other) const { return value_ <  other.value_; }
  bool operator<=(const ascending_value_wrapper& other) const { return value_ <= other.value_; }
  bool operator> (const ascending_value_wrapper& other) const { return value_ >  other.value_; }
  bool operator>=(const ascending_value_wrapper& other) const { return value_ >= other.value_; }

private:
  T value_;
};

template<typename T>
class descending_value_wrapper {
public:
  descending_value_wrapper(T value) : value_(std::move(value)) {}

  bool operator==(const descending_value_wrapper& other) const { return value_ == other.value_; }
  bool operator!=(const descending_value_wrapper& other) const { return value_ != other.value_; }
  bool operator< (const descending_value_wrapper& other) const { return value_ >  other.value_; }
  bool operator<=(const descending_value_wrapper& other) const { return value_ >= other.value_; }
  bool operator> (const descending_value_wrapper& other) const { return value_ <  other.value_; }
  bool operator>=(const descending_value_wrapper& other) const { return value_ <= other.value_; }

private:
  T value_;
};

template<typename T>
class ascending_reference_wrapper : private std::reference_wrapper<const T> {
public:
  using std::reference_wrapper<const T>::reference_wrapper;

  bool operator==(const ascending_reference_wrapper& other) const { return this->get() == other.get(); }
  bool operator!=(const ascending_reference_wrapper& other) const { return this->get() != other.get(); }
  bool operator< (const ascending_reference_wrapper& other) const { return this->get() <  other.get(); }
  bool operator<=(const ascending_reference_wrapper& other) const { return this->get() <= other.get(); }
  bool operator> (const ascending_reference_wrapper& other) const { return this->get() >  other.get(); }
  bool operator>=(const ascending_reference_wrapper& other) const { return this->get() >= other.get(); }
};

template<typename T>
class descending_reference_wrapper : private std::reference_wrapper<const T> {
public:
  using std::reference_wrapper<const T>::reference_wrapper;

  bool operator==(const descending_reference_wrapper& other) const { return this->get() == other.get(); }
  bool operator!=(const descending_reference_wrapper& other) const { return this->get() != other.get(); }
  bool operator< (const descending_reference_wrapper& other) const { return this->get() >  other.get(); }
  bool operator<=(const descending_reference_wrapper& other) const { return this->get() >= other.get(); }
  bool operator> (const descending_reference_wrapper& other) const { return this->get() <  other.get(); }
  bool operator>=(const descending_reference_wrapper& other) const { return this->get() <= other.get(); }
};

// `asc_val` / `desc_val` / `asc_ref` / `desc_ref` allow to compare tuples lexicographically
//   while choosing ascending or descending order separately for each field.
// Example:
//   auto compare = cmp::projected([](const std::pair<Foo, Bar>& x) {
//     return std::tuple{cmp::asc_ref(x.first), cmp::desc_ref(x.second)};
//   });
// If all fields compare via `cmp::asc_ref` then a simple `std::tie` can be used instead.

template<typename T>
auto asc_val(T v) { return ascending_value_wrapper<T>(std::move(v)); }

template<typename T>
auto desc_val(T v) { return descending_value_wrapper<T>(std::move(v)); }

template<typename T>
auto asc_ref(const T& v) noexcept { return ascending_reference_wrapper<T>(v); }
template<typename T>
void asc_ref(const T&&) = delete;

template<typename T>
auto desc_ref(const T& v) noexcept { return descending_reference_wrapper<T>(v); }
template<typename T>
void desc_ref(const T&&) = delete;


template<typename T, typename Projector>
bool projected(const T& a, const T& b, const Projector& projector) {
  return projector(a) < projector(b);
}


template<typename F>
auto greater_from_less(const F& less) {
  return [less](const auto& a, const auto& b) -> bool {
    return less(b, a);
  };
}

template<typename F>
auto lexicographical(const F& comp) {
  return [comp](const auto& a, const auto& b) -> bool {
    return absl::c_lexicographical_compare(a, b, comp);
  };
}

template<typename Projector>
auto projected(const Projector& projector) {
  return [projector](const auto& a, const auto& b) -> bool {
    return projected(a, b, projector);
  };
}

}  // namespace cmp
