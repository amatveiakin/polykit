load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "rules_cc",
  urls = ["https://github.com/bazelbuild/rules_cc/archive/262ebec3c2296296526740db4aefce68c80de7fa.zip"],
  sha256 = "9a446e9dd9c1bb180c86977a8dc1e9e659550ae732ae58bd2e8fd51e15b2c91d",
  strip_prefix = "rules_cc-262ebec3c2296296526740db4aefce68c80de7fa",
)

http_archive(
  name = "absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/0f3bb466b868b523cf1dc9b2aaaed65c77b28862.zip"],
  strip_prefix = "abseil-cpp-0f3bb466b868b523cf1dc9b2aaaed65c77b28862",
  sha256 = "9929f3662141bbb9c6c28accf68dcab34218c5ee2d83e6365d9cb2594b3f3171"
)

http_archive(
  name = "eigen",
  urls = ["https://gitlab.com/libeigen/eigen/-/archive/3.3/eigen-3.3.zip"],
  strip_prefix = "eigen-3.3",
  sha256 = "06f41901d54e67052a8722a6a3cbb3cdca21839a2a391a1c842e90a9f1abb9c5",
  build_file = "eigen.BUILD",
)

http_archive(
  name = "googletest",
  urls = ["https://github.com/google/googletest/archive/703bd9caab50b139428cea1aaff9974ebee5742e.zip"],
  strip_prefix = "googletest-703bd9caab50b139428cea1aaff9974ebee5742e",
  sha256 = "2db427be8b258ad401177c411c2a7c2f6bc78548a04f1a23576cc62616d9cd38",
  build_file = "gmock.BUILD",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
    name = "rules_python",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.1.0/rules_python-0.1.0.tar.gz",
    sha256 = "b6d46438523a3ec0f3cead544190ee13223a52f6a6765a29eae7b7cc24cc83a0",
)

load("@rules_python//python:pip.bzl", "pip_install")
pip_install(
   requirements = "//pybind:requirements.txt",
)

# Rules for pybind11, based on https://github.com/pybind/pybind11_bazel
http_archive(
  name = "pybind11_bazel",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/26973c0ff320cb4b39e45bc3e4297b82bc3a6c09.zip"],
  strip_prefix = "pybind11_bazel-26973c0ff320cb4b39e45bc3e4297b82bc3a6c09",
  sha256 = "a5666d950c3344a8b0d3892a88dc6b55c8e0c78764f9294e806d69213c03f19d",
)
http_archive(
  name = "pybind11",
  urls = ["https://github.com/pybind/pybind11/archive/v2.6.2.tar.gz"],
  strip_prefix = "pybind11-2.6.2",
  sha256 = "8ff2fff22df038f5cd02cea8af56622bc67f5b64534f1b83b9f133b8366acff2",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")
