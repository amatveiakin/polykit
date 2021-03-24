import itertools
import numpy as np

from profiler import Profiler
from delta_matrix import DeltaExprMatrixBuilder

from polykit import to_lyndon_basis
from polykit import coproduct, comultiply
from polykit import X, Inf, Delta, DeltaExpr, substitute_variables, project_on, involute, involute_projected
from polykit import CrossRatio, CompoundRatio, CR
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8
from polykit import Lira, Lira1, Lira2, Lira3, Lira4, Lira5, Lira6, Lira7, Lira8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15


def substitute(points, new_indices):
    return [new_indices[p - 1] for p in points]

profiler = Profiler()

matrix_builder = DeltaExprMatrixBuilder()

matrix_builder.add_expr(to_lyndon_basis(project_on_x5(QLi6(1,2,1,3,1,4,5,2))))
for arg_order in itertools.permutations([1,2,3,4,5]):
    expr = to_lyndon_basis(project_on_x5(QLi6(substitute([1,2,1,3,4,5], arg_order))))
    matrix_builder.add_expr(expr)
for arg_order in itertools.permutations([1,2,3,4,5]):
    expr = to_lyndon_basis(project_on_x5(QLi6(substitute([1,2,3,1,4,5], arg_order))))
    matrix_builder.add_expr(expr)
for args_tmpl in itertools.permutations([1,2,3,4,5]):
    args = args_tmpl[:4]
    expr = to_lyndon_basis(project_on_x5(QLi6(args)))
    matrix_builder.add_expr(expr)
profiler.finish("make columns")

# for args_tmpl in itertools.permutations([1,2,3,4,5,6]):
#     args = args_tmpl[:4]
#     expr = involute_projected(QLi4(args), [1,2,3,4,5,6], 6)
#     matrix_builder.add_expr(expr)

mat = matrix_builder.make_np_array()
profiler.finish("make matrix")

print(mat.shape)
print(mat)
rank = np.linalg.matrix_rank(mat)
profiler.finish("compute rank")
print("rank = {}".format(rank))
# print("corank = {}".format(min(mat.shape) - rank))
