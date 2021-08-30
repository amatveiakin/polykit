import numpy as np
import scipy.sparse

from pybind.polykit import substitute_variables
from python.polypy.lib.util import to_hashable


class DeltaExprMatrixBuilder:
    def __init__(self):
        self.sparse_rows = []
        self.monoms_to_columns = {}
        self.next_column_id = 0

    def _monom_to_column(self, monom):
        monom_tuple = to_hashable(monom)
        if not monom_tuple in self.monoms_to_columns:
            self.monoms_to_columns[monom_tuple] = self.next_column_id
            self.next_column_id += 1
        return self.monoms_to_columns[monom_tuple]

    def add_expr(self, expr):
        row = []
        for monom, coeff in expr:
            row.append((self._monom_to_column(monom), coeff))
        self.sparse_rows.append(row)

    # def add_expr_permutations(self, expr, num_points, num_args):
    #     assert num_points >= num_args
    #     for points in itertools.permutations(range(num_points)):
    #         # TODO: Optimize case num_points >= num_args+1 (skip tail permutations)
    #         self.add_expr(substitute_variables(expr, points[num_args:]))

    def make_np_array(self):
        num_columns = self.next_column_id
        rows = []
        for sparse_row in self.sparse_rows:
            row = [0] * num_columns
            for column_id, coeff in sparse_row:
                row[column_id] = coeff
            rows.append(row)
        return np.array(rows, dtype=int)

    def make_sp_sparse(self):
        data = []
        rows = []
        cols = []
        row_id = 0
        for sparse_row in self.sparse_rows:
            for column_id, coeff in sparse_row:
                data.append(coeff)
                rows.append(row_id)
                cols.append(column_id)
            row_id += 1
        # return scipy.sparse.csc_matrix((data, indices), shape=(len(self.sparse_rows), self.next_column_id), dtype=int)
        return scipy.sparse.csc_matrix((data, (rows, cols)), dtype=int)
