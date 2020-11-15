#pragma once

#include "util.h"


template<typename T>
class CircularView {
public:
  class iterator {
  public:
    const T& operator*() const {
      return data_[pos_ % size_];
    }

    iterator& operator++() {
      ++pos_;
      return *this;
    };

    bool operator==(const iterator& other) const {
      return data_ == other.data_ && size_ == other.size_ && pos_ == other.pos_;
    }
    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

  private:
    iterator(T* const data, int size, int pos)
      : data_(data), size_(size), pos_(pos) {
      CHECK_LT(0, size_);
      CHECK_LE(0, pos_);
      CHECK_LT(pos_, size_);
    }

    const T* data_ = nullptr;
    int size_ = 0;
    int pos_ = 0;
  };

  CircularView() = default;
  template<typename IteratorT>
  CircularView(const IteratorT& begin, const IteratorT& end, int start = 0)
    : CircularView(&*begin, end - begin, start) {}
  template<typename ContainerT>
  CircularView(const ContainerT& container, int start = 0)
    : CircularView(container.begin(), container.end(), start) {}

  int size() const { return size_; }
  int start() const { return start_; }

  const T& operator[](int idx) const { return data_[pos_mod(start_ + idx, size_)]; }

  // value can be positive or negative
  CircularView rotated(int value) const {
    return CircularView(data_, size_, start_ + value);
  }

  iterator begin() { return iterator(data_, size_, start_); }
  iterator end() { return iterator(data_, size_, start_ + size_); }

private:
  CircularView(const T* data, int size, int start)
    : data_(data), size_(size), start_(pos_mod(start, size_)) {}

  const T* data_ = nullptr;
  int size_ = 0;
  int start_ = 0;
};
