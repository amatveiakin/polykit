# Setup

**For pure C++ code**

1. Install Bazel: https://bazel.build/.
2. Install a C++ compiler with C++17 support.
   I've tested with Clang 10.0.0 and MSVC 14.0 (C/C++ compiler version
   19.27.29112), but other modern compilers should work as well.

All other dependencies are managed by Bazel. This means an internet connection
is required for on first run.

**For pure Python code**

Install Python3.

**For Python code with C++ bindings**

Follow the steps for C++ and for Python. In additional to that:

   * On Linux/MacOS. Install Python3 development tools. Ubuntu package name
     is `python3-dev`.
   * On Windows. I didn't manage to build Python bindings directly in Window,
     please tell me if you do :) As a workaround I've been using Windows
     Subsystem for Linux (WSL). Note that in this case the entire stack
     (Bazel, C++ compiler, Python) needs to be installed in WSL.


# Execute

For C++ workspace execute `run` (e.g. `run.bat` on Windows and `run.sh` on
Linux/MacOS). For Python execute `run_pybind`.

To execute tests use: `test` for a limited set fast of tests, `test_extra` to
run most tests, and `test_everything` to run all tests (slow).


# Troubleshooting

**Q. Unicode symbols are broken in Windows command prompt.**

**A.**
Type `chcp 65001` (needs to be executed every time a command prompt is loaded).
For best performance also set “DejaVu Sans Mono” as the terminal font. The
latter also applies to WSL terminals.

**Q. Error: `Python Configuration Error: Repository command failed find: ‘/usr/include/python3.8’: No such file or directory`**.

**A.** Python dev libraries are probably missing.

**Q. Error: `undefined symbol: _Py_ZeroStruct`**.

**A.** Python2 is probably used at runtime. In Ubuntu try running
`sudo apt install python-is-python3 python-dev-is-python3`.

**Q. Error: `cp: cannot stat '/usr/include/python2.7/Python-ast.h': No such file or directory`**

**A.** Python2 is used at configuration time. Try uninstalling Python2
completely. Afterwards you may have to clear Bazel cache via
`rm -rf /home/<username>/.cache/bazel/`.
