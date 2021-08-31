import numpy as np
import scipy.sparse
import argparse

parser = argparse.ArgumentParser(description='Computes .npz matrix rank.')
parser.add_argument('filename')
args = parser.parse_args()

mat_sparse = scipy.sparse.load_npz(args.filename)
bytes_per_int = 4
ram_estimate = mat_sparse.shape[0] * mat_sparse.shape[1] * bytes_per_int
print(f'{mat_sparse.shape} ~ {ram_estimate:,}B as dense')

# mat_coo = mat_sparse.tocoo()
# for row, col, value in zip(mat_coo.row, mat_coo.col, mat_coo.data):

# i = 0
# for row in mat_sparse.transpose().tocsr():
# for row in mat_sparse.transpose().tolil():
#     print(row)
#     print('---')
#     i += 1
#     if i > 100:
#         break

unique_rows = set()
mat_lil = mat_sparse.transpose().tolil()
print(f'LIL built')
for row, value in zip(mat_lil.rows, mat_lil.data):
    unique_rows.add(tuple(zip(row, value)))
print(f'{len(unique_rows)} / {len(mat_lil.rows)}')
