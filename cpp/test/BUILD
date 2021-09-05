[cc_test(
    name = f[:-4],
    srcs = [f],
    deps = [
        "@googletest//:gtest_main",
        "//cpp:polylog",
        "//cpp:polylog_linalg",
        "//cpp:test_util",
    ],
    copts = ["-Icpp"],
) for f in glob(["*.cpp"])]
