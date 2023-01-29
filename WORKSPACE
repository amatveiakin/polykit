load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "bazel_skylib",
  urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/1.4.0/bazel-skylib-1.4.0.tar.gz"],
  sha256 = "f24ab666394232f834f74d19e2ff142b0af17466ea0c69a3f4c276ee75f6efce",
)

http_archive(
  name = "rules_cc",
  urls = ["https://github.com/bazelbuild/rules_cc/archive/262ebec3c2296296526740db4aefce68c80de7fa.zip"],
  sha256 = "9a446e9dd9c1bb180c86977a8dc1e9e659550ae732ae58bd2e8fd51e15b2c91d",
  strip_prefix = "rules_cc-262ebec3c2296296526740db4aefce68c80de7fa",
)

http_archive(
  name = "absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/78be63686ba732b25052be15f8d6dee891c05749.zip"],
  strip_prefix = "abseil-cpp-78be63686ba732b25052be15f8d6dee891c05749",
  sha256 = "4f356a07b9ec06ef51f943928508566e992f621ed5fa4dd588865d7bed1284cd",
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
