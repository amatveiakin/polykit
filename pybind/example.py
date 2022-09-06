import glob
import itertools
import math
from pathlib import Path
import numpy as np
from progress.bar import Bar
import scipy.sparse
import time

from python.polypy.lib.linear import Linear
from python.polypy.lib.profiler import Profiler
from python.polypy.lib.util import rotate_list, to_hashable
from delta_matrix import DeltaExprMatrixBuilder

from polykit import Encoder, RichTextFormat, AnnotationSorting, set_formatting, reset_formatting, NoLineLimit
from polykit import tensor_product, to_lyndon_basis
from polykit import icoproduct, ncoproduct, icomultiply, ncomultiply
from polykit import X, Inf, Zero, x1, x2, x3, x4, x5, x6, x7, x8, x1s, x2s, x3s, x4s, x5s, x6s, x7s, x8s
from polykit import Delta, DeltaExpr, substitute_variables_0_based, substitute_variables_1_based, involute
from polykit import ProjectionExpr, project_on
from polykit import terms_with_num_distinct_variables, terms_with_min_distinct_variables, terms_containing_only_variables, terms_without_variables
from polykit import sorted_by_num_distinct_variables
from polykit import CrossRatio, CompoundRatio, CR
from polykit import I, Corr
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8
from polykit import Log, A2
from polykit import QLiPr
from polykit import Lira, Lira0, Lira1, Lira2, Lira3, Lira4, Lira5, Lira6, Lira7, Lira8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15


set_formatting(
    encoder=Encoder.unicode,
    compact_x=True,
)


def qi_symbol_example():
    print(QLi2(1,2,3,4,5,6))


def matrix_rank_example():
    def CB1(args):
        # return [QLi1(p) for p in itertools.permutations(args, 4)]
        # return [Log(args[:2] + list(p)) for p in itertools.permutations(args[2:], 2)]
        n = len(args)
        ret = []
        for i in range(n):
            for j in range(i+2, n):
                ip = i + 1
                assert ip < n
                jp = (j + 1) % n
                if jp != i:
                    ret.append(Log(args[i], args[j], args[ip], args[jp]))
        return ret
    def CB2(args):
        # return [QLi2(p) for p in itertools.combinations(args, 4)]
        return [QLi2(args[:1] + list(p)) for p in itertools.combinations(args[1:], 3)]
    def CB3(args):
        return [QLi3(p) for p in itertools.combinations(args, 4)]
    def CB4(args):
        return [QLi4(p) for p in itertools.combinations(args, 4)]
    def CB5(args):
        return [QLi5(p) for p in itertools.combinations(args, 4)]
    def CB6(args):
        return [QLi6(p) for p in itertools.combinations(args, 4)]
    def CB7(args):
        return [QLi7(p) for p in itertools.combinations(args, 4)]

    def CL4(args):
        return CB4(args) + [A2(p) for p in itertools.combinations(args, 5)]
    def CL5(args):
        return CB5(args) + [QLi5(p) for p in itertools.combinations(args, 6)]

    profiler = Profiler()
    matrix_builder = DeltaExprMatrixBuilder()
    points = [x1,x2,x3,x4,x5,x6,x7]
    def prepare(expr):
        return ncomultiply(expr, (2,1,1))
    for s1 in Bar('B3xB1').iter(CB3(points)):
        for s2 in CB1(points):
            matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
    for s1 in Bar('B2xB2').iter(CB2(points)):
        for s2 in CB2(points):
            matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
    profiler.finish("expr")
    mat = matrix_builder.make_np_array()
    profiler.finish("matrix")
    rank = np.linalg.matrix_rank(mat)
    profiler.finish("rank")
    nonzero_percent = np.count_nonzero(mat) * 100.0 / mat.size
    print(f"{mat.shape} [{nonzero_percent:.2f}% nonzero] => {rank}")


print("Successfully running PolyKit C++/Python bindings, congrats :)")
print("Navigate to workspace.py to see some examples.")

# qi_symbol_example()
# matrix_rank_example()
