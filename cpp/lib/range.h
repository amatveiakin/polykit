#pragma once

#include "check.h"


class Range {
public:
  using value_type = int;
  using size_type = value_type;

  // TODO: Make this a random access iterator.
  // TODO: It is fine that `pointer` and `reference` are not defined?
  class Iterator {
  public:
    using difference_type = int;
    using value_type = int;
    using pointer = void;
    using reference = void;
    using iterator_category = std::forward_iterator_tag;

    Iterator(value_type value) : value_(value) {}
    Iterator& operator++() { ++value_; return *this; }
    value_type operator*() const { return value_; }
    bool operator==(Iterator other) { return value_ == other.value_; }
    bool operator!=(Iterator other) { return value_ != other.value_; }

  private:
    value_type value_;
  };

  Range(value_type from, value_type to) : from_(from), to_(to) {
    if (from_ > to_) {
      from_ = to_;
    }
  }

  size_type size() const { return to_ - from_; }

  Iterator begin() const { return Iterator(from_); }
  Iterator end()   const { return Iterator(to_); }

private:
  value_type from_;
  value_type to_;
};

inline Range range(int from, int to) {
  return Range(from, to);
}
inline Range range(int to) {
  return Range(0, to);
}

inline Range range_incl(int from, int to) {
  return Range(from, to + 1);
}
inline Range range_incl(int to) {
  return Range(0, to + 1);
}

inline std::vector<int> seq(int from, int to) {
  const auto r = range(from, to);
  return std::vector(r.begin(), r.end());
}
inline std::vector<int> seq(int to) {
  return seq(0, to);
}

inline std::vector<int> seq_incl(int from, int to) {
  const auto r = range_incl(from, to);
  return std::vector(r.begin(), r.end());
}
inline std::vector<int> seq_incl(int to) {
  return seq_incl(0, to);
}

// Usage:
//   for (EACH : range(...)) { ... }
#define EACH  [[maybe_unused]] auto&& _loop_counter_
