import format

from autosolve import *
from iterated_integral_gen import *
from lyndon import lyndon_factorize, to_lyndon_basis
from polylog_gen import *
from powertool import *
from profiler import Profiler
from shuffle import *
from tensor import *
from util import *
from word_algebra import *
from word_coalgebra import *


profiler = Profiler()
# expr = Li6(1,2,3,4,5,6,7,8)
expr = Li6(1,2,3,4,5,6)
profiler.finish("expr")
print(f"Num terms before Lyndon = {len(expr.without_annotations())}")
lyndon = d_expr_to_lyndon_basis(expr)
profiler.finish("lyndon")
print(f"Num terms after Lyndon = {len(lyndon.without_annotations())}")

# print_expression("Expr", expr)
