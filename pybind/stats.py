import numpy as np
import scipy.sparse
import argparse

parser = argparse.ArgumentParser(description='Computes .npz matrix rank.')
parser.add_argument('filename')
args = parser.parse_args()

mat_sparse = scipy.sparse.load_npz(args.filename)
bytes_per_int = 4
ram_estimate = mat_sparse.shape[0] * mat_sparse.shape[1] * bytes_per_int
print(f'{mat_sparse.shape} ~ {ram_estimate:,}')
