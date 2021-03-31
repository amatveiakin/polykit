import itertools
import math
import numpy as np
from progress.bar import Bar

from profiler import Profiler
from delta_matrix import DeltaExprMatrixBuilder

from polykit import to_lyndon_basis
from polykit import coproduct, comultiply
from polykit import X, Inf, Delta, DeltaExpr, substitute_variables
from polykit import ProjectionExpr, project_on, involute, involute_projected
from polykit import terms_with_num_distinct_variables, terms_with_min_distinct_variables, terms_containing_only_variables, terms_without_variables
from polykit import sorted_by_num_distinct_variables
from polykit import CrossRatio, CompoundRatio, CR
from polykit import I, Corr
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8
from polykit import QLiPr
from polykit import Lira, Lira1, Lira2, Lira3, Lira4, Lira5, Lira6, Lira7, Lira8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15
from polykit import loops_matrix


def substitute(points, new_indices):
    return [new_indices[p - 1] for p in points]

def with_progress(generator):
    values = list(generator)
    bar = Bar('Computing...', max=len(values))
    for p in values:
        yield p
        bar.next()
    bar.finish()

def iterate_permutations(n):
    return with_progress(itertools.permutations(list(range(1, n+1))))

profiler = Profiler()

matrix_builder = DeltaExprMatrixBuilder()

def describe(matrix_builder):
    global profiler
    profiler.finish("expr")
    mat = matrix_builder.make_np_array()
    rank = np.linalg.matrix_rank(mat)
    profiler.finish("rank")
    print(f"{mat.shape} => {rank}")


# mat = np.transpose(np.array(loops_matrix(), dtype=int))
# print(mat.shape)
# print(mat)
# rank = np.linalg.matrix_rank(mat)
# print("rank = {}".format(rank))


def prepare(expr):
    return to_lyndon_basis(expr)
for args in iterate_permutations(7):
    expr = QLi4(substitute([1,2,3,4,5,6], args))
    matrix_builder.add_expr(prepare(expr))
describe(matrix_builder)  # 231


# def prepare(expr):
#     return comultiply(expr, (2,4))
#     # return to_lyndon_basis(expr)
# for args in iterate_permutations(6):
#     expr = QLi6(substitute([1,2,3,4,5,6], args))
#     matrix_builder.add_coexpr(prepare(expr))
# describe(matrix_builder)


# expr = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr += project_on_x6(QLi5(substitute([1,2,3,4,5,6], args)))
# expr2 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr2 += project_on_x6(QLi5(substitute([1,2,3,4], args)))
# expr += expr2.dived_int(2) * 3
# expr = to_lyndon_basis(expr)
# print(expr)  # zero


# def projector(expr):
#     return project_on_x8(terms_without_variables(expr, [1]))
# expr8 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6,7,8]):
#     expr8 += QLiPr(7, substitute([1,2,3,4,5,6,7,8], args), projector)
# profiler.finish("8 points")
# expr6 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6,7,8]):
#     expr6 += QLiPr(7, substitute([1,2,3,4,5,6], args), projector)
# profiler.finish("6 points")
# expr4 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6,7,8]):
#     expr4 += QLiPr(7, substitute([1,2,3,4], args), projector)
# profiler.finish("4 points")
# expr = expr8 * 3 + expr6 * 4 + expr4 * 6
# profiler.finish("Sum")
# expr = to_lyndon_basis(expr)
# profiler.finish("Lyndon")
# print(expr)  # zero


# def prepare(expr):
#     return project_on_x8(terms_without_variables(expr, [1]))
# expr8 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6,7,8]):
#     expr8 += prepare(QLi7(substitute([1,2,3,4,5,6,7,8], args)))
# profiler.finish("8 points")
# expr6 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6,7,8]):
#     expr6 += prepare(QLi7(substitute([1,2,3,4,5,6], args)))
# profiler.finish("6 points")
# expr4 = ProjectionExpr()
# for args in itertools.permutations([1,2,3,4,5,6,7,8]):
#     expr4 += prepare(QLi7(substitute([1,2,3,4], args)))
# profiler.finish("4 points")
# expr = expr8 * 3 + expr6 * 4 + expr4 * 6
# expr = to_lyndon_basis(expr)
# print(expr)  # zero


# def projector(expr):
#     return project_on_x6(terms_without_variables(expr, [1]))
# def prepare(expr):
#     # return to_lyndon_basis(project_on_x6(terms_without_variables(expr, [1])))
#     return to_lyndon_basis(expr)
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLiPr(5, substitute([1,2,3,4,5,6], args), projector)
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLiPr(5, substitute([1,2,3,4], args), projector)
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)



# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLiPr(5, substitute([1,2,3,4,5,6], args), projector)
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 90
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLiPr(5, substitute([1,2,3,4], args), projector)
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 104


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,2,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 9
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,1,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 21


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,1,1,2,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,1,2,1,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,2,1,1,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,2,1,3,1,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,1,1,1,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,1,1,3,1,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4]):
#     expr = Corr(substitute([1,1,1,3,1,1,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)

# def prepare_inf(expr):
#     return to_lyndon_basis(terms_without_variables(expr, [5]))
# for arg_order in itertools.permutations([1,2,3,4,5]):
#     expr = prepare_inf(QLi6(substitute([1,2,3,1,4,5], arg_order)))
#     matrix_builder.add_expr(expr)
# describe(matrix_builder)
# for arg_order in itertools.permutations([1,2,3,4,5]):
#     expr = prepare_inf(QLi6(substitute([1,2,1,3,4,5], arg_order)))
#     matrix_builder.add_expr(expr)
# describe(matrix_builder)


# def prepare(expr):
#     return to_lyndon_basis(project_on_x6(expr))
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLi6(substitute([1,2,1,3,1,4,5,6], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # == 435
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLi6(substitute([1,2,1,3,4,1,5,6], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # == 616
# for args in itertools.permutations([1,2,3,4,5,6]):
#     expr = QLi6(substitute([1,2,3,4,5,6], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # == 616


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(expr))
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = QLi6(substitute([1,2,1,3,1,4,5,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = QLi6(substitute([1,2,1,3,1,2,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(expr))
#     # return to_lyndon_basis(expr)
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = QLi5(substitute([1,2,1,3,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = QLi5(substitute([1,2,3,1,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = QLi5(substitute([1,2,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(expr))
# for arg_order in itertools.permutations([1,2,3,4,5]):
#     expr = prepare(QLi6(substitute([1,2,1,3,4,5], arg_order)))
#     matrix_builder.add_expr(expr)
# for arg_order in itertools.permutations([1,2,3,4,5]):
#     expr = prepare(QLi6(substitute([1,2,3,1,4,5], arg_order)))
#     matrix_builder.add_expr(expr)
# # for arg_order in itertools.permutations([1,2,3,4,5]):
# #     expr = prepare(QLi6(substitute([1,2,3,4], arg_order)))
# #     matrix_builder.add_expr(expr)
# describe(matrix_builder)  # 72
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(2,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 86
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(2,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 92
# # matrix_builder.add_expr(prepare(QLi6(1,2,1,3,1,4,5,2)))
# # matrix_builder.add_expr(prepare(QLi6(1,2,1,2,1,3,4,5)))
# # describe(matrix_builder)  # 92


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(terms_without_variables(expr, [1])))
# for arg_order in itertools.permutations([1,2,3,4,5]):
#     expr = prepare(QLi6(substitute([1,2,3,1,4,5], arg_order)))
#     matrix_builder.add_expr(expr)
# describe(matrix_builder)  # 42
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(2,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 86
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(2,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 92
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira5(1,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 92


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(expr))
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(2,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 62
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(2,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 74
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira5(1,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 74
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira5(1,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 82


# def prepare(expr):
#     return to_lyndon_basis(involute_projected(expr, [1,2,3,4,5,6], 6))
# for args_tmpl in itertools.permutations([1,2,3,4,5,6]):
#     args = args_tmpl[:4]
#     matrix_builder.add_expr(prepare(QLi4(args)))
# matrix_builder.add_expr(prepare(QLi4(1,6,1,5,3,2)))
# matrix_builder.add_expr(prepare(QLi4(6,5,1,3,4,5)))


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(expr))
# for args_tmpl in itertools.permutations([1,2,3,4,5]):
#     args = args_tmpl[:4]
#     matrix_builder.add_expr(prepare(QLi4(args)))


# mat = matrix_builder.make_np_array()
# profiler.finish("make matrix")

# print(mat.shape)
# print(mat)
# rank = np.linalg.matrix_rank(mat)
# profiler.finish("compute rank")
# print("rank = {}".format(rank))
# # print("corank = {}".format(min(mat.shape) - rank))
