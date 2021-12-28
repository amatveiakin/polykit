import numpy as np
import scipy.sparse

from pybind.polykit import substitute_variables
from python.polypy.lib.util import to_hashable


def _to_unique_sparse_columns(sparse_rows, num_columns):
    sparse_columns = [[] for _ in range(num_columns)]
    row_id = 0
    for row in sparse_rows:
        for col_id, value in row:
            sparse_columns[col_id].append((row_id, value))
        row_id += 1
    return set([tuple(col) for col in sparse_columns])


# TODO: Rename to ExprMatrixBuilder; rename the file.
class DeltaExprMatrixBuilder:
    def __init__(self):
        self.sparse_rows = set()
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
        self.sparse_rows.add(tuple(row))

    def make_np_array(self):
        return self.make_sp_sparse().toarray()

    def make_sp_sparse(self):
        data = []
        rows = []
        cols = []
        col_id = 0
        sparse_columns = _to_unique_sparse_columns(self.sparse_rows, self.next_column_id)
        for sparse_col in sparse_columns:
            for row_id, coeff in sparse_col:
                data.append(coeff)
                rows.append(row_id)
                cols.append(col_id)
            col_id += 1
        return scipy.sparse.csc_matrix((data, (rows, cols)), dtype=int)
