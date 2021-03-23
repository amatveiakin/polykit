import itertools

from profiler import Profiler

from polykit import to_lyndon_basis
from polykit import coproduct, comultiply
from polykit import X, Inf, Delta, substitute_variables, project_on, involute, involute_projected
from polykit import QLi, QLi1, QLi2, QLi3, QLi4, QLi5, QLi6, QLi7, QLi8
from polykit import QLiNeg, QLiNeg1, QLiNeg2, QLiNeg3, QLiNeg4, QLiNeg5, QLiNeg6, QLiNeg7, QLiNeg8
from polykit import QLiSymm, QLiSymm1, QLiSymm2, QLiSymm3, QLiSymm4, QLiSymm5, QLiSymm6, QLiSymm7, QLiSymm8
from polykit import project_on, project_on_x1, project_on_x2, project_on_x3, project_on_x4, project_on_x5, project_on_x6, project_on_x7, project_on_x8, project_on_x9, project_on_x10, project_on_x11, project_on_x12, project_on_x13, project_on_x14, project_on_x15


# e1 = QLi2(1,2,3,4,5,6)
# assert to_lyndon_basis(project_on_x1(e1)) == project_on_x1(to_lyndon_basis(e1))

# expr = QLi1(1,2,3,4) + QLi1(1,3,2,4)
# print(expr)
# print(comultiply(QLi3(1,2,3,Inf), (1,2)))


def substitute(points, new_indices):
    return [new_indices[p - 1] for p in points]

# TODO: Remove `checksum` placeholder; build a matrix and compute rank
profiler = Profiler()
checksum = 0
for arg_order in itertools.permutations([1,2,3,4,5]):
    expr = to_lyndon_basis(project_on_x5(QLi6(substitute([1,2,1,3,4,5], arg_order))))
    checksum += expr.l1_norm()
profiler.finish("all")
print("Checksum = {}".format(checksum))
