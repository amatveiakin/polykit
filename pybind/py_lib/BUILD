package(default_visibility = ["//:__subpackages__"])

load("@pip//:requirements.bzl", "requirement")


py_library(
    name = "polykit_util",
    srcs = [
        "profiler.py",
        "util.py",
    ],
    deps = [
        requirement("psutil"),
    ]
)
