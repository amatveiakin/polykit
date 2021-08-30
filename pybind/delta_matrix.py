import numpy as np
import scipy.sparse

from pybind.polykit import substitute_variables
from python.polypy.lib.util import to_hashable


def matrix_area(mat):
    return mat.shape[0] * mat.shape[1]

def sparse_to_rows(mat):
    rows = [[] for _ in range(mat.shape[0])]
    mat_coo = mat.tocoo()
    for row, col, value in zip(mat_coo.row, mat_coo.col, mat_coo.data):
        rows[row].append((col, value))
    return set([tuple(r) for r in rows])

def compress_sparse(mat):
    area = matrix_area(mat)
    transposed_at_least_once = False
    shape_log = [mat.shape]
    while True:
        sparse_rows = sparse_to_rows(mat)
        rows = []
        cols = []
        data = []
        row_id = 0
        for row in sparse_rows:
            for col_id, value in row:
                rows.append(row_id)
                cols.append(col_id)
                data.append(value)
            row_id += 1
        mat = scipy.sparse.coo_matrix((data, (rows, cols)), dtype=int)
        new_area = matrix_area(mat)
        if new_area == area and transposed_at_least_once:
            print(' => '.join([str(s) for s in shape_log]))
            return mat
        assert new_area <= area
        mat = mat.transpose()
        transposed_at_least_once = True
        area = new_area
        shape_log.append(mat.shape)


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
        # t_row = tuple(row)
        # if t_row in self.sparse_rows:
        #     # print(f'duplicate row - discarding')
        #     pass
        # elif not t_row:
        #     # print(f'zero row - discarding')
        #     pass
        # else:
        #     old_len = len(self.sparse_rows)
        #     self.sparse_rows.add(t_row)
        #     assert len(self.sparse_rows) == old_len + 1, f'\n===\n{self.sparse_rows}\n---\n{t_row}\n'

    # def add_expr_permutations(self, expr, num_points, num_args):
    #     assert num_points >= num_args
    #     for points in itertools.permutations(range(num_points)):
    #         # TODO: Optimize case num_points >= num_args+1 (skip tail permutations)
    #         self.add_expr(substitute_variables(expr, points[num_args:]))

    # def sparse_to_array(self, mat_sparse):
    #     print(f'### A =\n{mat_sparse.toarray()} => {np.linalg.matrix_rank(mat_sparse.toarray())}')
    #     mat_csr = mat_sparse.transpose().tocsr()
    #     # print(f'mat_csr.data = {mat_csr.data}')
    #     # print(f'mat_csr.indices = {mat_csr.indices}')
    #     # print(f'mat_csr.indptr = {mat_csr.indptr}')
    #     print(f'shape = {mat_csr.shape}')
    #     print(f'nnz = {mat_csr.nnz}')
    #     # for q in mat_csr:
    #     #     print(type(q))
    #     #     print(q)
    #     sparse_to_rows(mat_csr)
    #     exit(0)
    #     # unique_rows = set()
    #     # mat_lil = mat_sparse.transpose().tolil()
    #     # print(f'### B =\n{mat_lil.toarray()} => {np.linalg.matrix_rank(mat_lil.toarray())}')
    #     # for row, value in zip(mat_lil.rows, mat_lil.data):
    #     #     print(f'row = {row}')
    #     #     print(f'value = {value}')
    #     #     unique_rows.add(tuple(zip(row, value)))
    #     # print(unique_rows)
    #     # mat = np.zeros((len(unique_rows), mat_lil.shape[1]), dtype=int)
    #     # for row in unique_rows:
    #     #     for col, value in row:
    #     #         mat[row, col] = value
    #     # print(f'### C =\n{mat}')
    #     # return mat

    # def make_np_array(self):
    #     return self.sparse_to_array(self.make_sp_sparse())

    def make_np_array(self):
        return compress_sparse(self.make_sp_sparse()).toarray()

    # def make_np_array(self):
    #     num_columns = self.next_column_id
    #     rows = []
    #     for sparse_row in self.sparse_rows:
    #         row = [0] * num_columns
    #         for column_id, coeff in sparse_row:
    #             row[column_id] = coeff
    #         rows.append(row)
    #     return np.array(rows, dtype=int)

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
        return scipy.sparse.csc_matrix((data, (rows, cols)), dtype=int)
