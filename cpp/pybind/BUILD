package(default_visibility = ["//:__subpackages__"])

load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library", "py_binary")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")


cc_library(
    name = "py_util",
    hdrs = ["py_util.h"],
)


pybind_extension(
    name = "expressions",
    srcs = ["expressions.cpp"],
    deps = [
        "//:math",
        ":py_util",
    ],
)

pybind_extension(
    name = "loops",
    srcs = ["loops.cpp"],
    deps = ["//:polylog"],
)

pybind_extension(
    name = "lyndon",
    srcs = ["lyndon.cpp"],
    deps = ["//:math"],
)

pybind_extension(
    name = "polylog",
    srcs = ["polylog.cpp"],
    deps = ["//:polylog"],
)

pybind_extension(
    name = "ratio",
    srcs = ["ratio.cpp"],
    deps = ["//:math"],
)

pybind_extension(
    name = "x",
    srcs = ["x.cpp"],
    deps = ["//:math"],
)


py_library(
    name = "polykit_util",
    srcs = [
        "util.py",
    ],
)

py_library(
    name = "polykit",
    srcs = ["polykit.py"],
    deps = [":polykit_util"],
    data = [
        ":expressions.so",
        ":loops.so",
        ":lyndon.so",
        ":polylog.so",
        ":ratio.so",
        ":x.so",
    ],
)

py_library(
    name = "delta_matrix",
    srcs = ["delta_matrix.py"],
    deps = [
        requirement("numpy"),
        ":polykit",
    ],
)

py_binary(
    name = "py_workspace",
    srcs = ["py_workspace.py"],
    deps = [
        requirement("numpy"),
        requirement("progress"),
        ":delta_matrix",
        ":polykit",
    ],
)
