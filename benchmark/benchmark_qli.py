# This was an attempt to create a one-stop for all benchmarks, but in the end
# it is not used the ISSAC-2021 presentation.
# 
# Available benchmarks:
#   - "cpp" is the pure C++ version wrapped with pybind11.
#       Instead use: 'benchmark_qli.cpp'. The overhead from pybind11 is small,
#       but non-zero.
#   - "hybrid" uses pybind11 to expose high-performance C++ expression class,
#       but computes QLi itself in Python.
#   - "python" computes QLi purely in Python. Note that this version can only
#       be executed via Bazel using the default Python interpreter.
#       Instead use: '/python/benchmark_qli.py' to test other interpreters.
#       TODO: Figure out how to make this runnable without Bazel.
#       TODO: Figure out how to make Bazel use PyPy.

import time
import argparse

import pybind.polykit as cpp
import python.polypy.lib.polylog_qli as python


def cross_ratio(p):
    assert len(p) % 2 == 0
    ret = cpp.DeltaExpr()
    n = len(p)
    for i in range(n):
        ret += (-1)**i * cpp.DeltaExpr.single([cpp.Delta(p[i], p[(i+1)%n])])
    return ret

def neg_cross_ratio(p):
    assert len(p) == 4
    return cross_ratio([p[0], p[2], p[1], p[3]])

def qli_node_func(p):
    assert len(p) == 4
    return (
        neg_cross_ratio([p[0][0], p[1][0], p[2][0], p[3][0]])
        if p[0][1] else
        -neg_cross_ratio([p[1][0], p[2][0], p[3][0], p[0][0]])
    )

def qli_impl(weight, points):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0
    min_weight = (num_points - 2) / 2
    assert weight >= min_weight
    def subsums():
        nonlocal weight, points
        ret = cpp.DeltaExpr()
        for i in range(num_points - 3):
            foundation = points[:i+1] + points[i+3:]
            ret += cpp.tensor_product(qli_node_func(points[i:i+4]), qli_impl(weight - 1, foundation))
        return ret
    if weight == min_weight:
        if num_points == 4:
            return qli_node_func(points)
        else:
            return subsums()
    else:
        ret = cpp.tensor_product(cross_ratio([p[0] for p in points]), qli_impl(weight - 1, points))
        if num_points > 4:
            ret += subsums()
        return ret

def hybrid_QLi(weight, points):
    tagged_points = []
    for i in range(len(points)):
        tagged_points.append((points[i], (i+1) % 2 == 1))
    return qli_impl(weight, tagged_points)


parser = argparse.ArgumentParser()
parser.add_argument("implementation", choices=["cpp", "python", "hybrid"])
args = parser.parse_args()

QLi = None
if args.implementation == "cpp":
    QLi = cpp.QLi
elif args.implementation == "python":
    QLi = python.QLi
elif args.implementation == "hybrid":
    QLi = hybrid_QLi


for weight in range(4, 8):
    iterations = 20 if weight < 6 else 5
    num_points = 8
    start = time.time()
    for _ in range(iterations):
        QLi(weight, list(range(1, num_points+1)))
    duration = (time.time() - start) / iterations
    print(f"({weight}, {duration:.3f})")
