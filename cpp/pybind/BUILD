package(default_visibility = ["//:__subpackages__"])

load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library", "py_binary")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")


cc_library(
    name = "py_util",
    hdrs = ["py_util.h"],
)

# Everything should be in the same `pybind_extension` extension, otherwise things depending
# on static variables (such as formatting config overrides) wouldn't work.
pybind_extension(
    name = "pylib",
    srcs = [
        "expressions.cpp",
        "formatting.cpp",
        "loops.cpp",
        "lyndon.cpp",
        "polylog.cpp",
        "pylib.cpp",
        "ratio.cpp",
        "x.cpp",
    ],
    deps = [
        "//:polylog",
        ":py_util",
    ],
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
    data = [":pylib.so"],
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
