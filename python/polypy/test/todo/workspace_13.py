import format

from autosolve import *
from iterated_integral_gen import *
from lyndon import lyndon_factorize, to_lyndon_basis
from polylog_gen import *
from powertool import *
from shuffle import *
from tensor import *
from util import *
from word_algebra import *
from word_coalgebra import *


expr = I(1,2,3,4,5,6)

print_expression("Before Lyndon", expr, element_to_str=d_monom_to_str)
lyndon = d_expr_to_lyndon_basis(expr)
print_expression("After Lyndon", lyndon, element_to_str=d_monom_to_str)
d_expr_check_integratability(expr)

lhs = word_comultiply(expr, form=(2,2))
rhs = (
    + word_coproduct(I(1,2,3,4), I(1,4,5,6))
    + word_coproduct(I(2,3,4,5), I(1,2,5,6))
    + word_coproduct(I(3,4,5,6), I(1,2,3,6))
)

print_expression("LHS", lhs, element_to_str=d_coproduct_element_to_str)
print_expression("RHS", rhs, element_to_str=d_coproduct_element_to_str)
print_expression("Diff", lhs - rhs, element_to_str=d_coproduct_element_to_str)
