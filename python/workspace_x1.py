import itertools
import math
import numpy as np

from profiler import Profiler


# templates = [
#     [
#         (+1, [1,2,6,7,3,4,5]),
#         (+1, [1,4,5,6,3,2,7]),
#         (+1, [1,5,3,4,6,7,2]),
#         (+1, [1,7,2,3,6,5,4]),
#     ],
#     [
#         (-1, [1,2,4,5,7,6,3]),
#         (+1, [1,5,2,7,4,3,6]),
#         (-1, [1,5,6,7,4,3,2]),
#         (+1, [1,6,4,5,7,2,3]),
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


# templates = [
#     [
#         (+1, [1,2,6,7,3,4,5]),
#         (+1, [1,4,5,6,3,2,7]),
#         (+1, [1,5,3,4,6,7,2]),
#         (+1, [1,7,2,3,6,5,4]),
#     ],
#     [
#         (-1, [1,2,4,5,7,6,3]),
#         (+1, [1,5,2,7,4,3,6]),
#         (-1, [1,5,6,7,4,3,2]),
#         (+1, [1,6,4,5,7,2,3]),
#     ],
# ]


# templates = [
#     [
#         (-1, [1,2,4,5,8,6,3,7]),
#         (-1, [1,2,5,6,8,4,3,7]),
#         (-1, [1,4,2,8,5,7,3,6]),
#         (+1, [1,4,7,8,5,2,3,6]),
#         (+1, [1,6,2,8,5,7,3,4]),
#         (-1, [1,6,7,8,5,2,3,4]),
#         (+1, [1,7,4,5,8,6,3,2]),
#         (+1, [1,7,5,6,8,4,3,2]),
#     ],
#     [
#         (+1, [1,2,3,4,8,5,6,7]),
#         (+1, [1,2,3,5,8,4,6,7]),
#         (-1, [1,4,2,3,5,8,7,6]),
#         (+1, [1,4,3,8,5,2,7,6]),
#         (-1, [1,5,2,3,4,8,7,6]),
#         (+1, [1,5,3,8,4,2,7,6]),
#         (+1, [1,6,2,7,4,8,3,5]),
#         (+1, [1,6,2,7,5,8,3,4]),
#         (-1, [1,6,7,8,4,2,3,5]),
#         (-1, [1,6,7,8,5,2,3,4]),
#         (+1, [1,7,4,6,2,5,3,8]),
#         (+1, [1,7,4,6,8,5,3,2]),
#         (+1, [1,7,5,6,2,4,3,8]),
#         (+1, [1,7,5,6,8,4,3,2]),
#         (+1, [1,8,3,4,2,5,6,7]),
#         (+1, [1,8,3,5,2,4,6,7]),
#     ],
#     # [
#     #     (+1, [1,2,3,4,8,5,6,7]),
#     #     (-1, [1,2,4,5,8,3,6,7]),
#     #     (+1, [1,3,2,4,5,8,7,6]),
#     #     (-1, [1,3,4,8,5,2,7,6]),
#     #     (+1, [1,5,2,4,3,8,7,6]),
#     #     (-1, [1,5,4,8,3,2,7,6]),
#     #     (+1, [1,6,2,7,3,8,4,5]),
#     #     (+1, [1,6,2,7,5,8,4,3]),
#     #     (-1, [1,6,7,8,3,2,4,5]),
#     #     (-1, [1,6,7,8,5,2,4,3]),
#     #     (+1, [1,7,3,6,2,5,4,8]),
#     #     (+1, [1,7,3,6,8,5,4,2]),
#     #     (+1, [1,7,5,6,2,3,4,8]),
#     #     (+1, [1,7,5,6,8,3,4,2]),
#     #     (+1, [1,8,3,4,2,5,6,7]),
#     #     (-1, [1,8,4,5,2,3,6,7]),
#     # ],
#     # [
#     #     (-1, [1,2,3,4,8,5,6,7]),
#     #     (+1, [1,2,4,5,8,3,6,7]),
#     #     (-1, [1,3,2,5,6,4,8,7]),
#     #     (-1, [1,3,4,5,6,2,8,7]),
#     #     (+1, [1,4,2,3,8,5,6,7]),
#     #     (+1, [1,4,2,5,8,3,6,7]),
#     #     (-1, [1,5,2,3,6,4,8,7]),
#     #     (+1, [1,5,3,4,6,2,8,7]),
#     #     (-1, [1,7,2,8,6,4,3,5]),
#     #     (-1, [1,7,2,8,6,4,5,3]),
#     #     (-1, [1,7,3,6,8,5,2,4]),
#     #     (-1, [1,7,3,6,8,5,4,2]),
#     #     (-1, [1,7,4,8,6,2,3,5]),
#     #     (-1, [1,7,4,8,6,2,5,3]),
#     #     (-1, [1,7,5,6,8,3,2,4]),
#     #     (-1, [1,7,5,6,8,3,4,2]),
#     # ],
# ]


templates = [
    [
        (+1, [1,2,3,4,5,6]),
        (-1, [4,2,3,1,5,6]),
    ],
    [
        (+1, [1,2,3,4,5,6]),
        (-1, [1,5,3,4,2,6]),
    ],
    [
        (+1, [1,2,3,4,5,6]),
        (-1, [1,2,6,4,5,3]),
    ],
    [
        (+1, [1,2,3,4,5,6]),
        (-1, [1,3,2,4,6,5]),
    ],
    [
        (+1, [1,2,3,4,5,6]),
        (+1, [2,3,4,5,6,1]),
        (+1, [3,4,5,6,1,2]),
    ],
]


M = 6


# Parameters:
#   - `good_permutations_index` - every (1/good_permutations_index) permutation
#     must satisfy `is_good_permutation`;
#   - `is_good_permutation(p)` - a predicate on permutations;
#   - `make_good_permutation_with_sign(p)` - changes `p` in place so that is
#     satisfies `is_good_permutation`; returns the sign of the permutation
#     that was used to do so.


# good_permutations_index = 4

# def is_good_permutation(p):
#     return p[0] > p[1] and p[2] > p[3]

# def make_good_permutation_with_sign(p):
#     sign = 1
#     if p[0] < p[1]:
#         p[0], p[1] = p[1], p[0]
#         # sign *= -1
#     if p[2] < p[3]:
#         p[2], p[3] = p[3], p[2]
#         sign *= -1
#     return sign


good_permutations_index = 1

def is_good_permutation(p):
    return True

def make_good_permutation_with_sign(p):
    return 1


profiler = Profiler()

N = math.factorial(M)
assert(N % good_permutations_index == 0)
num_rows = N // good_permutations_index
num_cols = N * len(templates)
the_matrix = np.zeros((num_rows, num_cols), dtype=int)
permutation_to_index = {}
index_to_permutation = []

for perm in itertools.permutations(range(1, 1+M)):
    if is_good_permutation(perm):
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
            value = make_good_permutation_with_sign(term_permuted) * coeff
            the_matrix[permutation_to_index[tuple(term_permuted)], col_idx] = value
        col_idx += 1

profiler.finish("make matrix")

# np.set_printoptions(threshold=np.inf)
print(the_matrix)
print(the_matrix.shape)
print()
rank = np.linalg.matrix_rank(the_matrix)
profiler.finish("compute rank")
print("Rank = {}".format(rank))
print("Corank = {}".format(min(the_matrix.shape) - rank))
