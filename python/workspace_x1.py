import itertools
import math
import numpy as np


# templates = [
#     [
#         (-1, [1,2,6,7,3,4,5]),
#         (-1, [1,4,5,6,3,2,7]),
#         (-1, [1,5,3,4,6,7,2]),
#         (-1, [1,7,2,3,6,5,4]),
#     ],
#     [
#         (+1, [1,2,4,5,7,6,3]),
#         (-1, [1,6,4,5,7,2,3]),
#         (+1, [1,5,6,7,4,3,2]),
#         (-1, [1,5,2,7,4,3,6]),
#     ],
#     [
#         (-1, [1,2,3,4,7,6,5]),
#         (+1, [2,6,3,4,7,1,5]),
#         (-1, [1,4,2,3,5,6,7]),
#         (+1, [4,6,2,3,5,1,7]),
#     ],
#     [
#         (+1, [1,2,3,4,5,6,7]),
#         (+1, [2,1,3,4,5,6,7]),
#     ],
#     [
#         (+1, [1,2,3,4,5,6,7]),
#         (+1, [1,2,4,3,5,6,7]),
#     ],
# ]

templates = [
    [
        (+1, [1,2,6,7,3,4,5]),
        (+1, [1,4,5,6,3,2,7]),
        (+1, [1,5,3,4,6,7,2]),
        (+1, [1,7,2,3,6,5,4]),
    ],
    [
        (-1, [1,2,4,5,7,6,3]),
        (+1, [1,5,2,7,4,3,6]),
        (-1, [1,5,6,7,4,3,2]),
        (+1, [1,6,4,5,7,2,3]),
    ],
    # [
    #     (+1, [1,2,3,4,7,6,5]),
    #     (+1, [1,4,2,3,5,6,7]),
    #     (-1, [2,6,3,4,7,1,5]),
    #     (-1, [4,6,2,3,5,1,7]),
    # ],
    [
        (+1, [1,2,3,4,5,6,7]),
        (+1, [2,1,3,4,5,6,7]),
    ],
    [
        (+1, [1,2,3,4,5,6,7]),
        (+1, [1,2,4,3,5,6,7]),
    ],
]


M = 7
N = math.factorial(M)
num_rows = N
num_cols = N * len(templates)
the_matrix = np.zeros((num_rows, num_cols), dtype=int)
permutation_to_index = {}
index_to_permutation = []

for perm in itertools.permutations(range(1, 1+M)):
    permutation_to_index[tuple(perm)] = len(index_to_permutation)
    index_to_permutation.append(perm)
assert len(index_to_permutation) == num_rows, len(index_to_permutation)
# print(index_to_permutation)

col_idx = 0
for perm in itertools.permutations(range(M)):
    for tmpl in templates:
        for coeff, term in tmpl:
            term_permuted = [0] * len(term)
            for i in range(len(perm)):
                term_permuted[i] = perm[term[i] - 1] + 1
            the_matrix[permutation_to_index[tuple(term_permuted)], col_idx] = coeff
        col_idx += 1

# np.set_printoptions(threshold=np.inf)
print(the_matrix)
print(the_matrix.shape)
print()
print("Rank = {}".format(np.linalg.matrix_rank(the_matrix)))
