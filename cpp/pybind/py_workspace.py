import glob
import itertools
import math
from pathlib import Path
import numpy as np
from progress.bar import Bar

from profiler import Profiler
from delta_matrix import DeltaExprMatrixBuilder

from polykit import Encoder, RichTextFormat, AnnotationSorting, set_formatting, reset_formatting, NoLineLimit
from polykit import tensor_product, to_lyndon_basis
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
from polykit import Lira, Lira0, Lira1, Lira2, Lira3, Lira4, Lira5, Lira6, Lira7, Lira8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15
from polykit import loops_matrix



def cross_ratio(p):
    assert len(p) % 2 == 0
    ret = DeltaExpr()
    n = len(p)
    for i in range(n):
        ret += (-1)**i * DeltaExpr.single([Delta(p[i], p[(i+1)%n])])
    return ret

# Symbol for (1 - cross_ratio(p))
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
        ret = DeltaExpr()
        for i in range(num_points - 3):
            foundation = points[:i+1] + points[i+3:]
            ret += tensor_product(qli_node_func(points[i:i+4]), qli_impl(weight - 1, foundation))
        return ret
    if weight == min_weight:
        if num_points == 4:
            return qli_node_func(points)
        else:
            return subsums()
    else:
        ret = tensor_product(cross_ratio([p[0] for p in points]), qli_impl(weight - 1, points))
        if num_points > 4:
            ret += subsums()
        return ret

def py_QLi(weight, points):
    tagged_points = []
    for i in range(len(points)):
        tagged_points.append((points[i], (i+1) % 2 == 1))
    return qli_impl(weight, tagged_points)


profiler = Profiler()
py_expr = py_QLi(8, [1,2,3,4,5,6])
profiler.finish('Python')
cpp_expr = QLi(8, [1,2,3,4,5,6])
profiler.finish('C++')
print(py_expr - cpp_expr)
exit(0)


# TODO: How to get project folder from `bazel run`?
RESULTS_FOLDER = '/mnt/c/Danya/results/'


def substitute(points, new_indices):
    return [new_indices[p - 1] for p in points]

def with_progress(values):
    bar = Bar('Computing...', max=len(values), suffix='%(index)d/%(max)d   ETA %(eta_td)s')
    for p in values:
        yield p
        bar.next()
    bar.finish()

# TODO: Parallelize via `multiprocessing`
def iterate_permutations(n, *, filter=None):
    values = [
        p
        for p
        in itertools.permutations(list(range(1, n+1)))
        if filter is None or filter(p)
    ]
    return with_progress(values)

def get_matrix_filename():
    folder = RESULTS_FOLDER
    file_prefix = 'matrix-'
    extension = '.npy'
    existing_files = glob.glob(f'{folder}{file_prefix}*{extension}')
    max_existing_index = 0
    for f in existing_files:
        index = Path(f).stem[len(file_prefix):]
        max_existing_index = max(max_existing_index, int(index))
    new_index = max_existing_index + 1
    return f'{folder}{file_prefix}{new_index}{extension}'


set_formatting(encoder=Encoder.unicode)

profiler = Profiler()

# matrix_builder = DeltaExprMatrixBuilder()

# def describe(matrix_builder):
#     global profiler
#     profiler.finish("expr")
#     mat = matrix_builder.make_np_array()
#     filename = get_matrix_filename()
#     np.save(filename, mat)
#     print(f'Saved to {filename}')
#     rank = np.linalg.matrix_rank(mat)
#     profiler.finish("rank")
#     print(f"{mat.shape} => {rank}")


expr = QLi(6, [1,2,3,4,5,6,7,8])
profiler.finish("expr")


# mat = np.transpose(np.array(loops_matrix(), dtype=int))
# print(mat.shape)
# print(mat)
# rank = np.linalg.matrix_rank(mat)
# print("rank = {}".format(rank))


# #   TODO:
# def prepare(expr):
#     return comultiply(expr, (2,2,2))
# for args in iterate_permutations(8, filter = lambda p: p[0] == 1 and p[1] < p[2]):
#     expr = QLi6(args)
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)


# #   TODO:
# def prepare(expr):
#     return comultiply(expr, (3,3))
# for args in iterate_permutations(7):
#     expr = QLi6(substitute([1,2,3,4,5,6], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations(7):
#     expr = QLi4(substitute([1,2,3,4,5,6], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 231


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



# def projector(expr):
#     return project_on_x6(terms_without_variables(expr, [1]))
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
#     expr = Lira3(2,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 86
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira3(2,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
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
#     expr = Lira3(2,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 86
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira3(2,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 92
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(1,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 92


# def prepare(expr):
#     return to_lyndon_basis(project_on_x5(expr))
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira3(2,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 62
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira3(2,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 74
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(1,1)(CR(substitute([1,2,3,4], args)), CR(substitute([1,4,5,2], args)))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 74
# for args in itertools.permutations([1,2,3,4,5]):
#     expr = Lira4(1,1)(CR(substitute([1,2,3,4], args)), CompoundRatio())
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
