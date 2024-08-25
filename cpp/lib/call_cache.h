#pragma once

#include "absl/container/flat_hash_map.h"


// Workaround a bug. After upgrading to C++23 using an `std::tuple` directly fails to compile with:
//
//   /usr/lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:691:2: error: pack expansion contains parameter pack '_UTypes' that has a different length (1 vs. 2) from outer parameter packs
//     691 |         using __convertible = __and_<is_convertible<_UTypes, _Types>...>;
//         |         ^~~~~
//   /usr/lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:853:27: note: in instantiation of template type alias '__convertible' requested here
//     853 |           = _TCC<true>::template __convertible<_Args...>::value;
//         |                                  ^
//   /usr/lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:948:12: note: in instantiation of static data member 'std::tuple<const std::tuple<int, int> &>::__convertible<int &, int &>' requested here
//     948 |         explicit(!__convertible<_UElements&...>)
//         |                   ^
//   /usr/lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:2014:36: note: while substituting deduced template arguments into function template 'tuple' [with _UElements = <int, int>]
//    2014 |     { return tuple<_Elements&&...>(std::forward<_Elements>(__args)...); }
//         |                                    ^
//   external/abseil-cpp~/absl/container/internal/container_memory.h:194:42: note: in instantiation of function template specialization 'std::forward_as_tuple<const std::tuple<int, int> &>' requested here
//     194 |   return {std::piecewise_construct, std::forward_as_tuple(std::forward<F>(f)),
//         |                                          ^
//   external/abseil-cpp~/absl/container/internal/container_memory.h:200:10: note: in instantiation of function template specialization 'absl::container_internal::PairArgs<const std::tuple<int, int> &, const Linear<internal::GammaExprParam> &>' requested here
//     200 |   return PairArgs(p.first, p.second);
//         |          ^
//   external/abseil-cpp~/absl/container/internal/container_memory.h:220:27: note: (skipping 2 contexts in backtrace; use -ftemplate-backtrace-limit=0 to see all)
//     220 |       std::forward<F>(f), PairArgs(std::forward<Args>(args)...));
//         |                           ^
//   external/abseil-cpp~/absl/container/internal/hash_policy_traits.h:134:15: note: in instantiation of function template specialization 'absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>::apply<absl::container_internal::raw_hash_set<absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>, absl::hash_internal::Hash<std::tuple<int, int>>, std::equal_to<std::tuple<int, int>>, std::allocator<std::pair<const std::tuple<int, int>, Linear<internal::GammaExprParam>>>>::EqualElement<std::tuple<int, int>>, std::pair<const std::tuple<int, int>, Linear<internal::GammaExprParam>> &>' requested here
//     134 |     return P::apply(std::forward<F>(f), std::forward<Ts>(ts)...);
//         |               ^
//   external/abseil-cpp~/absl/container/internal/raw_hash_set.h:3496:38: note: in instantiation of function template specialization 'absl::container_internal::hash_policy_traits<absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>>::apply<absl::container_internal::raw_hash_set<absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>, absl::hash_internal::Hash<std::tuple<int, int>>, std::equal_to<std::tuple<int, int>>, std::allocator<std::pair<const std::tuple<int, int>, Linear<internal::GammaExprParam>>>>::EqualElement<std::tuple<int, int>>, std::pair<const std::tuple<int, int>, Linear<internal::GammaExprParam>> &, absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>>' requested here
//    3496 |     return empty() || !PolicyTraits::apply(EqualElement<K>{key, eq_ref()},
//         |                                      ^
//   external/abseil-cpp~/absl/container/internal/raw_hash_set.h:3335:26: note: in instantiation of function template specialization 'absl::container_internal::raw_hash_set<absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>, absl::hash_internal::Hash<std::tuple<int, int>>, std::equal_to<std::tuple<int, int>>, std::allocator<std::pair<const std::tuple<int, int>, Linear<internal::GammaExprParam>>>>::find_soo<std::tuple<int, int>>' requested here
//    3335 |     if (is_soo()) return find_soo(key);
//         |                          ^
//   cpp/lib/call_cache.h:57:28: note: in instantiation of function template specialization 'absl::container_internal::raw_hash_set<absl::container_internal::FlatHashMapPolicy<std::tuple<int, int>, Linear<internal::GammaExprParam>>, absl::hash_internal::Hash<std::tuple<int, int>>, std::equal_to<std::tuple<int, int>>, std::allocator<std::pair<const std::tuple<int, int>, Linear<internal::GammaExprParam>>>>::find<std::tuple<int, int>>' requested here
//      57 |     const auto it = cache_.find(arg);
//         |                            ^
//   cpp/lib/polylog_gli.cpp:76:15: note: in instantiation of function template specialization 'CallCache<Linear<internal::GammaExprParam>, std::tuple<int, int>>::apply<const std::function<Linear<internal::GammaExprParam> (std::tuple<int, int>)> &>' requested here
//      76 |         cache.apply(impl, std::tuple<int, int>{row-1, col})
//         |  template<typename... Args>
//
// Testes in clang 17.0.6.
// Looks like a failure of absl to support C++23.
// TODO: Got back to using `std::tuple` directly if it's fixed.
template<typename... Args>
struct OpaqueTuple {
  OpaqueTuple(Args... args) : tuple(args...) {}

  auto operator<=>(const OpaqueTuple&) const = default;

  template<typename H>
  friend H AbslHashValue(H h, const OpaqueTuple& opaque_tuple) {
    return H::combine(std::move(h), opaque_tuple.tuple);
  }

  std::tuple<Args...> tuple;
};

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
    const OpaqueTuple<Args...> args_tuple{args...};
    const auto it = cache_.find(args_tuple);
    if (it != cache_.end()) {
      return it->second;
    } else {
      Value ret = std::forward<F>(func)(args...);
      cache_[args_tuple] = ret;
      return ret;
    }
  }

private:
  absl::flat_hash_map<OpaqueTuple<Args...>, Value> cache_;
};
