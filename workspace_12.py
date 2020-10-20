import format

from autosolve import *
from lyndon import lyndon_factorize, to_lyndon_basis
from polylog_gen import *
from shuffle import *
from tensor import *
from util import *
from word_algebra import *
from word_coalgebra import *


n = 6
lhs = Li2_sym(n)

rhs = Linear()
asc = list(range(1, n+1))
for i in range(n):
    for j in range(i+1, n):
        rhs += (-1)**(i-j) * Li2(remove_indices(asc, (i, j)))

format.print_expression("LHR", d_expr_to_lyndon_basis(lhs), element_to_str=d_monom_to_str)
# format.print_expression("LHR", d_monoms_with_n_distinct_chars(to_lyndon_basis(lhs), 3), element_to_str=d_monom_to_str)

# format.print_expression("LHR", to_lyndon_basis(lhs))
# format.print_expression("RHS", to_lyndon_basis(rhs))
# format.print_expression("LHS functional expr", lhs.annotations())
# format.print_expression("RHS functional expr", rhs.annotations())
# format.print_expression("Diff", to_lyndon_basis(lhs - rhs))
