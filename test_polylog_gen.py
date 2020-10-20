import math
import unittest

from linear import Linear
from lyndon import to_lyndon_basis
from tensor import d_monom_to_str
from polylog_gen import *


class TestShuffle(unittest.TestCase):

    def test_li_2_6_equation(self):
        remainder = to_lyndon_basis(
            Li(2, 6) -
            (Li2(1,3,5,6) - Li2(1,4,5,6) - Li2(2,3,5,6) + Li2(2,4,5,6))
        )
        self.assertTrue(remainder == Linear(), "\n" + remainder.to_str(element_to_str=d_monom_to_str))

    def test_li_sym(self):
        self.assertTrue(to_lyndon_basis(Li2_sym(1,2,3,4,5,6) + Li2_sym(2,3,4,5,6,1)) == Linear())
        self.assertTrue(to_lyndon_basis(Li3_sym(1,2,3,4,5,6) - Li3_sym(2,3,4,5,6,1)) == Linear())
        self.assertTrue(to_lyndon_basis(Li4_sym(1,2,3,4,5,6) + Li4_sym(2,3,4,5,6,1)) == Linear())



if __name__ == '__main__':
    unittest.main()
