import math
import unittest

from alphabet_mapping import AlphabetMapping


class TestAlphabetMapping(unittest.TestCase):

    def test_alphabet_mapping(self):
        mapping = AlphabetMapping(5)  # alphabet size is 10
        self.assertEqual(mapping.to_alphabet((1, 2)), 0)
        self.assertEqual(mapping.to_alphabet((4, 5)), 9)
        self.assertEqual(mapping.from_alphabet(1), (1, 3))
        for i in range(10):
            self.assertEqual(mapping.to_alphabet(mapping.from_alphabet(i)), i)


if __name__ == '__main__':
    unittest.main()
