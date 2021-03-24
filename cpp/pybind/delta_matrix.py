import numpy as np


class DeltaExprMatrixBuilder:
    def __init__(self):
        self.sparse_columns = []
        self.monoms_to_rows = {}
        self.next_row_id = 0

    def _monom_to_row(self, monom):
        monom_tuple = tuple(monom)
        if not monom_tuple in self.monoms_to_rows:
            self.monoms_to_rows[monom_tuple] = self.next_row_id
            self.next_row_id += 1
        return self.monoms_to_rows[monom_tuple]

    def add_expr(self, expr):
        col = []
        expr.foreach(lambda monom, coeff: col.append((self._monom_to_row(monom), coeff)))
        self.sparse_columns.append(col)

    def make_np_array(self):
        num_rows = self.next_row_id
        columns = []
        for sparse_col in self.sparse_columns:
            col = [0] * num_rows
            for row_id, coeff in sparse_col:
                col[row_id] = coeff
            columns.append(col)
        return np.transpose(np.array(columns, dtype=int))
