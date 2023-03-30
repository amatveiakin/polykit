# Don't use `bazel run`: it messes with the current folder, and we may want to read local inputs.
bazel build -c opt --config=clang :workspace && ../bazel-bin/cpp/workspace
