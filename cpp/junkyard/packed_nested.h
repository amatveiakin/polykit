// TODO: What about unaligned data access?

// Optimization potential: tighten the limits and support heap
//   representation for outliers. Need to be careful not to let
//   everything fall into heap and slow down the program tenfold.
//   Should probably have a constant monitoring for heap elements
//   and alert the user if a certain threshold is exceeded.

#pragma once

#include <array>
#include <climits>
#include <cstddef>
#include <limits>

#include "absl/algorithm/container.h"
#include "absl/types/span.h"

#include "check.h"


template<typename T>
struct PElementTraits<T> {};  // unsupported by default

template<>
struct PElementTraits<uint4_t> {
  static constexpr bool fixed_size = true;
};

// TODO: What about other intergal types?

template<int N>
struct PElementTraits<std::bitset<N>> {
  static constexpr bool fixed_size = true;
};

template<typename... Args>
struct PElementTraits<std::variant<Args...>> {
  static constexpr bool fixed_size = (PElementTraits<Args>::fixed_size && ...);
};

template<typename T>
struct PElementTraits<PArray<T>> {
  static constexpr bool fixed_size = PElementTraits<T>::fixed_size;
};

template<typename T>
struct PElementTraits<PVector<T>> {
  // Check `PElementTraits<T>::fixed_size` existence to see it `T` is supported.
  static constexpr bool fixed_size = PElementTraits<T>::fixed_size && false;
};



// General concept. End-type is a POD (in practice: int or bitset),
// everything else is always flattened, so that we can compute equality
// via memcmp and hash quickly.

// Limitations:
//   * Size in bytes is <= 256
//   * Size in elements is <= 256 (note: doesn't follow from above for int4)

// Element types:
//   * fixed size, classic
//   * fixed size, packed (used for ints)
//   * variable size (cannot be packed)


// PVector data model, element is fixed size
//   * size in elements
//   * content

// PVector data model, element is variable size
//   * size in elements
//   * offsets
//   * content


// PArray data model, element is fixed size
//   * content

// PArray data model, element is variable size
//   * offsets
//   * content



template<typename T, int StorgeBytes>
class PVector {
  // Should choose other overloaded for variable-size elements.
  static_assert(PElementTraits<T>::fixed_size);

public:
  PVector() {}

  bool empty() const { return size() == 0; }
  int size() const { return size_; }

  // // Note: there is no way to change elements. This would be impossible
  // // for PVector<PVector>: data is packed, so pushing a push_back on an
  // // element in the middle would be very expensive and complicated.
  // const T& operator[](int idx) const { return data_[idx]; }
  // const T& at(int idx) const         { return data_.at(idx); }

  void push_back(T value) {
    int old_size = size();
    write_size(old_size + 1);
    data_[old_size] = std::move(value);
  }
  void pop_back() {
    int new_size = size() - 1;
    write_size(new_size);
    data_[new_size] = T{};
  }

  auto begin() const { return data_.begin(); }
  auto end() const { return data_.begin() + size(); }

  bool operator==(const PVector& other) const { return rep_.bytes == other.rep_.bytes; }
  bool operator< (const PVector& other) const { return rep_.bytes <  other.rep_.bytes; }

private:
  static constexpr kMaxElements = ;

  void write_size(int new_size) {
    CHECK_LE(0, new_size);
    CHECK_LT(new_size, kMaxElements);
    size_ = new_size;
  }

  unsigned char size_ = 0;
  std::array<T, kMaxElements> data_;
};


template<typename T>
class PSpan {
public:
  // ...

private:
  T* begin_;
  T* end_;
};


template<typename T>
class PSpan<PSpan<T>> {
public:
  // ...

private:
  T* begin_;
  T* end_;
};


template<typename T, int StorgeBytes>
class PVector<PSpan<T>, StorgeBytes> {
public:
  PVector() {}

  bool empty() const { return size() == 0; }
  int size() const { return num_elements_; }

  template<int InnerStorage>
  void push_back(const PVector<T, InnerStorage>& value) {
    auto old_end = end_internal();
    int old_size = size();
    write_size(old_size + 1, data_size_ + value.data_size());
    old_end->size = value.size();
    ++old_end;
    absl::c_copy(value, old_end);
  }

private:
  static constexpr kMaxElements = ;

  auto end_internal() {
    return data_.begin() + data_size_;
  }
  void write_size(int new_num_elements, int new_data_size) {
    CHECK_LE(0, new_num_elements);
    CHECK_LT(new_num_elements, kMaxElements);
    CHECK_LE(0, new_data_size);
    CHECK_LT(new_data_size, kMaxElements);
    num_elements_ = new_num_elements;
    data_size_ = new_data_size;
  }

  union ElementT {
    unsigned char size = 0;
    T data;
  };

  unsigned char num_elements_;
  unsigned char data_size_;
  std::array<ElementT, N> data_;
};
