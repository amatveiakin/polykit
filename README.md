# PolyKit

A toolkit for polylogarithms. Contains a set functions that compute
polylogarithm symbols and formal symbols. These functions are highly
specific. This is neither a computer algebra system nor a tool for
numeric computations. Only polylogarithms of pure variables and
cross-ratio are supported. The advantage of this software is that it's
faster that general-purpose computer algerbra systems.

PolyKit was used to verify some results in “On the Goncharov depth conjecture
and a formula for volumes of orthoschemes” (https://arxiv.org/abs/2012.05599)
and most results in “Cluster Polylogarithms” (https://arxiv.org/abs/2208.01564).
It was presented at [ISSAC 2021](https://issac-conference.org/2021/posters.php).


### Authors

* _Software development by_ Andrei Matveiakin
* _Maths by_ Daniil Rudenko


### Folder structure

* `cpp` — main implementation, most mature;
* `python` — implementation of some basic features in pure Python;
* `pybind` — bindings allowing to use a subset of C++ functionality from Python;
* `rust` — an experimental implementation of the project in Rust, promising but nascent;
* the rest is mostly technical folders of no general interest.

The `workspace.cpp`/`workspace.py` files and associated `run.sh`/`run.bat`
scripts are used internally for development. See `example.cpp`/`example.py`
and corresponding `run_example.sh`/`run_example.bat` for sample usage.
