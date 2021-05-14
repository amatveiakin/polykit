import math
import unittest

from ..lib.delta import *
from ..lib.polylog_qli import *


class TestDExpr(unittest.TestCase):

    def test_checks(self):
        expr = QLi2(1,2,3,4,5,6)
        d_expr_check_simple_invariants(expr)
        d_expr_check_integratability(expr)

    def test_lyndon(self):
        expr = QLi2(1,2,3,4,5,6)
        lyndon_d_expr = d_expr_to_lyndon_basis(expr)
        lyndon_generic = to_lyndon_basis(expr)
        self.assertEqual(lyndon_d_expr, lyndon_generic)



if __name__ == '__main__':
    unittest.main()
