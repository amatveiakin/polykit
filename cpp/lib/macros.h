#pragma once


#define INTERNAL_STRINGIFY(s) #s
#define STRINGIFY(s) INTERNAL_STRINGIFY(s)

// Automatically chooses function overload depending on the context.
#define DISAMBIGUATE(func)  [](auto... args) { return func(args...); }
