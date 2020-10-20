import math
import unittest

from linear import Linear
from lyndon import lyndon_factorize, to_lyndon_basis
from util import generate_all_words


def _is_lyndon_word(word):
    for i in range(1, len(word)):
        if word >= word[i:]:
            return False
    return True


class TestLyndon(unittest.TestCase):

    def __assert_lyndon_factorization(self, words):
        for w in words:
            self.assertTrue(_is_lyndon_word(w), f"Not a Lyndon word: {w}")
        for i in range(len(words) - 1):
            self.assertGreaterEqual(words[i], words[i+1], f"Factorization: {words}")

    def test_lyndon_factorize_golden(self):
        self.assertEqual(
            lyndon_factorize([1, 0, 1, 0, 0, 1, 1, 1, 0, 1]),
            [(1,), (0, 1), (0, 0, 1, 1, 1, 0, 1)]
        )
        self.assertEqual(
            lyndon_factorize([1, 0, 1, 0, 1, 1, 1, 0, 0, 1]),
            [(1,), (0, 1, 0, 1, 1, 1), (0, 0, 1)]
        )

    def test_lyndon_factorize_correctness(self):
        for length in range(1, 12):
            for word in generate_all_words(2, length):
                self.__assert_lyndon_factorization(lyndon_factorize(word))
        for length in range(1, 8):
            for word in generate_all_words(3, length):
                self.__assert_lyndon_factorization(lyndon_factorize(word))
        for length in range(1, 6):
            for word in generate_all_words(4, length):
                self.__assert_lyndon_factorization(lyndon_factorize(word))
            for word in generate_all_words(5, length):
                self.__assert_lyndon_factorization(lyndon_factorize(word))
            for word in generate_all_words(6, length):
                self.__assert_lyndon_factorization(lyndon_factorize(word))

    def test_to_lyndon_basis_empty(self):
        self.assertEqual(
            to_lyndon_basis(Linear({
                (0, 0): 1,
            })).data,
            {}
        )

    def test_to_lyndon_basis_simple(self):
        self.assertEqual(
            to_lyndon_basis(Linear({
                (1, 0): 1,
            })).data,
            {
                (0, 1): -1,
            }
        )

    def test_to_lyndon_basis_preserves_coeff(self):
        self.assertEqual(
            to_lyndon_basis(Linear({
                (1, 0): 42,
            })).data,
            {
                (0, 1): -42,
            }
        )

    def test_to_lyndon_basis_recursive(self):
        self.assertEqual(
            to_lyndon_basis(Linear({
                (1, 0, 0): 1,
            })).data,
            # (1, 0, 0) => - (0, 1, 0) - (0, 0, 1)
            # (0, 1, 0) => -2 (0, 0, 1)
            {
                (0, 0, 1): 1,
            }
        )

    def test_to_lyndon_basis_triple(self):
        self.assertEqual(
            to_lyndon_basis(Linear({
                (0, 1, 2): 1,
                (0, 2, 1): 1,
                (2, 0, 1): 1
            })).data,
            {}
        )


if __name__ == '__main__':
    unittest.main()
