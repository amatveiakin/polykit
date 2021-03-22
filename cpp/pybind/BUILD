load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")


cc_library(
    name = "py_util",
    hdrs = ["py_util.h"],
)


pybind_extension(
    name = "delta",
    srcs = ["delta.cpp"],
    deps = [
        "//:math",
        ":py_util",
    ],
)

pybind_extension(
    name = "polylog_qli",
    srcs = ["polylog_qli.cpp"],
    deps = ["//:polylog"],
)

pybind_extension(
    name = "x",
    srcs = ["x.cpp"],
    deps = ["//:math"],
)


py_library(
    name = "polykit_util",
    srcs = [
        "format.py",
        "linear.py",
        "util.py",
    ],
)

py_library(
    name = "polykit",
    srcs = ["polykit.py"],
    deps = [":polykit_util"],
    data = [
        ":delta.so",
        ":polylog_qli.so",
        ":x.so",
    ],
)

py_binary(
    name = "py_workspace",
    srcs = ["py_workspace.py"],
    deps = [":polykit"],
)
