import format

from autosolve import *
from lyndon import lyndon_factorize, to_lyndon_basis
from polylog_gen import *
from powertool import *
from shuffle import *
from tensor import *
from util import *
from word_algebra import *
from word_coalgebra import *


# expr = to_lyndon_basis(project_on_x1(Li4(6)))
# # expr = to_lyndon_basis(Li4(1,2,3,4,5,6,7,8,9,10))
# print_expression("Li4(6) total", expr)
# # print_expression("Li4(10) total", expr, element_to_str=d_monom_to_str)
# print_expression("Li4(6) distinct", words_with_n_distinct_chars(expr, 4))

# exit()

expr = word_comultiply(
    + Li4(1,2,3,4,5,6,7,8,9,10)
    - Li4(2,3,4,5,6,7,8,9,10,1)
    ,
    form=(2,2)
)
print_expression("Comult", expr, element_to_str=d_coproduct_element_to_str)

exit()



n = 6
lhs = Li2_sym(n)

rhs = Linear()
asc = list(range(1, n+1))
for i in range(n):
    for j in range(i+1, n):
        rhs += (-1)**(i-j) * Li2(removed_indices(asc, (i, j)))

print_expression("LHR", d_expr_to_lyndon_basis(lhs), element_to_str=d_monom_to_str)
# print_expression("LHR", d_monoms_with_n_distinct_chars(to_lyndon_basis(lhs), 3), element_to_str=d_monom_to_str)

# print_expression("LHR", to_lyndon_basis(lhs))
# print_expression("RHS", to_lyndon_basis(rhs))
# print_expression("LHS functional expr", lhs.annotations())
# print_expression("RHS functional expr", rhs.annotations())
# print_expression("Diff", to_lyndon_basis(lhs - rhs))
