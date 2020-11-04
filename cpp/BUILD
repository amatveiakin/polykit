# TODO: Split targets
cc_library(
    name = "polylog_lib",
    hdrs = glob(["lib/*.h"]),
    srcs = glob(["lib/*.cpp"]),
    deps = [
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings:strings",
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
    name = "polylog_lib_test",
    srcs = glob(["test/*.cpp"]),
    deps = [
        "@googletest//:gtest_main",
        ":polylog_lib",
        ":test_util",
    ],
)

cc_binary(
    name = "shuffle_unroll",
    srcs = ["generators/shuffle_unroll.cpp"],
    deps = [
        ":polylog_lib",
    ]
)

cc_binary(
    name = "workspace",
    srcs = ["workspace.cpp"],
    deps = [
        ":polylog_lib",
        "@absl//absl/debugging:failure_signal_handler",
        "@absl//absl/debugging:symbolize",
    ]
)
