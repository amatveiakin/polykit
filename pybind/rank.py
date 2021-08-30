import numpy as np
import scipy.sparse
import argparse

parser = argparse.ArgumentParser(description='Computes .npz matrix rank.')
parser.add_argument('filename')
args = parser.parse_args()

mat_sparse = scipy.sparse.load_npz(args.filename)
mat = mat_sparse.toarray()
print('matrix loaded')
rank = np.linalg.matrix_rank(mat)
print(f'{mat.shape} => {rank}')
