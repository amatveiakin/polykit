#pragma once

#include <cstdlib>
#include <iostream>

#include "absl/base/optimization.h"

#include "macros.h"
#include "string_basic.h"


inline void FATAL [[noreturn]] (const std::string& msg = {}) {
  std::cerr << "\nAssertion failed";
  if (!msg.empty()) {
    std::cerr << ": " << msg;
  }
  std::cerr << std::endl;
  std::abort();
}

// Usage:
//   switch (v) {
//     ...  // handle all enum values
//   };
//   FATAL_BAD_ENUM(v);
template<typename T>
inline void FATAL_BAD_ENUM [[noreturn]] (const T& value, const std::string& msg = {}) {
  std::cerr << "\nIllegal " << get_type_name<T>() << " value == " << static_cast<int>(value);
  if (!msg.empty()) {
    std::cerr << ": " << msg;
  }
  std::cerr << std::endl;
  std::abort();
}

// Usage:
//   SWITCH_ENUM_OR_DIE(condition, {
//     ...
//   });
// TODO: Why does putting `CHECK`s inside `SWITCH_ENUM_OR_DIE` triggers "-Wunused-value" in clang?
#define SWITCH_ENUM_OR_DIE(value, body)  \
  do {  \
    switch (value) body  \
    FATAL_BAD_ENUM(value);  \
  } while (0)

#define SWITCH_ENUM_OR_DIE_WITH_CONTEXT(value, context, body)  \
  do {  \
    switch (value) body  \
    FATAL_BAD_ENUM(value, context);  \
  } while (0)


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
  return std::cerr << "\nAssertion failed in " << loc.file << ":" << loc.line << "\n";
}
template<typename X, typename Y>
CheckResult binary_assertion(
    CheckLocation loc, const char* operation, bool ok,
    const X& x, const Y& y, const char* x_str, const char* y_str) {
  if (ABSL_PREDICT_FALSE(!ok)) {
    assertion_failed(loc)
        << to_string(x) << " " << operation << " " << to_string(y) << "\n"
        << "  a.k.a.\n"
        << x_str << " " << operation << " " << y_str << "\n";
  }
  return CheckResult(ok);
}

inline CheckResult check(CheckLocation loc, bool ok, const char* arg_str) {
  if (ABSL_PREDICT_FALSE(!ok)) {
    assertion_failed(loc) << arg_str << "\n";
  }
  return CheckResult(ok);
}
template<typename X, typename Y>
CheckResult check_eq(CheckLocation loc, const X& x, const Y& y, const char* x_str, const char* y_str) {
  return binary_assertion(loc, "==", x == y, x, y, x_str, y_str);
}
template<typename X, typename Y>
CheckResult check_ne(CheckLocation loc, const X& x, const Y& y, const char* x_str, const char* y_str) {
  return binary_assertion(loc, "!=", x != y, x, y, x_str, y_str);
}
template<typename X, typename Y>
CheckResult check_lt(CheckLocation loc, const X& x, const Y& y, const char* x_str, const char* y_str) {
  return binary_assertion(loc, "<", x < y, x, y, x_str, y_str);
}
template<typename X, typename Y>
CheckResult check_le(CheckLocation loc, const X& x, const Y& y, const char* x_str, const char* y_str) {
  return binary_assertion(loc, "<=", x <= y, x, y, x_str, y_str);
}
template<typename X, typename Y>
CheckResult check_gt(CheckLocation loc, const X& x, const Y& y, const char* x_str, const char* y_str) {
  return binary_assertion(loc, ">", x > y, x, y, x_str, y_str);
}
template<typename X, typename Y>
CheckResult check_ge(CheckLocation loc, const X& x, const Y& y, const char* x_str, const char* y_str) {
  return binary_assertion(loc, ">=", x >= y, x, y, x_str, y_str);
}
}  // namespace internal


// Use tertiary operator to avoid evaluating arguments when check passes

// TODO: To fix "-Wunused-value" clang warning when using inside a macro (like SWITCH_ENUM_OR_DIE):
//   - Add `<< ""` to the fail branch of the ternary operator, or
//   - Return `std::cerr` from a function
//   (https://stackoverflow.com/questions/69415046/clang-issues-wunused-value-depending-on-whether-the-code-is-called-from-a-macro)
// TODO: File the clang bug.

#define CHECK(condition)  \
  internal::check({__FILE__, __LINE__}, condition, STRINGIFY(condition)).ok ? std::cerr : std::cerr

#define CHECK_EQ(lhs, rhs)  \
  internal::check_eq({__FILE__, __LINE__}, lhs, rhs, STRINGIFY(lhs), STRINGIFY(rhs)).ok ? std::cerr : std::cerr
#define CHECK_NE(lhs, rhs)  \
  internal::check_ne({__FILE__, __LINE__}, lhs, rhs, STRINGIFY(lhs), STRINGIFY(rhs)).ok ? std::cerr : std::cerr
#define CHECK_LT(lhs, rhs)  \
  internal::check_lt({__FILE__, __LINE__}, lhs, rhs, STRINGIFY(lhs), STRINGIFY(rhs)).ok ? std::cerr : std::cerr
#define CHECK_LE(lhs, rhs)  \
  internal::check_le({__FILE__, __LINE__}, lhs, rhs, STRINGIFY(lhs), STRINGIFY(rhs)).ok ? std::cerr : std::cerr
#define CHECK_GT(lhs, rhs)  \
  internal::check_gt({__FILE__, __LINE__}, lhs, rhs, STRINGIFY(lhs), STRINGIFY(rhs)).ok ? std::cerr : std::cerr
#define CHECK_GE(lhs, rhs)  \
  internal::check_ge({__FILE__, __LINE__}, lhs, rhs, STRINGIFY(lhs), STRINGIFY(rhs)).ok ? std::cerr : std::cerr


#ifndef ENABLE_ASSERTIONS
#  ifdef NDEBUG
#    define ENABLE_ASSERTIONS 0
#  else
#    define ENABLE_ASSERTIONS 1
#  endif
#endif

#undef ASSERT
#if ENABLE_ASSERTIONS
#  define ASSERT(condition)  CHECK(condition)
#else
#  define ASSERT(condition)  ((void)0)
#endif
