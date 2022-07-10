# Note. Bazel is not required to run pure Python code. This BUILD file is used
# by pybind to include Python libraries.

package(default_visibility = ["//:__subpackages__"])

load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")


py_library(
    name = "polypy_lib",
    srcs = glob(["polypy/lib/*.py"]),
    deps = [
        requirement("psutil"),
        requirement("sortedcontainers"),
    ]
)

py_binary(
    name = "workspace",
    srcs = ["workspace.py"],
    deps = [
        ":polypy_lib",
    ]
)
