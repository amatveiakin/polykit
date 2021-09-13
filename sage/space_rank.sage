# TODO: Why doesn't this work?
Parallelism().set(nproc=32)

num_rows = 0
num_cols = 0
triplets = []
for line in sys.stdin:
    row, col, value = [int(x) for x in line.split()]
    num_rows = max(num_rows, row + 1)
    num_cols = max(num_cols, col + 1)
    triplets.append((row, col, value))
# mat = matrix(ZZ, num_rows, num_cols)
mat = matrix(ZZ, num_rows, num_cols, sparse=True)
for row, col, value in triplets:
    mat[row, col] = value

dim = mat.rank(algorithm='linbox')
print(dim)
