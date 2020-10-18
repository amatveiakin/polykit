import time

import format

from tensor import Inf, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from polylog_gen import *
from polylog import *
from shuffle import *
from util import *
from word_algebra import *


class Profiler:
    def __init__(self):
        self.start = time.time()

    def finish(self, stage_name):
        end = time.time()
        print(f"Profiler: {stage_name} took {end - self.start} seconds")
        self.start = end

# format.set_enable_unicode(True)
# format.set_enable_monospace_font_correction(False)

# t = Tensor(Li2_p6(1, 2, 3, 4, 5, 6))
# print(str(t))
# print("----")
# q = Tensor(Li2(1, 3, 5, 6) - Li2(1, 4, 5, 6) - Li2(2, 3, 5, 6) + Li2(2, 4, 5, 6))
# print(str(q))
# print("----")
# diff = Tensor(t.summands - q.summands)
# diff.convert_to_lyndon_basis()
# print(diff)

# t = Tensor(Li(2, 6))
# print(str(t))
# print("----")
# q = Tensor(Li(2, [1,3,5,6]) - Li(2, [1,4,5,6]) - Li(2, [2,3,5,6]) + Li(2, [2,4,5,6]))
# print(str(q))
# print("----")
# diff = Tensor(t.summands - q.summands)
# diff.convert_to_lyndon_basis()
# print(diff)

# t = Tensor(Li5_p6(1, 2, 3, 4, 5, 6))
# # print(t)
# t.check_criterion()

# profiler = Profiler()
# # l = Li5_p6(1, 2, 3, 4, 5, 6)
# # l = Li(6, [1,2,3,4,5,6,7,Inf])
# l = Li(8, [1,2,3,4,5,6,7,8,9,10])
# profiler.finish("gen_Li")
# words_before_lyndon = project_on_xi(l, 1)
# profiler.finish("project_on_xi")
# words = to_lyndon_basis(words_before_lyndon)
# profiler.finish("to_lyndon_basis")
# print("")
# print(f"Before Lyndon - {len(words_before_lyndon)} terms:\n{words_before_lyndon}\n")
# print(f"After Lyndon - {len(words)} terms:\n{words}\n")
# threshold = 8
# words_filtered = Linear({k : v for k, v in words.items() if len(set(k)) >= threshold})
# print(f"After Lyndon, at least {threshold} different - {len(words_filtered)} terms:\n{words_filtered}\n")
# end = time.time()

# t = Tensor(Li2_p6(1, 2, 3, 4, 5, 6))
# t = Tensor(Li(6, 8))
# t.check_criterion()
# print(f"Before Lyndon - {len(t.summands)} terms:\n{t}\n")
# t.convert_to_lyndon_basis()
# print(f"After Lyndon - {len(t.summands)} terms:\n{t}\n")

def print_annotated(msg, expr):
    print(f"{msg} - {len(expr)} terms:\n{expr}\n")

goal_before_lyndon = (
    + Li4(1,2,3,4,5,6)
    + Li4(2,3,4,5,6,1)
    - (
        + Li4(1,2,3,4)
        - Li4(1,2,3,6)
        + Li4(1,2,5,6)
        - Li4(1,4,5,6)
        + Li4(2,3,4,5)
        + Li4(3,4,5,6)
    )
)
goal = to_lyndon_basis(goal_before_lyndon.without_annotations())
# print_annotated("Goal before Lyndon", goal_before_lyndon)
print_annotated("Goal", goal)

# print_annotated("Tmpl 1", to_lyndon_basis(project_on_x1(Li4(1,2,3,4))))
# print_annotated("Tmpl 1 a", to_lyndon_basis(project_on_x1(Li4(1,Inf,3,4))))
# print_annotated("Tmpl 1 b", to_lyndon_basis(project_on_x1(Li4(1,2,Inf,4))))
# print_annotated("Tmpl 1 c", to_lyndon_basis(project_on_x1(Li4(1,2,3,Inf))))
# print_annotated("Tmpl 2", to_lyndon_basis(project_on_x1(Li4(1,3,4,2))))
# print_annotated("Tmpl 3", to_lyndon_basis(project_on_x1(Li4(1,4,2,3))))

def aaab(a, b):
    return (
        - Li4(1,a,b,Inf)
    )

def aabb(a, b):
    return (
        - Li4(1,Inf,a,b)
        - Li4(1,a,Inf,b)
        - Li4(1,a,b,Inf)
    )

def abbb(a, b):
    return (
        - Li4(1,Inf,a,b)
    )

def aabc_abcc(a, b, c):
    return (
        + Li4(1,a,b,c)
        - Li4(1,Inf,b,c)
        - Li4(1,a,Inf,c)
        - Li4(1,a,b,Inf)
    )

# expr_raw = (
#     + Li4(1,2,3,4)
#     - Li4(1,4,5,6)
#     + aaab(2,3)
#     - aaab(2,5)
#     + abbb(3,6)
#     - abbb(5,6)
#     - aabc_abcc(2,3,6)
#     + aabc_abcc(2,5,6)
# )
# expr_raw = (
#     + Li4(2,3,4,5)
# )
expr_raw = (
    + Li4(3,4,5,6)
)
expr = project_on_x3(expr_raw)
print_annotated("Expr", to_lyndon_basis(expr))
print_annotated("Functional expr", expr_raw.annotations())

print_annotated("Diff", to_lyndon_basis(expr - goal))
