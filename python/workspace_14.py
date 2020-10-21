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
expr = Li6(1,2,3,4,5,6,7,8)
profiler.finish("expr")
expr = d_expr_to_lyndon_basis(expr)
profiler.finish("lyndon")
print_expression("Expr", expr)
