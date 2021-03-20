@REM Install LLVM via http://releases.llvm.org/download.html
@REM Specifying LLVM path is optional. It is auto-detected if BAZEL_LLVM is not set.
@REM   set BAZEL_LLVM=C:\Program Files\LLVM
bazel run -c opt --config=clang-cl --extra_toolchains=@local_config_cc//:cc-toolchain-x64_windows-clang-cl --extra_execution_platforms=//:windows-clang-cl :workspace
