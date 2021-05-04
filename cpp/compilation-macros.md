## Optimization and profiling

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


## Testing

`RUN_LARGE_TESTS` (bool; default = false).
Enables larger-than-normal tests which run seconds to tens of seconds in release
mode. Enabled by “test_extra” and “test_everything” scripts.

`RUN_HUGE_TESTS` (bool; default = false).
Enables the largest tests which can run several minutes in release mode. Enabled
by “test_everything” script.
