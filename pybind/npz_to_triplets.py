import scipy.sparse
import argparse

parser = argparse.ArgumentParser(description='Converts .npz matrix to triplets (row, col, value)')
parser.add_argument('filename')
args = parser.parse_args()

mat = scipy.sparse.load_npz(args.filename).tocoo()
for row, col, value in zip(mat.row, mat.col, mat.data):
    print(row, col, value)
