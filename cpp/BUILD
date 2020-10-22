# TODO: Split targets
cc_library(
    name = "polylog_lib",
    hdrs = glob(["lib/*.h"]),
    srcs = glob(["lib/*.cpp"]),
    deps = [
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/strings:strings",
    ],
)

cc_binary(
    name = "workspace",
    srcs = ["workspace.cpp"],
    deps = [
        "polylog_lib",
        "@absl//absl/debugging:failure_signal_handler",
        "@absl//absl/debugging:symbolize",
    ]
)
