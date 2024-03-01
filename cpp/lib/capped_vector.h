#pragma once

#include <array>

#include "check.h"


// Optimization potential: properly use in-place new/delete instead of filling with T().
template<typename T, uint8_t N>
class CappedVector {
public:
  using value_type = T;
  using size_type = uint8_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;

  static constexpr uint8_t kCapacity = N;

  CappedVector() = default;
  CappedVector(std::initializer_list<T> init) : CappedVector(init.begin(), init.end()) {}
  template<class InputIt>
  CappedVector(InputIt first, InputIt last) {
    const auto count = std::distance(first, last);
    CHECK_LE(count, N);
    std::copy(first, last, data_.begin());
    size_ = count;
  }

  static constexpr bool kCanInline = true;

  uint8_t size() const {
    return size_;
  }

  const T& operator[](uint8_t index) const {
    return data_[index];
  }
  T& operator[](uint8_t index) {
    return data_[index];
  }

  const T& at(uint8_t index) const {
    CHECK_LT(index, size_);
    return data_[index];
  }
  T& at(uint8_t index) {
    CHECK_LT(index, size_);
    return data_[index];
  }

  const T& front() const {
    CHECK(!empty());
    return data_[0];
  }
  T& front() {
    CHECK(!empty());
    return data_[0];
  }

  const T& back() const {
    CHECK(!empty());
    return data_[size_ - 1];
  }
  T& back() {
    CHECK(!empty());
    return data_[size_ - 1];
  }

  void push_back(const T& value) {
    CHECK(!full());
    data_[size_++] = value;
  }
  void push_back(T&& value) {
    CHECK(!full());
    data_[size_++] = std::move(value);
  }

  void pop_back() {
    CHECK(!empty());
    back() = T();
    --size_;
  }

  template<class InputIt>
  void insert(const_iterator pos, InputIt first, InputIt last) {
    const auto insert_count = std::distance(first, last);
    CHECK_LE(size_ + insert_count, N);
    const auto insert_pos = pos - begin();
    const auto move_count = size_ - insert_pos;
    if (move_count > 0) {
      std::move_backward(begin() + insert_pos, end(), end() + insert_count);
    }
    std::copy(first, last, begin() + insert_pos);
    size_ += insert_count;
  }

  void reserve(uint8_t) {}

  void clear() {
    for (auto& item : data_) {
      item = T();
    }
    size_ = 0;
  }

  bool empty() const {
    return size_ == 0;
  }

  bool full() const {
    return size_ == N;
  }

  const T* data() const {
    return data_.data();
  }
  T* data() {
    return data_.data();
  }

  const T* begin() const {
    return data_.begin();
  }
  T* begin() {
    return data_.begin();
  }

  const T* end() const {
    return data_.begin() + size_;
  }
  T* end() {
    return data_.begin() + size_;
  }

  bool operator< (const CappedVector& other) const { return absl::c_lexicographical_compare(*this, other); }
  bool operator==(const CappedVector& other) const { return absl::c_equal(*this, other); }

  template <typename H>
  friend H AbslHashValue(H h, const CappedVector& vec) {
    return H::combine(std::move(h), absl::MakeConstSpan(vec));
  }

private:
  uint8_t size_ = 0;
  std::array<T, N> data_;
};

static_assert(sizeof(CappedVector<char, 1>) == 2);

template<typename T, uint8_t N>
std::string dump_to_string_impl(const CappedVector<T, N>& v) {
  return internal::dump_vector_like_to_string(v);
}
