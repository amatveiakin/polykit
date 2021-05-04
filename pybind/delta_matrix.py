import numpy as np

from polykit import substitute_variables
from python.polypy.lib.util import to_hashable


class DeltaExprMatrixBuilder:
    def __init__(self):
        self.sparse_columns = []
        self.monoms_to_rows = {}
        self.next_row_id = 0

    def _monom_to_row(self, monom):
        monom_tuple = to_hashable(monom)
        if not monom_tuple in self.monoms_to_rows:
            self.monoms_to_rows[monom_tuple] = self.next_row_id
            self.next_row_id += 1
        return self.monoms_to_rows[monom_tuple]

    def add_expr(self, expr):
        col = []
        for monom, coeff in expr:
            col.append((self._monom_to_row(monom), coeff))
        self.sparse_columns.append(col)

    # def add_expr_permutations(self, expr, num_points, num_args):
    #     assert num_points >= num_args
    #     for points in itertools.permutations(range(num_points)):
    #         # TODO: Optimize case num_points >= num_args+1 (skip tail permutations)
    #         self.add_expr(substitute_variables(expr, points[num_args:]))

    def make_np_array(self):
        num_rows = self.next_row_id
        columns = []
        for sparse_col in self.sparse_columns:
            col = [0] * num_rows
            for row_id, coeff in sparse_col:
                col[row_id] = coeff
            columns.append(col)
        return np.transpose(np.array(columns, dtype=int))
