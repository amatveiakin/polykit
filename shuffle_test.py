import math
import unittest

from shuffle import shuffle_product, shuffle_product_many


class TestShuffle(unittest.TestCase):

    def test_shuffle_1_1(self):
        self.assertCountEqual(
            shuffle_product((1,), (2,)),
            [
                (1, 2),
                (2, 1),
            ]
        )

    def test_shuffle_2_1(self):
        self.assertCountEqual(
            shuffle_product((1, 2), (3,)),
            [
                (1, 2, 3),
                (1, 3, 2),
                (3, 1, 2),
            ]
        )

    def test_shuffle_2_2(self):
        self.assertCountEqual(
            shuffle_product((1, 2), (3, 4)),
            [
                (1, 2, 3, 4),
                (1, 3, 2, 4),
                (1, 3, 4, 2),
                (3, 1, 2, 4),
                (3, 1, 4, 2),
                (3, 4, 1, 2),
            ]
        )

    def test_shuffle_2_2_2(self):
        self.assertCountEqual(
            shuffle_product_many([(1, 1), (1, 1), (1, 1)]),
            [(1, 1, 1, 1, 1, 1)] * (math.comb(6, 2) * math.comb(4, 2))
        )


if __name__ == '__main__':
    unittest.main()
