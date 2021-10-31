#pragma once


// Automatically chooses function overload depending on the context.
#define DISAMBIGUATE(func)  \
    [](auto&&... args) { return func(std::forward<decltype(args)>(args)...); }

// Converts a function of multiple arguments into a function of a tuple.
template<typename F>
auto applied(const F& func) {
  // TODO: Add perfect forwarding for `func`. Check if this would do:
  //   [func = std::forward<F>(func)](...) { ... }
  return [func](auto&& tuple) {
    return std::apply(func, std::forward<decltype(tuple)>(tuple));
  };
}
