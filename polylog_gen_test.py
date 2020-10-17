import math
import unittest

from linear import Linear
from lyndon import to_lyndon_basis
from tensor import Tensor
from polylog_gen import Li


class TestShuffle(unittest.TestCase):

    def test_li_2_6_equation(self):
        remainder = to_lyndon_basis(
            Li(2, 6) -
            (Li(2, [1,3,5,6]) - Li(2, [1,4,5,6]) - Li(2, [2,3,5,6]) + Li(2, [2,4,5,6]))
        )
        self.assertTrue(remainder == Linear(), "\n" + str(Tensor(remainder)))


if __name__ == '__main__':
    unittest.main()
