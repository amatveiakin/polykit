import itertools
import numpy as np

from profiler import Profiler

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

# col_exprs = (
#     QLi2(1,2,3,4),
#     QLi2(1,2,3,5),
#     QLi2(1,2,4,5),
#     QLi2(1,3,4,5),
#     QLi2(2,3,4,5),
# )
# for expr in col_exprs:
#     expr = to_lyndon_basis(project_on_x5(expr))
#     col = []
#     expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
#     sparse_columns.append(col)
# profiler.finish("all")

# for args_tail in itertools.permutations([2,3,4]):
#     args = [1] + list(args_tail)
#     expr = to_lyndon_basis(project_on_x4(QLi3(args)))
#     col = []
#     expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
#     sparse_columns.append(col)
# profiler.finish("all")

# for args_tail in itertools.permutations([2,3,4]):
#     args = [1] + list(args_tail)
#     expr = to_lyndon_basis(project_on_x4(QLi4(args)))
#     col = []
#     expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
#     sparse_columns.append(col)
# profiler.finish("all")

# Equal with and without
# # expr_all = to_lyndon_basis(project_on_x6(QLi3(1,2,3,4,5,6)))
# # col = []
# # expr_all.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
# # sparse_columns.append(col)
# for args_tmpl in itertools.permutations([1,2,3,4,5,6]):
#     args = args_tmpl[:4]
#     expr = to_lyndon_basis(project_on_x6(QLi3(args)))
#     col = []
#     expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
#     sparse_columns.append(col)
# profiler.finish("all")

# Not equal with and without
# # expr_all = to_lyndon_basis(project_on_x6(QLi4(1,2,3,4,5,6)))
# # col = []
# # expr_all.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
# # sparse_columns.append(col)
# ... but equal if you add this on top
# # # expr_all = to_lyndon_basis(project_on_x6(QLi4(2,3,4,5,6,1)))
# # # col = []
# # # expr_all.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
# # # sparse_columns.append(col)
# for args_tmpl in itertools.permutations([1,2,3,4,5,6]):
#     args = args_tmpl[:4]
#     expr = to_lyndon_basis(project_on_x6(QLi4(args)))
#     col = []
#     expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
#     sparse_columns.append(col)
# profiler.finish("all")

print(Lira3(3)(CR(1,2,3,4)*CR(2,3,4,5)))
exit(0)

expr_bonus = to_lyndon_basis(project_on_x5(QLi6(1,2,1,3,1,4,5,2)))
col = []
expr_bonus.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
sparse_columns.append(col)
for arg_order in itertools.permutations([1,2,3,4,5]):
    expr = to_lyndon_basis(project_on_x5(QLi6(substitute([1,2,1,3,4,5], arg_order))))
    col = []
    expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
    sparse_columns.append(col)
for arg_order in itertools.permutations([1,2,3,4,5]):
    expr = to_lyndon_basis(project_on_x5(QLi6(substitute([1,2,3,1,4,5], arg_order))))
    col = []
    expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
    sparse_columns.append(col)
for args_tmpl in itertools.permutations([1,2,3,4,5]):
    args = args_tmpl[:4]
    expr = to_lyndon_basis(project_on_x5(QLi6(args)))
    col = []
    expr.foreach(lambda monom, coeff: col.append((monom_to_row(monom), coeff)))
    sparse_columns.append(col)
profiler.finish("make columns")

num_rows = next_row_id
columns = []
for sparse_col in sparse_columns:
    col = [0] * num_rows
    for row_id, coeff in sparse_col:
        col[row_id] = coeff
    columns.append(col)

mat = np.transpose(np.array(columns))
profiler.finish("make matrix")
print(mat.shape)
print(mat)
rank = np.linalg.matrix_rank(mat)
profiler.finish("compute rank")
print("rank = {}".format(rank))
# print("corank = {}".format(min(mat.shape) - rank))
