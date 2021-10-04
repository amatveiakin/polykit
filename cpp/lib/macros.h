#pragma once


#define INTERNAL_STRINGIFY(s) #s
#define STRINGIFY(s) INTERNAL_STRINGIFY(s)

// TODO: Move `DISAMBIGUATE` and `APPLY` to an appropriate header.
// Automatically chooses function overload depending on the context.
#define DISAMBIGUATE(func)  \
    [](auto&&... args) { return func(std::forward<decltype(args)>(args)...); }

// TODO: Make this a function rather than a macro.
// Wraps a function accepting multiple arguments to accept a tuple.
#define APPLY(func)  \
    [](auto&& tuple) { return std::apply(func, std::forward<decltype(tuple)>(tuple)); }
