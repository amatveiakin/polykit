import itertools
import numpy as np

from profiler import Profiler

from polykit import to_lyndon_basis
from polykit import coproduct, comultiply
from polykit import X, Inf, Delta, DeltaExpr, substitute_variables, project_on, involute, involute_projected
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15


def corank(mat, rank):
    return max(mat.shape) - rank

def substitute(points, new_indices):
    return [new_indices[p - 1] for p in points]

profiler = Profiler()
sparse_columns = []
monoms_to_rows = {}
next_row_id = 0

def monom_to_row(monom):
    global monoms_to_rows, next_row_id
    monom_tuple = tuple(monom)
    if not monom_tuple in monoms_to_rows:
        monoms_to_rows[monom_tuple] = next_row_id
        next_row_id += 1
    return monoms_to_rows[monom_tuple]

for arg_order in itertools.permutations([1,2,3,4,5]):
    expr = to_lyndon_basis(project_on_x5(QLi6(substitute([1,2,1,3,4,5], arg_order))))
    if not expr.is_zero():
        col = []
        expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
        sparse_columns.append(col)
profiler.finish("all")

num_rows = next_row_id
columns = []
for sparse_col in sparse_columns:
    col = [0] * num_rows
    for row_id, coeff in sparse_col:
        col[row_id] = coeff
    columns.append(col)

mat = np.array(columns)
print(mat.shape)
print(mat)
rank = np.linalg.matrix_rank(mat)
print("rank = {}".format(rank))
print("corank = {}".format(corank(mat, rank)))
