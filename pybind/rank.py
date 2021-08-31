import numpy as np
import scipy.sparse
import argparse

parser = argparse.ArgumentParser(description='Computes .npz matrix rank.')
parser.add_argument('filename')
args = parser.parse_args()

mat_sparse = scipy.sparse.load_npz(args.filename)
print('matrix loaded')

# mat = mat_sparse.toarray()
# print('dense built')
# rank = np.linalg.matrix_rank(mat)
# print(f'{mat.shape} => {rank}')

unique_rows = set()
mat_lil = mat_sparse.transpose().tolil()
print(f'LIL built')
for row, value in zip(mat_lil.rows, mat_lil.data):
    unique_rows.add(tuple(zip(row, value)))
print(f'{len(unique_rows)} / {len(mat_lil.rows)}')
print('unique rows built')

mat = np.zeros((len(unique_rows), mat_lil.shape[1]))
for row in unique_rows:
    for col, value in row:
        mat[row, col] = value
print('dense built')
rank = np.linalg.matrix_rank(mat)
print(f'{mat.shape} => {rank}')
