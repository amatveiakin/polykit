#pragma once

#include <future>

#include "util.h"


template<typename Src, typename F>
auto mapped_parallel(const Src& src, const F& func) {
  // A more natural and likely faster way to implement this would be
  //   std::transform(std::execution::par_unseq, src.begin(), src.end(), dst.begin(), std::forward<F>(func));
  // But it's not supported by clang as at Sep 2021 (https://en.cppreference.com/w/cpp/compiler_support, P0024R2)
  // Note that `std::execution` requires linking Threading Building Blocks ("-ltbb").

  auto futures = mapped(src, [func](const auto& x) {
    return std::async([func, x](){ return func(x); });
  });
  return mapped_mutable(futures, [](auto& fut) {
    return fut.get();
  });
}
