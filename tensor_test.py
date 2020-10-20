import math
import unittest

from tensor import *
from polylog_gen import *
from profiler import Profiler


class TestDExpr(unittest.TestCase):

    def test_checks(self):
        expr = Li2(1,2,3,4,5,6)
        d_expr_check_simple_invariants(expr)
        d_expr_check_integratability(expr)

    def test_lyndon(self):
        expr = Li2(1,2,3,4,5,6)
        profiler = Profiler(active=False)  # note: use bigger weight for profiling
        lyndon_d_expr = d_expr_to_lyndon_basis(expr)
        profiler.finish("d_expr_to_lyndon_basis")
        lyndon_generic = to_lyndon_basis(expr)
        profiler.finish("to_lyndon_basis")
        self.assertEquals(lyndon_d_expr, lyndon_generic)



if __name__ == '__main__':
    unittest.main()
