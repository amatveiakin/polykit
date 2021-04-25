1. Install a modern C++ compiler with C++17 support.
2. Install Bazel: https://bazel.build/.
3. Execute `run.bat` (Windows) or `run.sh` (Linux/MacOS). Bazel should install
   the dependencies, build and run the project. Internet connection is required
   for the first run.

To execute tests, use `test_all`. Despite the name is calls most but not all
of the tests skipping the most expensive ones. To run every single test use
`test_extra`.

Tested compilers: Clang 10.0.0, MSVC 14.0 (C/C++ compiler version 19.27.29112).

Note. If working with Unicode in Windows command prompt, type `chcp 65001`.
For best performance it is also suggested to use “DejaVu Sans Mono” font.
