#pragma once

#include "check.h"


class Range {
public:
  class Iterator {
  public:
    Iterator(int value) : value_(value) {}
    Iterator& operator++() { ++value_; return *this; }
    int operator*() const { return value_; }
    bool operator==(Iterator other) { return value_ == other.value_; }
    bool operator!=(Iterator other) { return value_ != other.value_; }
  private:
    int value_;
  };

  Range(int from, int to) : from_(from), to_(to) {
    if (from_ > to_) {
      from_ = to_;
    }
  }

  Iterator begin() const { return Iterator(from_); }
  Iterator end()   const { return Iterator(to_); }

private:
  int from_;
  int to_;
};

inline Range range(int to) {
  return Range(0, to);
}
inline Range range(int from, int to) {
  return Range(from, to);
}

inline Range range_incl(int to) {
  return Range(0, to + 1);
}
inline Range range_incl(int from, int to) {
  return Range(from, to + 1);
}


// Usage:
//   for (EACH : range(...)) { ... }
#ifdef __GNUC__
#  define EACH  __attribute__((unused)) auto&& _loop_counter_
#else
#  define EACH  auto&& _loop_counter_
#endif
