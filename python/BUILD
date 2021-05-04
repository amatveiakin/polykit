# Note. Bazel is not required to run pure Python code. This BUILD file is used
# by pybind to include Python libraries.

package(default_visibility = ["//:__subpackages__"])

load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")


py_library(
    name = "polypy_lib",
    srcs = [
        "polypy/lib/profiler.py",
        "polypy/lib/util.py",
    ],
    deps = [
        requirement("psutil"),
    ]
)
