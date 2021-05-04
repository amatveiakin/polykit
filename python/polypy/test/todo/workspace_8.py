import itertools
import time

import format

from tensor import Inf, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from autosolve import *
from polylog_gen import *
from shuffle import *
from util import *
from word_algebra import *


e = (
(
    + Li3(1,2,3,4,5,6)
    - Li3(2,3,4,5,6,1)
)
-
(
    + Li3(1,2,3,4)
    - Li3(2,3,4,5)
    + Li3(3,4,5,6)
    - Li3(4,5,6,1)
    + Li3(5,6,1,2)
    - Li3(6,1,2,3)
)
)

# print(to_lyndon_basis(project_on_xi(e, 3)))

# t = Tensor(e.without_annotations())
# t.convert_to_lyndon_basis()
# print(t)

# exit()


def normalize_Li3(word):
    w = tuple(word)
    assert len(w) == 4
    m = min(w)
    while w[0] != m:
        w = rotate_list(w, 1)
    if w[3] < w[1]:
        w[3], w[1] = w[1], w[3]
    return tuple(w)

expr = (
    + W(1,2,3,4)
    - W(1,2,3,6)
    + W(1,2,5,6)
    - W(1,4,5,6)
    - W(2,3,4,5)
    + W(3,4,5,6)
)

n = 6
expr_sum = Linear()
for i in range(n):
    index_map = {j+1: (j+i)%n+1 for j in range(n)}
    expr_sum += (5-i) * word_expr_substitute(expr, index_map)
format.print_expression("Raw", expr_sum)
normalized = expr_sum.mapped_obj(lambda w: normalize_Li3(w))
# normalized_verbous = expr_sum.mapped_obj(lambda w: f"{w} -> {normalize_Li3(w)}")
# format.print_expression("Normalized - verbous", normalized_verbous)
format.print_expression("Normalized", normalized)
