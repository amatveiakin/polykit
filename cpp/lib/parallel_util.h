#pragma once

#include <execution>
#include <future>

#include "util.h"


#ifndef PARALLELISM_IMPLEMENTATION
#  define PARALLELISM_IMPLEMENTATION 1
#endif

#if PARALLELISM_IMPLEMENTATION == 0

template<typename Src, typename F>
auto mapped_parallel(const Src& src, F&& func) {
  return mapped(src, std::forward<F>(func));
}

#elif PARALLELISM_IMPLEMENTATION == 1
// This requires Threading Building Blocks ("-ltbb") with libstdc++, i.e. with gcc/clang builds.
// TODO: Link TBB via Bazel.
template<typename Src, typename F>
auto mapped_parallel(const Src& src, F&& func) {
  std::vector<std::decay_t<std::invoke_result_t<F, typename Src::value_type>>> dst(src.size());
  std::transform(std::execution::par_unseq, src.begin(), src.end(), dst.begin(), std::forward<F>(func));
  return dst;
}

#elif PARALLELISM_IMPLEMENTATION == 2
// This crashed with std::bad_alloc on large input.
template<typename Src, typename F>
auto mapped_parallel(const Src& src, const F& func) {
  auto futures = mapped(src, [func](const auto& x) {
    return std::async([func, x](){ return func(x); });
  });
  return mapped_mutable(futures, [](auto& fut) {
    return fut.get();
  });
}

#else
#  error Unsupported PARALLELISM_IMPLEMENTATION
#endif
