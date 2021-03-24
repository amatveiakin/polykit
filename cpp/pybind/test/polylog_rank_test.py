import itertools
import unittest
import numpy as np

from pybind.delta_matrix import DeltaExprMatrixBuilder
from pybind.polykit import *


class TestPolylogRank(unittest.TestCase):

    def test_five_term_relation(self):
        matrix_builder = DeltaExprMatrixBuilder()
        col_exprs = (
            QLi2(1,2,3,4),
            QLi2(1,2,3,5),
            QLi2(1,2,4,5),
            QLi2(1,3,4,5),
            QLi2(2,3,4,5),
        )
        for expr in col_exprs:
            expr = to_lyndon_basis(project_on_x5(expr))
            matrix_builder.add_expr(expr)
        rank = np.linalg.matrix_rank(matrix_builder.make_np_array())
        self.assertEqual(rank, 3)

    def test_QLi3_arg4(self):
        matrix_builder = DeltaExprMatrixBuilder()
        for args_tail in itertools.permutations([2,3,4]):
            args = [1] + list(args_tail)
            expr = to_lyndon_basis(project_on_x4(QLi3(args)))
            matrix_builder.add_expr(expr)
        rank = np.linalg.matrix_rank(matrix_builder.make_np_array())
        self.assertEqual(rank, 2)

    def test_QLi4_arg4(self):
        matrix_builder = DeltaExprMatrixBuilder()
        for args_tail in itertools.permutations([2,3,4]):
            args = [1] + list(args_tail)
            expr = to_lyndon_basis(project_on_x4(QLi4(args)))
            matrix_builder.add_expr(expr)
        rank = np.linalg.matrix_rank(matrix_builder.make_np_array())
        self.assertEqual(rank, 3)

    def test_QLi3_arg6_arg4_equation(self):
        matrix_builder = DeltaExprMatrixBuilder()
        for args_tmpl in itertools.permutations([1,2,3,4,5,6]):
            args = args_tmpl[:4]
            expr = to_lyndon_basis(project_on_x6(QLi3(args)))
            matrix_builder.add_expr(expr)
        rank_partial = np.linalg.matrix_rank(matrix_builder.make_np_array())
        expr_arg6 = to_lyndon_basis(project_on_x6(QLi3(1,2,3,4,5,6)))
        matrix_builder.add_expr(expr_arg6)
        rank_full = np.linalg.matrix_rank(matrix_builder.make_np_array())
        self.assertEqual(rank_full, rank_partial)

    def test_QLi4_arg6_arg4_equation(self):
        matrix_builder = DeltaExprMatrixBuilder()
        for args_tmpl in itertools.permutations([1,2,3,4,5,6]):
            args = args_tmpl[:4]
            expr = to_lyndon_basis(project_on_x6(QLi4(args)))
            matrix_builder.add_expr(expr)
        rank_partial = np.linalg.matrix_rank(matrix_builder.make_np_array())
        expr_arg6 = to_lyndon_basis(project_on_x6(QLi4(1,2,3,4,5,6)))
        matrix_builder.add_expr(expr_arg6)
        rank_full = np.linalg.matrix_rank(matrix_builder.make_np_array())
        self.assertEqual(rank_full, rank_partial + 1)
        expr_arg6_rorated = to_lyndon_basis(project_on_x6(QLi4(2,3,4,5,6,1)))
        matrix_builder.add_expr(expr_arg6_rorated)
        rank_extra = np.linalg.matrix_rank(matrix_builder.make_np_array())
        self.assertEqual(rank_extra, rank_full)


if __name__ == '__main__':
    unittest.main()
