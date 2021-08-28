load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_test")


py_test(
    name = "linear_vs_cpp",
    srcs = ["linear_vs_cpp.py"],
    deps = ["//pybind:polykit"],
)

py_test(
    name = "polylog_rank_test",
    srcs = ["polylog_rank_test.py"],
    deps = [
        requirement("numpy"),
        "//pybind:delta_matrix",
        "//pybind:polykit",
    ],
)
