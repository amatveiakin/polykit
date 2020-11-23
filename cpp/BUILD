cc_library(
    name = "base",
    hdrs = [
        "lib/bit.h",
        "lib/call_cache.h",
        "lib/check.h",
        "lib/circular_view.h",
        "lib/compression.h",
        "lib/format.h",
        "lib/hash.h",
        "lib/lexicographical.h",
        "lib/metaprogramming.h",
        "lib/profiler.h",
        "lib/sequence_iteration.h",
        "lib/string.h",
        "lib/string_basic.h",
        "lib/util.h",
        "lib/zip.h",
    ],
    srcs = [
        "lib/compression.cpp",
        "lib/format.cpp",
        "lib/sequence_iteration.cpp",
    ],
    deps = [
        "@absl//absl/algorithm:container",
        "@absl//absl/algorithm:algorithm",
        "@absl//absl/types:span",
        "@absl//absl/strings:strings",
    ],
)

cc_library(
    name = "math",
    hdrs = [
        "lib/algebra.h",
        "lib/coalgebra.h",
        "lib/corr_expression.h",
        "lib/cotheta.h",
        "lib/delta.h",
        "lib/delta_ratio.h",
        "lib/epsilon.h",
        "lib/linear.h",
        "lib/lyndon.h",
        "lib/multiword.h",
        "lib/polylog_cross_ratio_param.h",
        "lib/polylog_param.h",
        "lib/projection.h",
        "lib/quasi_shuffle.h",
        "lib/ratio.h",
        "lib/shuffle.h",
        "lib/shuffle_unrolled.h",
        "lib/summation.h",
        "lib/theta.h",
        "lib/word.h",
        "lib/x.h",
    ],
    srcs = [
        "lib/corr_expression.cpp",
        "lib/cotheta.cpp",
        "lib/delta.cpp",
        "lib/epsilon.cpp",
        "lib/polylog_cross_ratio_param.cpp",
        "lib/polylog_param.cpp",
        "lib/projection.cpp",
        "lib/ratio.cpp",
        "lib/shuffle.cpp",
        "lib/shuffle_power_unrolled.cpp",
        "lib/shuffle_unrolled.cpp",
        "lib/theta.cpp",
        "lib/word.cpp",
    ],
    deps = [
        "@absl//absl/strings:strings",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        ":base",
    ],
)

cc_library(
    name = "polylog",
    hdrs = [
        "lib/iterated_integral.h",
        "lib/mystic_algebra.h",
        "lib/polylog.h",
        "lib/polylog_cross_ratio.h",
        "lib/polylog_quadrangle.h",
        "lib/polylog_via_correlators.h",
    ],
    srcs = [
        "lib/iterated_integral.cpp",
        "lib/mystic_algebra.cpp",
        "lib/polylog.cpp",
        "lib/polylog_cross_ratio.cpp",
        "lib/polylog_quadrangle.cpp",
        "lib/polylog_via_correlators.cpp",
    ],
    deps = [
        "@absl//absl/strings:strings",
        ":base",
        ":math",
    ],
)

cc_library(
    name = "test_util",
    hdrs = glob(["test_util/*.h"]),
    srcs = glob(["test_util/*.cpp"]),
    deps = [
        "@googletest//:gtest",
    ],
)

# TODO: Split test targets
cc_test(
    name = "all_tests",
    srcs = glob(["test/*.cpp"]),
    deps = [
        "@googletest//:gtest_main",
        ":polylog",
        ":test_util",
    ],
)

cc_binary(
    name = "shuffle_unroll",
    srcs = ["generators/shuffle_unroll.cpp"],
    deps = [
        ":math",
    ]
)

cc_binary(
    name = "workspace",
    srcs = ["workspace.cpp"],
    deps = [
        ":base",
        ":math",
        ":polylog",
        "@absl//absl/debugging:failure_signal_handler",
        "@absl//absl/debugging:symbolize",
    ]
)
