import unittest

from ..lib.util import generate_all_words


class TestUtil(unittest.TestCase):

    def test_generate_all_words_1_1(self):
        self.assertEqual(
            list(generate_all_words(1, 1)),
            [
                (0,),
            ]
        )

    def test_generate_all_words_2_3(self):
        self.assertEqual(
            list(generate_all_words(2, 3)),
            [
                (0, 0, 0),
                (0, 0, 1),
                (0, 1, 0),
                (0, 1, 1),
                (1, 0, 0),
                (1, 0, 1),
                (1, 1, 0),
                (1, 1, 1),
            ]
        )

    def test_generate_all_words_3_2(self):
        self.assertEqual(
            list(generate_all_words(3, 2)),
            [
                (0, 0),
                (0, 1),
                (0, 2),
                (1, 0),
                (1, 1),
                (1, 2),
                (2, 0),
                (2, 1),
                (2, 2),
            ]
        )


if __name__ == '__main__':
    unittest.main()
