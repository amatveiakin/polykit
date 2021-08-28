import glob
import itertools
import math
from pathlib import Path
import numpy as np
from progress.bar import Bar

from python.polypy.lib.linear import Linear
from python.polypy.lib.profiler import Profiler
from python.polypy.lib.util import rotate_list, to_hashable
from delta_matrix import DeltaExprMatrixBuilder

from polykit import Encoder, RichTextFormat, AnnotationSorting, set_formatting, reset_formatting, NoLineLimit
from polykit import tensor_product, to_lyndon_basis
from polykit import coproduct, ncoproduct, comultiply, ncomultiply
from polykit import X, Inf, Zero, x1, x2, x3, x4, x5, x6, x7, x8, x1s, x2s, x3s, x4s, x5s, x6s, x7s, x8s
from polykit import Delta, DeltaExpr, substitute_variables, involute
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
from polykit import loops_matrix


# TODO: How to get project folder from `bazel run`?
RESULTS_FOLDER = '/mnt/c/Danya/results/'


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
def CB1Alt(args):
    return [Log(p) for p in itertools.combinations(args, 4)]
def CB2(args):
    # return [QLi2(p) for p in itertools.combinations(args, 4)]
    return [QLi2(args[:1] + list(p)) for p in itertools.combinations(args[1:], 3)]
def CB3(args):
    return [QLi3(p) for p in itertools.combinations(args, 4)]
# def CB4(args):
#     return [QLi4(p) for p in itertools.combinations(args, 4)]
def CB4_formal(args):
    return [f'QLi4({p})' for p in itertools.combinations(args, 4)]
def CB4(args):
    return [eval(f) for f in CB4_formal(args)]
def CB5(args):
    return [QLi5(p) for p in itertools.combinations(args, 4)]
def CB6(args):
    return [QLi6(p) for p in itertools.combinations(args, 4)]
def CB7(args):
    return [QLi7(p) for p in itertools.combinations(args, 4)]

def CSB3(args):
    return [QLi3(p) for p in itertools.permutations(args, 4)]
def CSB4(args):
    return [QLi4(p) for p in itertools.permutations(args, 4)]

# def CL4(args):
#     return CB4(args) + [A2(p) for p in itertools.combinations(args, 5)]
def CL4_formal(args):
    return CB4_formal(args) + [f'A2({p})' for p in itertools.combinations(args, 5)]
def CL4(args):
    return [eval(f) for f in CL4_formal(args)]
def CL5(args):
    return CB5(args) + [QLi5(p) for p in itertools.combinations(args, 6)]

def CSL4(args):
    return CSB4(args) + [A2(p) for p in itertools.permutations(args, 5)]


def substitute(points, new_indices):
    return [new_indices[p - 1] for p in points]

# TODO: Rename to `dihedral_sum`
def dihedralize(func, args, num_points):
    ret = func(args)
    for shift in range(1, num_points):
        ret += func([(a + shift - 1) % num_points + 1 for a in args])
    args.reverse()
    for shift in range(0, num_points):
        ret -= func([(a + shift - 1) % num_points + 1 for a in args])
    # indices = list(range(0, num_args))
    # for shift in range(0, num_args):
    #     ret -= func([args[num_args - idx - 1] for idx in rotate_list(indices, shift)])
    return ret

def glue_formal_symbol(formal, expr):
    return Linear.from_cpp(expr).mapped(lambda term : (formal, term))


def is_sorted(l):
    return sorted(l) == list(l)

def include_permutation(permutation, max_point):
    return is_sorted(permutation[max_point:])

def add_expr(matrix_builder, prepare, func, permutation, indices):
    if include_permutation(permutation, max(indices)):
        expr = func(substitute(indices, permutation))
        matrix_builder.add_expr(prepare(expr))

def with_progress(values):
    bar = Bar('Computing...', max=len(values), suffix='%(index)d/%(max)d   ETA %(eta_td)s')
    for p in values:
        yield p
        bar.next()
    bar.finish()

# TODO: Parallelize via `multiprocessing`
def iterate_permutations(points, *, filter=None):
    values = [
        p
        for p
        in itertools.permutations(points)
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


set_formatting(
    encoder=Encoder.unicode,
    compact_x=True,
    # expression_line_limit=NoLineLimit,
)

profiler = Profiler()

matrix_builder = DeltaExprMatrixBuilder()

def describe(matrix_builder):
    global profiler
    profiler.finish("expr")
    mat = matrix_builder.make_np_array()
    # filename = get_matrix_filename()
    # np.save(filename, mat)
    # print(f'Saved to {filename}')
    rank = np.linalg.matrix_rank(mat)
    profiler.finish("rank")
    nonzero_percent = np.count_nonzero(mat) * 100.0 / mat.size
    print(f"{mat.shape} [{nonzero_percent:.2f}% nonzero] => {rank}")



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




# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     expr = QLi2(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 6

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3,Zero,Inf]):
#     expr = QLi2(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 13

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,-x1,-x2,Zero,Inf]):
#     expr = QLi3(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 8

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     expr = QLi3(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 30

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     expr = QLi3(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 17

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3,Zero,Inf]):
#     expr = QLi3(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 48

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6,x7,x8]):
#     expr = QLi3(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 140

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,-x1,-x2,Zero,Inf]):
#     expr = QLi4(args[:4])
#     matrix_builder.add_expr(prepare(expr))
#     expr = QLi4(substitute([1,2,1,3,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 18

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # 81

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     expr = QLi4(args[:4])
#     matrix_builder.add_expr(prepare(expr))
#     expr = QLi4(substitute([1,2,1,3,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 42

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3,Zero,Inf]):
#     expr = QLi4(args[:4])
#     matrix_builder.add_expr(prepare(expr))
#     expr = QLi4(substitute([1,2,1,3,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 168

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     expr = QLi4(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 27

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3,Zero,Inf]):
#     expr = QLi4(args[:4])
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 84

# def prepare(expr):
#     return comultiply(expr, (2,2))
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     expr = QLi4(substitute([1,2,1,3,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  # 15

# def prepare(expr):
#     return comultiply(expr, (2,2))
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3,Zero,Inf]):
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # check: 75

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     expr = QLi4(substitute([1,2,3,4], args))
#     matrix_builder.add_expr(prepare(expr))
#     expr = QLi4(substitute([1,2,1,3,4,5], args))
#     matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)  #

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # 261

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,3,4])
# describe(matrix_builder)  # 114

# def prepare(expr):
#     return comultiply(expr, (2,2))
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # 143

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6,x7,x8]):
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi4, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # 546


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
# describe(matrix_builder)  # 45

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
# describe(matrix_builder)  # 90

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
# describe(matrix_builder)  # 104

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 258

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 234

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
# describe(matrix_builder)  # 27

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
# describe(matrix_builder)  # 63

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 123

# def prepare(expr):
#     return comultiply(expr, (2,3))
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 87

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,-x1,-x2,Zero,Inf]):
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,4,5,6])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, QLi5, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 48


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     add_expr(matrix_builder, prepare, QLi6, args, [1,2,3,4])
# describe(matrix_builder)  # 114


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     func = lambda points : QLiPr(4, points, project_on_x1)
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, func, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # 39

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,-x1,-x2,-x3]):
#     func = lambda points : QLiPr(5, points, project_on_x1)
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4,5,6])
#     add_expr(matrix_builder, prepare, func, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 117

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     func = lambda points : QLiPr(4, points, project_on_x1)
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, func, args, [1,2,1,3,4,5])
# describe(matrix_builder)  # 219

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,-x1,-x2,-x3,-x4]):
#     func = lambda points : QLiPr(5, points, project_on_x1)
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4])
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4,5,6])
#     add_expr(matrix_builder, prepare, func, args, [1,2,1,3,4,5])
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,1,4,5])
# describe(matrix_builder)  # 963

# print(to_lyndon_basis(
#     + QLiSymm4(1,2,3,4,5,6,7,8)
#     + QLiSymm4(1,8,7,6,5,4,3,2)
# ))  # ZERO

# print(to_lyndon_basis(
#     + QLiSymm5(1,2,3,4,5,6,7,8)
#     - QLiSymm5(1,8,7,6,5,4,3,2)
# ))  # ZERO

# print(to_lyndon_basis(
#     + QLiSymm6(1,2,3,4,5,6,7,8)
#     + QLiSymm6(1,8,7,6,5,4,3,2)
# ))  # ZERO

# print(to_lyndon_basis(
#     + QLiSymm4(x1,x2,x3,x4,x5,x6,x7,x8)
#     - QLiSymm4(x2,x1,x8,x7,x6,x5,x4,x3)
# ))  # ZERO

# print(to_lyndon_basis(
#     + QLiSymm5(x1,x2,x3,x4,x5,x6,x7,x8)
#     - QLiSymm5(x2,x1,x8,x7,x6,x5,x4,x3)
# ))  # ZERO

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([1,2,3,4,5,6,7,8]):
#     func = lambda points: (
#         + substitute_variables(QLi3(points), [ x1, x2, x3,-x1,-x2,-x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [ x1, x2,-x3,-x1,-x2, x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [ x1,-x2, x3,-x1, x2,-x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [ x1,-x2,-x3,-x1, x2, x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [-x1, x2, x3, x1,-x2,-x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [-x1, x2,-x3, x1,-x2, x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [-x1,-x2, x3, x1, x2,-x3,Zero,Inf])
#         + substitute_variables(QLi3(points), [-x1,-x2,-x3, x1, x2, x3,Zero,Inf])
#     )
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4])
# describe(matrix_builder)  # 18

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([1,2,3,4,5,6]):
#     func = lambda points: (
#         + substitute_variables(QLi3(points), [ x1, x2, x3,-x1,-x2,-x3])
#         + substitute_variables(QLi3(points), [ x1, x2,-x3,-x1,-x2, x3])
#         + substitute_variables(QLi3(points), [ x1,-x2, x3,-x1, x2,-x3])
#         + substitute_variables(QLi3(points), [ x1,-x2,-x3,-x1, x2, x3])
#         + substitute_variables(QLi3(points), [-x1, x2, x3, x1,-x2,-x3])
#         + substitute_variables(QLi3(points), [-x1, x2,-x3, x1,-x2, x3])
#         + substitute_variables(QLi3(points), [-x1,-x2, x3, x1, x2,-x3])
#         + substitute_variables(QLi3(points), [-x1,-x2,-x3, x1, x2, x3])
#     )
#     add_expr(matrix_builder, prepare, func, args, [1,2,3,4])
# describe(matrix_builder)  # 5




# def prepare(expr):
#     return to_lyndon_basis(expr)
# for args in iterate_permutations([x1,x2,x3,x4,x5,x6,x7]):
#     add_expr(matrix_builder, prepare, QLi3, args, [1,2,3,4])
# describe(matrix_builder)


# expr = (
#     +8*QLiSymm4(1,2,3,4,5,6)
#     - A2(1,2,3,4,5)
#     + A2(1,2,3,4,6)
#     - A2(1,2,3,5,6)
#     + A2(1,2,4,5,6)
#     - A2(1,3,4,5,6)
#     + A2(2,3,4,5,6)
# )
# a = (
#     + QLi4(1,2,3,4)
#     - QLi4(2,3,4,5)
#     + QLi4(3,4,5,6)
#     - QLi4(4,5,6,1)
#     + QLi4(5,6,1,2)
#     - QLi4(6,1,2,3)
# )
# b = (
#     + QLi4(1,2,3,5)
#     - QLi4(2,3,4,6)
#     + QLi4(3,4,5,1)
#     - QLi4(4,5,6,2)
#     + QLi4(5,6,1,3)
#     - QLi4(6,1,2,4)
# )
# # print(comultiply(expr, (2,2)))
# print(to_lyndon_basis(project_on_x1(expr)))
# print(to_lyndon_basis(project_on_x1(a)))
# print(to_lyndon_basis(project_on_x1(b)))
# print(to_lyndon_basis(project_on_x1(expr + 8*a - 4*b)))


# expr = tensor_product([
#     DeltaExpr.single([Delta(x1,x7)]),
#     DeltaExpr.single([Delta(x1,x6)]),
#     DeltaExpr.single([Delta(x1,x2)]),
#     DeltaExpr.single([Delta(x1,x3)]),
#     DeltaExpr.single([Delta(x1,x4)]),
#     DeltaExpr.single([Delta(x1,x5)]),
# ])
# print(expr)
# a = ncomultiply(expr, (2,4))
# print(a)
# b = ncomultiply(a, (2,2,2))
# print(b)


# TODO: Tests for all Bi and all number of arguments
# def prepare(expr):
#     return to_lyndon_basis(expr)
# for s in CB1([x1,x2,x3,x4,x5,x6]):
#     matrix_builder.add_expr(prepare(s))
# describe(matrix_builder)


# def prepare(expr):
#     return ncomultiply(expr, (2,1,1))
# for s1 in Bar('B3xB1').iter(CB3([x1,x2,x3,x4,x5,x6])):
#     for s2 in CB1([x1,x2,x3,x4,x5,x6]):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B2xB2').iter(CB2([x1,x2,x3,x4,x5,x6])):
#     for s2 in CB2([x1,x2,x3,x4,x5,x6]):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)  # 159


# points = [x1,x2,x3,x4,x5,x6,x7]
# def prepare(expr):
#     return ncomultiply(expr, (2,1,1))
# for s1 in Bar('B3xB1').iter(CB3(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B2xB2').iter(CB2(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)  # 624


# points = [x1,x2,x3,x4,x5,x6,x7,x8]
# def prepare(expr):
#     return ncomultiply(expr, (2,1,1))
# for s1 in Bar('B3xB1').iter(CB3(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B2xB2').iter(CB2(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)


# points = [x1,x2,x3,x4,x5]
# def prepare(expr):
#     return ncomultiply(expr, (2,1,1))
# for s1 in Bar('B3xB1').iter(CB3(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B2xB2').iter(CB2(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)  # 25


# points = [x1,x2,x3,x4,x5]
# def prepare(expr):
#     # return expr
#     return ncomultiply(expr, ())
# for s1 in Bar('L4xB1').iter(CL4(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B3xB2').iter(CB3(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)

# points = [x1,x2,x3,x4,x5,x6,x7]
# def prepare(expr):
#     # return expr
#     return ncomultiply(expr)
# for s1 in Bar('L4xB1').iter(CL4(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B3xB2').iter(CB3(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)


# def prepare(expr):
#     return ncomultiply(expr, (1,1,1))
# for s1 in Bar('B2xB1').iter(CB2([x1,x2,x3,x4,x5,x6])):
#     for s2 in CB1([x1,x2,x3,x4,x5,x6]):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)


# for s1 in Bar('B2xB1xB1').iter(CB2([x1,x2,x3,x4,x5,x6])):
#     for s2 in CB1([x1,x2,x3,x4,x5,x6]):
#         for s3 in CB1([x1,x2,x3,x4,x5,x6]):
#             matrix_builder.add_expr(ncoproduct(s1, s2, s3))
# describe(matrix_builder)  # 360


# a = tensor_product([
#     DeltaExpr.single([Delta(x1,x4)]),
#     DeltaExpr.single([Delta(x1,x5)]),
#     DeltaExpr.single([Delta(x1,x6)]),
#     DeltaExpr.single([Delta(x1,x7)]),
# ])
# b = tensor_product([
#     DeltaExpr.single([Delta(x1,x2)]),
#     DeltaExpr.single([Delta(x1,x3)]),
# ])
# print(coproduct(a, b))
# print(ncoproduct(a, b))


# print(coproduct(QLi1(3,4,5,6), QLi2(1,2,7,8)))
# print(ncoproduct(QLi1(3,4,5,6), QLi2(1,2,7,8)))


# print(ncomultiply(ncomultiply(A2(1,2,3,4,5))))
# print(ncomultiply(ncomultiply(QLi4(1,2,3,4,5,6))))

# word = tensor_product([
#     DeltaExpr.single([Delta(Zero,x3)]),
#     DeltaExpr.single([Delta(Zero,x4)]),
#     DeltaExpr.single([Delta(Zero,x1)]),
#     DeltaExpr.single([Delta(Zero,x2)]),
# ])
# a = ncomultiply(word, ())
# b = ncomultiply(a, ())
# # b = ncomultiply(a, (2,1,1))
# print(word)
# print(a)
# print(b)



# def prepare(expr):
#     return to_lyndon_basis(expr)
# for s in Bar('CL5').iter(CL5([x1,x2,x3,x4,x5,x6])):
#     matrix_builder.add_expr(prepare(s))
# describe(matrix_builder)  # 16

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for s in Bar('expr').iter(CL4([x1,x2,x3,x4,x5,x6])):
#     matrix_builder.add_expr(prepare(s))
# describe(matrix_builder)

# points = [x1,x2,x3,x4,x5,x6,x7]
# def prepare(expr):
#     # return expr
#     return ncomultiply(expr, (4,1,1))
# for s1 in Bar('L5xB1').iter(CL5(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('B4xB2').iter(CB4(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# # for s1 in Bar('B3xB3').iter(CB3(points)):
# #     for s2 in CB3(points):
# #         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)


# print(to_lyndon_basis(dihedralize(A2, [1,2,3,4,5], 5)))  # Non-zero
# print(to_lyndon_basis(dihedralize(QLi4, [1,2,3,4], 5)))  # Zero

# Zero: (also for all 4-arg combinations)
# print(to_lyndon_basis(dihedralize(QLi6, [1,2,3,4,5,6], 7)))

# print(to_lyndon_basis(dihedralize(QLi5, [1,2,3,4,5,6], 6)))

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for num_points in range(4, 8):
#     matrix_builder = DeltaExprMatrixBuilder()
#     for s in Bar('matrix').iter(CB2(list(range(1, num_points+1)))):
#         matrix_builder.add_expr(prepare(s))
#     describe(matrix_builder)


# def prepare(expr):
#     return to_lyndon_basis(expr)
# for s in CL4([x1,x2,x3,x4,x5]):
#     matrix_builder.add_expr(prepare(s))
# describe(matrix_builder)
# # matrix_builder.add_expr(prepare(Corr(x1,x2,x3,x4,x5)))
# # matrix_builder.add_expr(prepare(QLi4(x1,x2,x3,x4,x5,Inf)))
# matrix_builder.add_expr(prepare(A2(x1,x2,x3,x4,x4)))
# describe(matrix_builder)

# def prepare(expr):
#     return to_lyndon_basis(expr)
# for s in CL5([x1,x2,x3,x4,x5,x6]):
#     matrix_builder.add_expr(prepare(s))
# describe(matrix_builder)
# matrix_builder.add_expr(prepare(Corr(x1,x2,x3,x4,x5,x6)))
# describe(matrix_builder)


# expr = (
#     # +  A2(1,2,3,4,5)
#     # +8*Corr(1,2,3,4,5)
#     # + dihedralize(Corr, [1,2,2,3,4], 5)
#     # + dihedralize(Corr, [1,2,2,3,5], 5)
#     # + dihedralize(Corr, [2,2,3,4,5], 5)
#     + dihedralize(Corr, [2,2,4,5,1], 5)
# )
# print(sorted_by_num_distinct_variables(to_lyndon_basis(project_on_x1(expr))))

# print(to_lyndon_basis(A2(1,2,3,4,4) - 4*QLi4(1,2,3,4)))  # ZERO


# points = [x1,x2,x3,x4,x5,x6,x7]
# def prepare(expr):
#     return expr
#     # return ncomultiply(expr, (4,1,1))
# for s1 in Bar('L4xB2').iter(CL4(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)
# matrix_builder.add_expr(prepare(ncomultiply(A2(points))))
# describe(matrix_builder)

# def log_x_qli3(args):
#     a1,a2,a3,a4,a5 = args
#     return ncoproduct(Log(a1,a3,a4,a5), QLi3(a1,a2,a3,a4))
# expr = (
#     + ncomultiply(A2(1,2,3,4,5), (1,3))
#     - 4*dihedralize(log_x_qli3, [1,2,3,4,5], 5)
# )
# print(expr)  # ZERO

# points = [x1,x2,x3,x4,x5,x6,x7]
# def prepare(expr):
#     # return expr
#     return ncomultiply(expr, (4,1,1))
# for s1 in Bar('L4xB2').iter(CL4(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)
# def log_x_qli5(args):
#     a1,a2,a3,a4,a5,a6,a7 = args
#     return ncoproduct(Log(a1,a5,a6,a7), QLi5(a1,a2,a3,a4,a5,a6))
# expr = dihedralize(log_x_qli5, [1,2,3,4,5,6,7], 7)
# matrix_builder.add_expr(prepare(expr))
# describe(matrix_builder)

# dexpr = QLi1(1,2,3,4)
# expr = glue_formal_symbol("test", dexpr)
# print(dexpr)
# print(expr)

points = [x1,x2,x3,x4,x5,x6,x7]
for s1 in Bar('L4xB2').iter(CL4_formal(points)):
    for s2 in CB2(points):
        matrix_builder.add_expr(glue_formal_symbol(s1, to_lyndon_basis(s2)))
for p in Bar('B4xLogxB1').iter(list(itertools.combinations(points, 4))):
    for s in CB1(points):
        expr = tensor_product(Log(p), s)
        matrix_builder.add_expr(glue_formal_symbol(f'QLi4({p})', to_lyndon_basis(expr)))
describe(matrix_builder)



# def qli4_fixed(args):
#     a1,a2,a3,a4,a5 = args
#     return QLi4(a1,a2,a3,a4,a5,6)
# expr = dihedralize(qli4_fixed, [1,2,3,4,5], 5)
# print(to_lyndon_basis(project_on_x6(expr)))

# points = [x1,x2,x3,x4]
# for do_comul in [False, True]:
#     matrix_builder = DeltaExprMatrixBuilder()
#     def prepare(expr):
#         return ncomultiply(expr) if do_comul else expr
#         # return expr
#         # return ncomultiply(expr)
#     for s1 in Bar().iter(CB7(points)):
#         for s2 in CB1(points):
#             matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
#     for s1 in Bar().iter(CB6(points)):
#         for s2 in CB2(points):
#             matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
#     for s1 in Bar().iter(CB5(points)):
#         for s2 in CB3(points):
#             matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
#     for s1 in Bar().iter(CB4(points)):
#         for s2 in CB4(points):
#             matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
#     describe(matrix_builder)

# expr = (
#     + ncoproduct(QLi4(1,2,3,4), QLi1(1,2,3,4))
#     - ncoproduct(QLi3(1,2,3,4), QLi2(1,2,3,4))
# )
# print(ncomultiply(expr))  # ZERO

# expr = (
#     + ncoproduct(QLi6(1,2,3,4), QLi1(1,2,3,4))
#     - ncoproduct(QLi5(1,2,3,4), QLi2(1,2,3,4))
#     + ncoproduct(QLi4(1,2,3,4), QLi3(1,2,3,4))
# )
# print(ncomultiply(expr))  # ZERO

# expr = (
#     + ncoproduct(QLi8(1,2,3,4), QLi1(1,2,3,4))
#     - ncoproduct(QLi7(1,2,3,4), QLi2(1,2,3,4))
#     + ncoproduct(QLi6(1,2,3,4), QLi3(1,2,3,4))
#     - ncoproduct(QLi5(1,2,3,4), QLi4(1,2,3,4))
# )
# print(ncomultiply(expr))  # ZERO

# expr = (
#     + QLiSymm5(1,2,3,4,5,6)
#     - QLiSymm5(2,1,6,5,4,3)
# )
# print(to_lyndon_basis(expr))  # ZERO

# expr = (
#     + QLiSymm5(1,2,3,4,5,6)
#     - QLiSymm5(1,6,5,4,3,2)
# )
# print(to_lyndon_basis(expr))  # ZERO

# points = [x1,x2,x3,x4,x5,x6]
# def prepare(expr):
#     return expr
#     # return ncomultiply(expr)
# for s1 in Bar('SL4xB1').iter(CSL4(points)):
#     for s2 in CB1(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# for s1 in Bar('SB3xB2').iter(CSB3(points)):
#     for s2 in CB2(points):
#         matrix_builder.add_expr(prepare(ncoproduct(s1, s2)))
# describe(matrix_builder)
