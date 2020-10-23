#pragma once

#include <cstdlib>
#include <iostream>

#include "format_basic.h"


inline void FAIL(const std::string& msg = {}) {
  std::cerr << "Assertion failed";
  if (!msg.empty()) {
    std::cerr << ": " << msg;
  }
  std::cerr << "\n";
  std::abort();
}

// TODO: Macros that print names
// TODO: Include file and line number
// TODO: Include stacktrace
// TODO: Debug-only checks and replace all asserts
// TODO: Annotations via operator<<
inline void CHECK(bool cond, const std::string& msg = {}) {
  if (!cond) {
    FAIL(msg);
  }
}

template<typename X, typename Y>
inline void CHECK_EQ(X&& x, Y&& y) {
  if (!(x == y)) {
    FAIL(to_string(x) + " == " + to_string(y));
  }
}

template<typename X, typename Y>
inline void CHECK_NE(X&& x, Y&& y) {
  if (!(x != y)) {
    FAIL(to_string(x) + " != " + to_string(y));
  }
}

template<typename X, typename Y>
inline void CHECK_LT(X&& x, Y&& y) {
  if (!(x < y)) {
    FAIL(to_string(x) + " < " + to_string(y));
  }
}

template<typename X, typename Y>
inline void CHECK_GT(X&& x, Y&& y) {
  if (!(x > y)) {
    FAIL(to_string(x) + " > " + to_string(y));
  }
}

template<typename X, typename Y>
inline void CHECK_LE(X&& x, Y&& y) {
  if (!(x <= y)) {
    FAIL(to_string(x) + " <= " + to_string(y));
  }
}

template<typename X, typename Y>
inline void CHECK_GE(X&& x, Y&& y) {
  if (!(x >= y)) {
    FAIL(to_string(x) + " >= " + to_string(y));
  }
}
