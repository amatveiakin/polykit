import format

from tensor import Inf, Tensor, d_monom_to_str
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from autosolve import *
from polylog_gen import *
from shuffle import *
from util import *
from word_algebra import *
from word_coalgebra import *


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
