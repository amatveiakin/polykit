#pragma once

#include <array>
#include <vector>

#include "absl/algorithm/container.h"

#include "metaprogramming.h"


enum class SumSign {
  plus,
  alternating,
};

enum class SumStep : int;  // a simple strong int

// Idea. A builder interface instead of overloads, e.g.
//   * sum_looped(f).max_arg_value(8).starting_args({1,2,3,4}).compute();
//   * sum_looped(f, SumOptions{}.max_arg_value(8).starting_args({1,2,3,4});

template<typename F>
auto sum_looped_vec(
    const F& func,
    int max_arg_value,
    const std::vector<int>& starting_args,
    SumSign summation_sign = SumSign::plus,
    SumStep summation_step = SumStep(1)) {
  int step = static_cast<int>(summation_step);
  CHECK_GE(step, 1);
  CHECK(max_arg_value % step == 0)
    << "The sum is suspiciously non-symmetric: max_arg_value = " << max_arg_value
    << ", step = " << step << ". If you really want this, feel free to remove this check.";
  std::invoke_result_t<F, std::vector<X>> ret;
  for (int shift = 0; shift < max_arg_value; shift += step) {
    std::vector<X> args;
    for (const int a : starting_args) {
      CHECK_LE(1, a);
      CHECK_LE(a, max_arg_value);
      args.push_back((a - 1 + shift) % max_arg_value + 1);
    }
    int sign = 0;
    switch (summation_sign) {
      case SumSign::plus: sign = 1; break;
      case SumSign::alternating: sign = neg_one_pow(shift); break;
    }
    ret += sign * func(args);
  }
  return ret;
}

template<typename F, size_t N>
auto sum_looped(
    const F& func,
    int max_arg_value,
    const std::array<int, N>& starting_args,
    SumSign summation_sign = SumSign::plus,
    SumStep summation_step = SumStep(1)) {
  return sum_looped_vec(
    [&](const std::vector<X> args) {
      return std::apply(func, to_array<N>(args));
    },
    max_arg_value, to_vector(starting_args), summation_sign, summation_step
  );
}

template<typename F>
auto sum_looped(
    const F& func,
    int max_arg_value) {
  std::array<int, get_arity<F>{}> starting_args;
  absl::c_iota(starting_args, 1);
  return sum_looped(func, max_arg_value, starting_args);
}
