`ENABLE_ASSERTIONS` (bool; default = only in debug builds and tests).
Enables assertions. This affects only a small fractions of invariant-checking.
Most code relies on CHECKs, which are enabled all the time.

`PARALLELISM_IMPLEMENTATION` (0, 1 or 2; default = 1).
* 0: Disable parallelism.
* 1: Parallelism via `std::execution`. Not supported by libc++ as at Sep 2021
  (see https://en.cppreference.com/w/cpp/compiler_support, P0024R2). Requires
  linking Threading Building Blocks ("-ltbb") with libstdc++, i.e. with
  gcc/clang builds.
* 2: Parallelism via `std::async`. Known to cause `std::bad_alloc` on large
  inputs.

`UNROLL_SHUFFLE` (bool; default = only in release builds).
Unrolls shuffle product of two words. Significantly speeds up shuffle product
(and thus conversion to Lyndon basis) at the cost of increased compilation time.

`UNROLL_SHUFFLE_MULTI` (bool; default = false).
Unrolls shuffle product of multiple words. Slightly speeds up shuffle product
(and thus conversion to Lyndon basis) at the cost of increased compilation time.

`PVECTOR_STATS` (bool; default = false).
Collects statistics on how often PVector instances resorted to heap storage.
PVectors are used to store outer products and serve as keys in linear expression
underlying hash map, so the amount of inlining affects overall performance.

`DISABLE_PACKING` (bool; default = false).
Disables usage of inlined hash maps and vectors. Also disables DeltaExpr element
packing. Slows down performance and gives no benefits. Used only for benchmarks.
