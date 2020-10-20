import format

from tensor import *
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from autosolve import *
from polylog_gen import *
from powertool import *
from shuffle import *
from util import *
from word_algebra import *
from word_coalgebra import *


def Li2_lynd(*args):
    return to_lyndon_basis(Li2(*args))

def monom_indices(monom):
    return set(flatten([[d.a, d.b] for d in monom]))

def coproduct_monom_indices(monom):
    return monom_indices(monom[0]) | monom_indices(monom[1])


# t = Tensor(Li4(1,2,3,4,5,6).without_annotations())
# t.convert_to_lyndon_basis()
# lhs = normalize_n_n(word_comultiply(t.summands, form=(2, 2)))

# TODO: Alphabet mapping
# lhs = normalize_n_n(word_comultiply(Li4(1,2,3,4,5,6), form=(2, 2)))
# rhs = (
#     + word_coproduct(Li2_lynd(1,2,3,4), Li2_lynd(1,4,5,6))
#     + word_coproduct(Li2_lynd(1,2,5,6), Li2_lynd(5,2,3,4))
#     - word_coproduct(Li2_lynd(1,2,3,6), Li2_lynd(3,4,5,6))
# )

# expr = (
#     + Li4(1,2,3,4,5,6)
# )

# expr = (
# (
#     + Li4(1,2,3,4,5,6)
#     + Li4(2,3,4,5,6,1)
# )
# -
# (
#     + Li4(1,2,3,4)
#     + Li4(2,3,4,5)
#     + Li4(3,4,5,6)
#     + Li4(4,5,6,1)
#     + Li4(5,6,1,2)
#     + Li4(6,1,2,3)
# )
# )

# expr = (
# (
#     + Li5(1,2,3,4,5,6)
#     - Li5(2,3,4,5,6,1)
# )
# -
# (
#     + Li5(1,2,3,4)
#     - Li5(2,3,4,5)
#     + Li5(3,4,5,6)
#     - Li5(4,5,6,1)
#     + Li5(5,6,1,2)
#     - Li5(6,1,2,3)
# )
# )


# print(to_lyndon_basis(Li4_sym(1,2,3,4,5,6) + Li4_sym(2,3,4,5,6,1)))
# exit()


# lhs_func = (
#     + 2 * Li4_sym(1,2,3,4,5,6)
#     + 2 * cyclic_sum(Li4, 6, 4, alternating=True)
#     - (
#         + Li4(1,2,3,5)
#         - Li4(2,3,4,6)
#         + Li4(3,4,5,1)
#         - Li4(4,5,6,2)
#         + Li4(5,6,1,3)
#         - Li4(6,1,2,4)
#     )
# )

def lhs_func(x1,x2,x3,x4,x5,x6):
    return (
        + 2 * Li4_sym(x1,x2,x3,x4,x5,x6)
        + 2 * (
            + Li4(x1,x2,x3,x4)
            - Li4(x2,x3,x4,x5)
            + Li4(x3,x4,x5,x6)
            - Li4(x4,x5,x6,x1)
            + Li4(x5,x6,x1,x2)
            - Li4(x6,x1,x2,x3)
        )
        - (
            + Li4(x1,x2,x3,x5)
            - Li4(x2,x3,x4,x6)
            + Li4(x3,x4,x5,x1)
            - Li4(x4,x5,x6,x2)
            + Li4(x5,x6,x1,x3)
            - Li4(x6,x1,x2,x4)
        )
    )



# n = 7
# q_lhs = cyclic_sum(Li4_sym, n, 6, alternating=False)
# q_rhs = Linear()
# for x1 in range(1, n+1):
#     for x2 in range(x1+1, n+1):
#         for x3 in range(x2+1, n+1):
#             for x4 in range(x3+1, n+1):
#                 q_rhs += Li4(x1,x2,x3,x4)

# format.print_expression(
#     "Cyclic sum",
#     to_lyndon_basis(q_lhs + q_rhs),
#     element_to_str=d_monom_to_str)


# def f(x1,x2,x3,x4,x5):
#     return (
#         + W(x1,x2,x3,x4)
#         + W(x2,x3,x4,x5)
#         + W(x3,x4,x5,x1)
#         + W(x4,x5,x1,x2)
#         + W(x5,x1,x2,x3)
#     )
# print(cyclic_sum(f, 6, 5, alternating=True))

total = (
    + Li4(1, 2, 3, 5)
    + Li4(1, 2, 5, 7)
    + Li4(1, 3, 4, 5)
    + Li4(1, 3, 4, 7)
    + Li4(1, 3, 6, 7)
    + Li4(2, 3, 4, 6)
    + Li4(2, 3, 6, 1)
    + Li4(2, 4, 5, 1)
    + Li4(2, 4, 5, 6)
    + Li4(2, 4, 7, 1)
    + Li4(3, 4, 5, 7)
    + Li4(3, 4, 7, 2)
    + Li4(3, 5, 6, 2)
    + Li4(3, 5, 6, 7)
    + Li4(3, 7, 1, 2)
    + Li4(4, 1, 2, 3)
    + Li4(4, 5, 6, 1)
    + Li4(4, 6, 1, 2)
    + Li4(4, 6, 7, 1)
    + Li4(4, 6, 7, 3)
    + Li4(5, 2, 3, 4)
    + Li4(5, 6, 1, 3)
    + Li4(5, 6, 7, 2)
    + Li4(5, 7, 1, 4)
    + Li4(5, 7, 2, 3)
    + Li4(6, 1, 2, 5)
    + Li4(6, 1, 3, 4)
    + Li4(6, 3, 4, 5)
    + Li4(6, 7, 1, 5)
    + Li4(6, 7, 2, 4)
    + Li4(7, 1, 2, 6)
    + Li4(7, 1, 3, 5)
    + Li4(7, 2, 3, 6)
    + Li4(7, 2, 4, 5)
    + Li4(7, 4, 5, 6)
)

format.print_expression(
    "Diff",
    to_lyndon_basis(
        + cyclic_sum(Li4_sym, 7, 6, alternating=False)
        - total
    ),
    element_to_str=d_monom_to_str)


# def word_to_diffs(w):
#     last_c = None
#     ret = []
#     for c in w:
#         if last_c is not None:
#             d = abs(c - last_c)
#             d1 = 7 - abs(d)
#             q = d if d < d1 else d1
#             ret.append(q)
#         last_c = c
#     return tuple(ret)

# positive = Linear()
# for word, coeff in total.items():
#     w = list(word)
#     if coeff < 0:
#         w = rotate_list(w, 1)
#     if sum(word_to_diffs(w)) % 2 == 1:
#         w = rotate_list(w, 2)
#     positive += Linear({tuple(w): 1})

# format.print_expression("Fixed signs", positive)
# format.print_expression("Fixed signs", positive.mapped_obj(word_to_diffs))

# base = (
#     + W(1,2,3,4)
#     + W(1,2,3,5)
#     - W(1,2,3,6)
#     - W(1,2,3,7)
#     + W(1,2,4,6)
#     - W(1,2,4,7)
#     + W(1,2,5,7)
#     + W(1,2,6,7)
#     + W(1,3,4,5)
#     - W(1,3,4,6)
#     + W(1,3,5,6)
#     - W(1,3,5,7)
#     + W(1,3,6,7)
#     - W(1,4,5,6)
#     - W(1,4,6,7)
#     - W(1,5,6,7)
#     + W(2,3,4,5)
#     + W(2,3,4,6)
#     - W(2,3,4,7)
#     + W(2,3,5,7)
#     + W(2,4,5,6)
#     - W(2,4,5,7)
#     + W(2,4,6,7)
#     - W(2,5,6,7)
#     + W(3,4,5,6)
#     + W(3,4,5,7)
#     + W(3,5,6,7)
#     + W(4,5,6,7)
# )

# def f(x1,x2,x3,x4,x5,x6):
#     return (
#         + W(x1,x2,x3,x4)
#         + W(x3,x4,x5,x6)
#         + W(x5,x6,x1,x2)
#     )

# print(base - cyclic_sum(f, 7, 6, alternating=False))

# format.print_expression(
#     "Cyclic sum",
#     to_lyndon_basis(cyclic_sum(lhs_func, 7, 6, alternating=False)),
#     element_to_str=d_monom_to_str)

exit()



lhs = word_comultiply(
    lhs_func(1,2,3,4,5,6),
    form=(3,1)
)

def func(x1,x2,x3,x4,x5):
    return (
        + word_coproduct(Li3(x1,x2,x3,x4),  cross_ratio(x3,x4,x5,x1) - cross_ratio(x4,x5,x1,x2))
        + word_coproduct(Li3(x2,x3,x4,x5),  cross_ratio(x4,x5,x1,x2) - cross_ratio(x5,x1,x2,x3))
        + word_coproduct(Li3(x3,x4,x5,x1),  cross_ratio(x5,x1,x2,x3) - cross_ratio(x1,x2,x3,x4))
        + word_coproduct(Li3(x4,x5,x1,x2),  cross_ratio(x1,x2,x3,x4) - cross_ratio(x2,x3,x4,x5))
        + word_coproduct(Li3(x5,x1,x2,x3),  cross_ratio(x2,x3,x4,x5) - cross_ratio(x3,x4,x5,x1))
    )

# expr = cyclic_sum(func, 6, 5, alternating=True)
rhs = (
    + func(1,2,3,4,5)
    - func(2,3,4,5,6)
    + func(3,4,5,6,1)
    - func(4,5,6,1,2)
    + func(5,6,1,2,3)
    - func(6,1,2,3,4)
)

diff = lhs - rhs
# diff = lhs
# diff = rhs
format.print_expression(
    "Diff",
    diff,
    element_to_str=d_coproduct_element_to_str,
)

print("===")

format.print_expression(
    ">= 4 variables",
    diff.filtered_obj(lambda word: len(coproduct_monom_indices(word)) >= 4),
    element_to_str=d_coproduct_element_to_str,
)

print("===")

format.print_expression(
    ">= 5 variables",
    diff.filtered_obj(lambda word: len(coproduct_monom_indices(word)) >= 5),
    element_to_str=d_coproduct_element_to_str,
)

# print("\n".join([str(coproduct_monom_indices(word)) for word, _ in coproduct.items()]))

# lhs = normalize_n_n(word_comultiply(Li4(1,2,3,4,5,6), form=(2, 2)))
# rhs = (
#     + word_coproduct(Li2_lynd(1,2,3,4), Li2_lynd(1,4,5,6))
#     + word_coproduct(Li2_lynd(1,2,5,6), Li2_lynd(5,2,3,4))
#     - word_coproduct(Li2_lynd(1,2,3,6), Li2_lynd(3,4,5,6))
# )

# coproduct = word_comultiply(expr, form=(1,3))
# format.print_expression(
#     "Coproduct",
#     coproduct,
#     element_to_str=d_coproduct_element_to_str,
# )

# print("===")
# # print("\n".join([str(coproduct_monom_indices(word)) for word, _ in coproduct.items()]))
# format.print_expression(
#     ">= 5 variables",
#     coproduct.filtered_obj(lambda word: len(coproduct_monom_indices(word)) >= 5),
#     element_to_str=d_coproduct_element_to_str,
# )
