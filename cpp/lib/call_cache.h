#pragma once

#include "absl/container/flat_hash_map.h"


// Sample usage:
//    return QLi_impl(weight, asc_points);
// =>
//    static CallCache<DeltaExpr, int, std::vector<int>> call_cache;
//    return call_cache.apply(&QLi_impl, weight, asc_points);
//
template<typename Value, typename... Args>
class CallCache {
public:
  template<typename F>
  Value apply(F&& func, Args... args) {
    const std::tuple<Args...> args_tuple{std::move(args)...};
    const auto it = cache_.find(args_tuple);
    if (it != cache_.end()) {
      return it->second;
    } else {
      Value ret = std::apply(std::forward<F>(func), args_tuple);
      cache_[args_tuple] = ret;
      return ret;
    }
  }

private:
  absl::flat_hash_map<std::tuple<Args...>, Value> cache_;
};
