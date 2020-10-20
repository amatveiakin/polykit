import format

from tensor import *
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from autosolve import *
from polylog_gen import *
from shuffle import *
from util import *
from word_algebra import *
from word_coalgebra import *


def Li_6_points_fixed(weight, x1,x2,x3,x4,x5,x6):
    return (
        + Li(weight, [x1,x2,x3,x4,x5,x6])
        - (
        + Li(weight, [x1,x2,x3,x4])
        + Li(weight, [x3,x4,x5,x6])
        + Li(weight, [x5,x6,x1,x2])
        ).without_annotations()
        # - Li(weight, [x2,x3,x4,x5])
        # - Li(weight, [x4,x5,x6,x1])
        # - Li(weight, [x6,x1,x2,x3])
    )

def Li3_fixed(*points):
    return Li_6_points_fixed(3, *points)

def Li4_fixed(*points):
    return Li_6_points_fixed(4, *points)

def Li5_fixed(*points):
    return Li_6_points_fixed(5, *points)

def Li6_fixed(*points):
    return Li_6_points_fixed(6, *points)


def Li_8_points_fixed(weight, x1,x2,x3,x4,x5,x6,x7,x8):
    return (
        + Li(weight, [x1,x2,x3,x4,x5,x6,x7,x8])
        - (
        + Li_6_points_fixed(weight, *[x1,x2,x3,x4,x5,x6])
        + Li_6_points_fixed(weight, *[x3,x4,x5,x6,x7,x8])
        + Li_6_points_fixed(weight, *[x5,x6,x7,x8,x1,x2])
        + Li_6_points_fixed(weight, *[x7,x8,x1,x2,x3,x4])
        ).without_annotations()
    )


# criterion_expr = Li3_fixed(1,2,3,4,5,6) - Li3_fixed(2,3,4,5,6,1)
# criterion_expr = Li4_fixed(1,2,3,4,5,6) + Li4_fixed(2,3,4,5,6,1)
# criterion_expr = Li5_fixed(1,2,3,4,5,6) - Li5_fixed(2,3,4,5,6,1)
criterion_expr = Li_8_points_fixed(5, 1,2,3,4,5,6,7,8) - Li_8_points_fixed(5, 2,3,4,5,6,7,8,1)
# criterion_expr = Li6_fixed(1,2,3,4,5,6) + Li6_fixed(2,3,4,5,6,1)
criterion_expr = criterion_expr.without_annotations()
criterion_ok = is_zero(criterion_expr)
print(f"Criterion ok = {criterion_ok}")
if not criterion_ok:
    format.print_expression("Leftovers", to_lyndon_basis(project_on_x1(criterion_expr)))
    exit()
print()

# # expr = (
# #     + Li4_fixed(1,2,3,4,5,6)
# #     + Li4_fixed(2,3,4,5,6,7)
# #     + Li4_fixed(3,4,5,6,7,1)
# #     + Li4_fixed(4,5,6,7,1,2)
# #     + Li4_fixed(5,6,7,1,2,3)
# #     + Li4_fixed(6,7,1,2,3,4)
# #     + Li4_fixed(7,1,2,3,4,5)
# # )
# lhs = Li3_fixed(1,2,3,4,5,6)
# rhs = Linear()
# # for x1 in range(1, 7):
# #     for x2 in range(x1+1, 7):
# #         for x3 in range(x2+1, 7):
# #             for x4 in range(x3+1, 7):
# #                 expr += Li3_fixed(x1, x2, x3, x4)
# asc = list(range(1, 7))
# for i in range(6):
#     for j in range(i+1, 6):
#         rhs += (-1)**(i - j) * Li3(asc[0:i] + asc[i+1:j] + asc[j+1:])
# expr = lhs - rhs
# format.print_expression("RHS", rhs.annotations())
# # format.print_expression("Functional expression", expr.annotations())
# format.print_expression("Projection on x1", to_lyndon_basis(project_on_x1(expr)))
# print(f"Zero = {is_zero(expr)}")


lhs = Li_8_points_fixed(5, 1,2,3,4,5,6,7,8)
rhs = Linear()
weight = 8
asc = list(range(1, weight+1))
for i in range(weight):
    for j in range(i+1, weight):
        rhs += (-1)**(i - j) * Li_6_points_fixed(5, *(asc[0:i] + asc[i+1:j] + asc[j+1:]))
expr = lhs - rhs
format.print_expression("RHS", rhs.annotations())
# format.print_expression("Functional expression", expr.annotations())
format.print_expression("Projection on x1", to_lyndon_basis(project_on_x1(expr)))
print(f"Zero = {is_zero(expr)}")

exit()

# expr = Li6(1,2,3,4,5,6,7,8) - Li6(3,4,5,6,7,8,1,2)
# # format.print_expression("Diff", to_lyndon_basis(project_on_x1(expr)))
# print(f"Zero = {is_zero(expr)}")
# exit()


expr = Linear()
num_points = 7
asc = list(range(1, num_points+1))
for i in range(num_points):
    expr += (-1)**i * Li4(asc[0:i] + asc[i+1:])

format.print_expression("Formula", expr.annotations())
# t = Tensor(expr.without_annotations())
# t.convert_to_lyndon_basis()
# print(t)
format.print_expression("Projected on x1", to_lyndon_basis(project_on_x1(expr)))
