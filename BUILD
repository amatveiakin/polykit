cc_library(
    name = "base",
    hdrs = [
        "lib/bit.h",
        "lib/check.h",
        "lib/compression.h",
        "lib/format_basic.h",
        "lib/lexicographical.h",
        "lib/macros.h",
        "lib/metaprogramming.h",
        "lib/packed.h",
        "lib/profiler.h",
        "lib/range.h",
        "lib/sequence_iteration.h",
        "lib/set_util.h",
        "lib/string.h",
        "lib/string_basic.h",
        "lib/util.h",
        "lib/zip.h",
    ],
    srcs = [
        "lib/compression.cpp",
        "lib/format_basic.cpp",
        "lib/sequence_iteration.cpp",
    ],
    deps = [
        "@absl//absl/algorithm:container",
        "@absl//absl/algorithm:algorithm",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/container:inlined_vector",
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
        "lib/delta.h",
        "lib/delta_ratio.h",
        "lib/epsilon.h",
        "lib/format.h",
        "lib/linear.h",
        "lib/lyndon.h",
        "lib/polylog_li_param.h",
        "lib/polylog_lira_param.h",
        "lib/projection.h",
        "lib/quasi_shuffle.h",
        "lib/ratio.h",
        "lib/shuffle.h",
        "lib/summation.h",
        "lib/theta.h",
        "lib/x.h",
    ],
    srcs = [
        "lib/corr_expression.cpp",
        "lib/delta.cpp",
        "lib/epsilon.cpp",
        "lib/polylog_li_param.cpp",
        "lib/polylog_lira_param.cpp",
        "lib/projection.cpp",
        "lib/ratio.cpp",
        "lib/theta.cpp",
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
        "lib/polylog_li.h",
        "lib/polylog_liquad.h",
        "lib/polylog_lira.h",
        "lib/polylog_via_correlators.h",
        "lib/polylog_qli.h",
    ],
    srcs = [
        "lib/iterated_integral.cpp",
        "lib/mystic_algebra.cpp",
        "lib/polylog_li.cpp",
        "lib/polylog_liquad.cpp",
        "lib/polylog_lira.cpp",
        "lib/polylog_via_correlators.cpp",
        "lib/polylog_qli.cpp",
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
