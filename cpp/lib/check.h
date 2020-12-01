// TODO: Print argument names from macros like CHECK_EQ.
// TODO: Add debug-only checks and replace all asserts.

#pragma once

#include <cstdlib>
#include <iostream>

#include "absl/base/optimization.h"
#include "string_basic.h"


inline void FATAL [[noreturn]] (const std::string& msg = {}) {
  std::cerr << "Assertion failed";
  if (!msg.empty()) {
    std::cerr << ": " << msg;
  }
  std::cerr << std::endl;
  std::abort();
}


namespace internal {

struct CheckLocation {
  const char* file;
  int line;
};

struct CheckResult {
  CheckResult(bool ok_arg) : ok(ok_arg) {}
  CheckResult(const CheckResult&) = delete;
  CheckResult& operator=(const CheckResult&) = delete;
  CheckResult(CheckResult&&) = delete;
  CheckResult& operator=(CheckResult&&) = delete;
  ~CheckResult() {
    if (ABSL_PREDICT_FALSE(!ok)) {
      std::cerr << std::endl;
      abort();
    }
  }
  const bool ok;
};

inline std::ostream& assertion_failed(CheckLocation loc) {
  return std::cerr << "Assertion failed in " << loc.file << ":" << loc.line << "\n";
}

inline CheckResult check(CheckLocation loc, bool ok) {
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc);
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_eq(CheckLocation loc, const X& x, const Y& y) {
  const bool ok = x == y;
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc) << to_string(x) << " == " << to_string(y) << "\n";
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_ne(CheckLocation loc, const X& x, const Y& y) {
  const bool ok = x != y;
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc) << to_string(x) << " != " << to_string(y) << "\n";
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_lt(CheckLocation loc, const X& x, const Y& y) {
  const bool ok = x < y;
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc) << to_string(x) << " < " << to_string(y) << "\n";
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_le(CheckLocation loc, const X& x, const Y& y) {
  const bool ok = x <= y;
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc) << to_string(x) << " <= " << to_string(y) << "\n";
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_gt(CheckLocation loc, const X& x, const Y& y) {
  const bool ok = x > y;
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc) << to_string(x) << " > " << to_string(y) << "\n";
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_ge(CheckLocation loc, const X& x, const Y& y) {
  const bool ok = x >= y;
  if (ABSL_PREDICT_FALSE(!ok)) assertion_failed(loc) << to_string(x) << " >= " << to_string(y) << "\n";
  return CheckResult(ok);
}
}  // namespace internal


// Use tertiary operator to avoid evaluating arguments when check passes

#define CHECK(condition)  \
  internal::check({__FILE__, __LINE__}, condition).ok ? std::cerr : std::cerr

#define CHECK_EQ(lhs, rhs)  \
  internal::check_eq({__FILE__, __LINE__}, lhs, rhs).ok ? std::cerr : std::cerr
#define CHECK_NE(lhs, rhs)  \
  internal::check_ne({__FILE__, __LINE__}, lhs, rhs).ok ? std::cerr : std::cerr
#define CHECK_LT(lhs, rhs)  \
  internal::check_lt({__FILE__, __LINE__}, lhs, rhs).ok ? std::cerr : std::cerr
#define CHECK_LE(lhs, rhs)  \
  internal::check_le({__FILE__, __LINE__}, lhs, rhs).ok ? std::cerr : std::cerr
#define CHECK_GT(lhs, rhs)  \
  internal::check_gt({__FILE__, __LINE__}, lhs, rhs).ok ? std::cerr : std::cerr
#define CHECK_GE(lhs, rhs)  \
  internal::check_ge({__FILE__, __LINE__}, lhs, rhs).ok ? std::cerr : std::cerr
