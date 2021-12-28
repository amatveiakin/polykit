# Setup

**For pure C++ code**

1. Install Bazel: https://bazel.build/.
2. Install a C++ compiler with C++17 support.
   I've tested with Clang 10.0.0 and MSVC 14.0 (C/C++ compiler version
   19.27.29112), but other modern compilers should work as well.
3. Linux/MacOS: install TBB (in Ubuntu: `sudo apt-get install libtbb-dev`).
   Alternative: set `PARALLELISM_IMPLEMENTATION` to 0 or 2, see
   cpp/compilation-macros.md.
4. Install LinBox library with headers and dependencies (in Ubuntu:
   `sudo apt-get install liblinbox-dev libgmp-dev libntl-dev`).

All other dependencies are managed by Bazel. An internet connection is required
on first run.

**For pure Python code**

1. Install Python3. Note that PyPy has better performance than the default
   (CPython) interpreter.
2. Do one of the two:
   * Install Bazel: https://bazel.build/. If running Python code via Bazel, it
     will take care of the dependencies.
   * Install dependencies manually, namely “sortedcontainers” module.
     For CPython via PyPI: `pip install sortedcontainers` (you may need to
     replace `pip` with `pip3`).
     For PyPy: `pypy3 -m pip install sortedcontainers`

**For Python code with C++ bindings**

Follow the steps for C++. In additional to that:

   * On Linux/MacOS. Install Python3 development tools. Ubuntu package name
     is `python3-dev`.
   * On Windows. I didn't manage to build Python bindings directly in Window,
     please tell me if you do :) As a workaround I've been using Windows
     Subsystem for Linux (WSL). Note that in this case the entire stack
     (Bazel, C++ compiler, Python) needs to be installed in WSL.

Note that Bazel runs Python in a hermetic way and maintains all dependencies
itself. Python packages installed in the system will not be available to Bazel
and vice versa. In order to add a new Python dependency for pybind code, add it
to `pybind/requirements.txt`.


# Execute

Go the corresponding folder (`cpp`, `python` or `pybind`) and execute `run`
(i.e. `run.bat` on Windows and `run.sh` on Linux/MacOS). This will compile and
run the file called `workspace.<ext>`.

To execute tests use: `test` for the default set of tests, `test_extra` to run
most tests, and `test_everything` to run all tests (slow).

There exist several macros that affect C++ (and thus pybind) behavior. For more
information see `cpp/compilation-macros.md`.


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
