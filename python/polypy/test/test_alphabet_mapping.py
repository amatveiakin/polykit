import math
import unittest

from ..lib.alphabet_mapping import d_to_alphabet, d_from_alphabet


class TestAlphabetMapping(unittest.TestCase):

    def test_alphabet_mapping(self):
        self.assertEqual(d_to_alphabet((1, 2)), 0)
        self.assertEqual(d_to_alphabet((4, 5)), 9)
        self.assertEqual(d_from_alphabet(1), (1, 3))
        for i in range(10):
            self.assertEqual(d_to_alphabet(d_from_alphabet(i)), i)


if __name__ == '__main__':
    unittest.main()
