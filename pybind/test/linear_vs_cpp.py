import unittest

from pybind.polykit import Delta, QLi2

from python.polypy.lib.linear import Linear


def arg_a():  return QLi2(1,2,3,4)
def arg_b():  return QLi2(1,2,3,5)

def eval_cpp(code):
    a = arg_a()
    b = arg_b()
    return eval(code)

def eval_python(code):
    a = Linear.from_cpp(arg_a())
    b = Linear.from_cpp(arg_b())
    return eval(code)


class TestLinearVsCpp(unittest.TestCase):

    def compare_expr(self, code):
        self.assertEqual(Linear.from_cpp(eval_cpp(code)), eval_python(code))

    def compare_misc(self, code):
        self.assertEqual(eval_cpp(code), eval_python(code))

    def test_arithmetic(self):
        self.compare_expr('+a')
        self.compare_expr('-a')
        self.compare_expr('a + b')
        self.compare_expr('a - b')
        self.compare_expr('a * 5')
        self.compare_expr('3 * a')
        self.compare_expr('a.mapped(lambda x: tuple(x) + (Delta(1,2),))')
        self.compare_misc('a.l0_norm()')
        self.compare_misc('a.l1_norm()')


if __name__ == '__main__':
    unittest.main()
