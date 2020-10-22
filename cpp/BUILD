# TODO: Split targets
cc_library(
    name = "polylog_lib",
    hdrs = glob(["lib/*.h"]),
    srcs = glob(["lib/*.cpp"]),
    deps = ["@absl//absl/container:flat_hash_map"],
)

cc_binary(
    name = "workspace",
    srcs = ["workspace.cpp"],
    deps = ["polylog_lib"]
)
