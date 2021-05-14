package(default_visibility = ["//:__subpackages__"])

load("@pip//:requirements.bzl", "requirement")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
load("@rules_python//python:defs.bzl", "py_library", "py_binary")


cc_library(
    name = "cpp_py_util",
    hdrs = ["cpp_lib/py_util.h"],
)

# Everything should be in the same `pybind_extension` extension, otherwise things depending
# on static variables (such as formatting config overrides) wouldn't work.
pybind_extension(
    name = "py_bindings",
    srcs = [
        "cpp_lib/expressions.cpp",
        "cpp_lib/formatting.cpp",
        "cpp_lib/polylog.cpp",
        "cpp_lib/py_bindings.cpp",
        "cpp_lib/ratio.cpp",
        "cpp_lib/x.cpp",
    ],    
    deps = [
        "//cpp:polylog",
        ":cpp_py_util",
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

py_library(
    name = "polykit",
    srcs = ["polykit.py"],
    deps = ["//python:polypy_lib"],
    data = [":py_bindings.so"],
)


py_binary(
    name = "workspace",
    srcs = ["workspace.py"],
    deps = [
        requirement("numpy"),
        requirement("progress"),
        ":delta_matrix",
        ":polykit",
    ],
)
