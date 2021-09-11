ALGORITHM = 'linbox'
# ALGORITHM = 'flint'

weight = 6
num_points = 5
prefix = f'/mnt/c/Danya/results/l_vs_m/w{weight}_p{num_points}_'

def load_matrix(suffix):
    num_rows = 0
    num_cols = 0
    triplets = []
    for line in open(f'{prefix}{suffix}.triplets', 'r'):
        row, col, value = [int(x) for x in line.split()]
        num_rows = max(num_rows, row + 1)
        num_cols = max(num_cols, col + 1)
        triplets.append((row, col, value))
    m = matrix(ZZ, num_rows, num_cols)
    for row, col, value in triplets:
        m[row, col] = value
    return m

a = load_matrix('a')
b = load_matrix('b')
united = load_matrix('united')
print('Matrices loaded')
a_dim = a.rank(algorithm=ALGORITHM)
print('Dim A computed')
b_dim = b.rank(algorithm=ALGORITHM)
print('Dim B computed')
united_dim = united.rank(algorithm=ALGORITHM)
print('Dim united computed')
intersection = a_dim + b_dim - united_dim
print(f'w={weight}, p={num_points}: ({a_dim}, {b_dim}), ∪ = {united_dim}, ∩ = {intersection}')
