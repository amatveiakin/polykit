import math
import unittest

from ..lib.linear import Linear
from ..lib.lyndon import to_lyndon_basis
from ..lib.delta import d_monom_to_str
from ..lib.polylog_qli import *


class TestShuffle(unittest.TestCase):

    def test_li_2_6_equation(self):
        remainder = to_lyndon_basis(
            QLi2(1,2,3,4,5,6) -
            (QLi2(1,3,5,6) - QLi2(1,4,5,6) - QLi2(2,3,5,6) + QLi2(2,4,5,6))
        )
        self.assertTrue(remainder == Linear(), "\n" + remainder.to_str(element_to_str=d_monom_to_str))

    def test_li_sym(self):
        self.assertTrue(to_lyndon_basis(QLiSymm2(1,2,3,4,5,6) + QLiSymm2(2,3,4,5,6,1)) == Linear())
        self.assertTrue(to_lyndon_basis(QLiSymm3(1,2,3,4,5,6) - QLiSymm3(2,3,4,5,6,1)) == Linear())
        self.assertTrue(to_lyndon_basis(QLiSymm4(1,2,3,4,5,6) + QLiSymm4(2,3,4,5,6,1)) == Linear())



if __name__ == '__main__':
    unittest.main()
