load("@rules_python//python:defs.bzl", "py_library", "py_binary")


py_binary(
    name = "benchmark_qli",
    srcs = ["benchmark_qli.py"],
    deps = [
        "//pybind:polykit",
        "//python:polypy_lib",
    ],
)

cc_binary(
    name = "benchmark_qli_cpp",
    srcs = ["benchmark_qli.cpp"],
    deps = [
        "//cpp:polylog",
    ],
)

cc_binary(
    name = "benchmark_equations_cpp",
    srcs = ["benchmark_equations.cpp"],
    deps = [
        "//cpp:polylog",
    ],
)
