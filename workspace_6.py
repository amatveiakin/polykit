import itertools
import time

import format

from tensor import Inf, Tensor
from lyndon import lyndon_factorize, to_lyndon_basis
from enumerator import Enumerator
from autosolve import *
from polylog_gen import *
from shuffle import *
from util import *
from word_algebra import *


def print_with_title(msg, expr):
    print(f"{msg} - {len(expr)} terms:\n{expr}\n")


e1 = (
    + Li5(1,2,3,4,5,6,7,8)
)
e2 = (
    + Li5(1,2,3,4,5,7)
    - Li5(1,2,3,4,5,8)
    - Li5(1,2,3,4,6,7)
    + Li5(1,2,3,4,6,8)
    + Li5(1,2,3,5,6,7)
    - Li5(1,2,3,5,6,8)
    + Li5(1,2,3,5,7,8)
    - Li5(1,2,3,6,7,8)
    - Li5(1,2,4,5,6,7)
    + Li5(1,2,4,5,6,8)
    - Li5(1,2,4,5,7,8)
    + Li5(1,2,4,6,7,8)
    + Li5(1,3,4,5,6,7)
    - Li5(1,3,4,5,6,8)
    + Li5(1,3,4,5,7,8)
    - Li5(1,3,4,6,7,8)
    + Li5(1,3,5,6,7,8)
    - Li5(1,4,5,6,7,8)
    - Li5(2,3,4,5,6,7)
    + Li5(2,3,4,5,6,8)
    - Li5(2,3,4,5,7,8)
    + Li5(2,3,4,6,7,8)
    - Li5(2,3,5,6,7,8)
    + Li5(2,4,5,6,7,8)
)
e3 = (
    + Li5(1,2,3,7)
    - Li5(1,2,3,8)
    - Li5(1,2,4,7)
    + Li5(1,2,4,8)
    + Li5(1,3,4,7)
    - Li5(1,3,4,8)
    - Li5(1,3,5,7)
    + Li5(1,3,5,8)
    + Li5(1,4,5,7)
    - Li5(1,4,5,8)
    - Li5(1,4,6,7)
    + Li5(1,4,6,8)
    + Li5(1,5,6,7)
    - Li5(1,5,6,8)
    + Li5(1,5,7,8)
    - Li5(1,6,7,8)
    - Li5(2,3,4,7)
    + Li5(2,3,4,8)
    + Li5(2,3,5,7)
    - Li5(2,3,5,8)
    - Li5(2,4,5,7)
    + Li5(2,4,5,8)
    + Li5(2,4,6,7)
    - Li5(2,4,6,8)
    - Li5(2,5,6,7)
    + Li5(2,5,6,8)
    - Li5(2,5,7,8)
    + Li5(2,6,7,8)
)
lhs = e1
rhs = e2 + e3
e = lhs - rhs

print_with_title("LHS", e1.annotations())
print("=")
annotations_sorted = Linear(
    {k: v for k, v in
    sorted(rhs.annotations().items(), key=lambda item: (-len(item[0].name), item[0].name))}
)
print(annotations_sorted)
# print_with_title("RHS - 6", e2.annotations())
# print_with_title("RHS - 4", e3.annotations())
print("~~~")

t = Tensor(e.without_annotations())
t.convert_to_lyndon_basis()
print(t)
exit()



# weight = 5
# points = 8
# goal_raw = Li(weight, points)
# goal = to_lyndon_basis(project_on_x1(goal_raw.without_annotations()))
# print_with_title("Before eliminations", goal)
# elim_annotations = Linear()

# index_combinations = generate_all_words(alphabet_size=points, length=6)
# for indices_raw in index_combinations:
#     indices = [idx + 1 for idx in indices_raw]
#     elim = Li(weight, indices, projector=project_on_x1)
#     expr_expr = to_lyndon_basis(elim.without_annotations())
#     eliminate(goal, elim_annotations, expr_expr, elim.annotations(), min_distinct=3)

# print_with_title("After eliminations", goal)
# print_with_title("Elimination expression", elim_annotations)

# exit()


# e = (
# (
#     + Li5(1,2,3,4,5,6,7,8)
# )
# +
# (
#     - Li5(1,2,3,4,5,7)
#     + Li5(1,2,3,4,5,8)
#     + Li5(1,2,3,4,6,7)
#     - Li5(1,2,3,4,6,8)
#     - Li5(1,2,3,5,6,7)
#     + Li5(1,2,3,5,6,8)
#     - Li5(1,2,3,5,7,8)
#     + Li5(1,2,3,6,7,8)
#     + Li5(1,2,4,5,6,7)
#     - Li5(1,2,4,5,6,8)
#     + Li5(1,2,4,5,7,8)
#     - Li5(1,2,4,6,7,8)
#     - Li5(1,3,4,5,6,7)
#     + Li5(1,3,4,5,6,8)
#     - Li5(1,3,4,5,7,8)
#     + Li5(1,3,4,6,7,8)
#     - Li5(1,3,5,6,7,8)
#     + Li5(1,4,5,6,7,8)
# )
# # +
# # (
# #     - Li5(1,2,3,7)
# #     + Li5(1,2,3,8)
# #     + Li5(1,2,4,7)
# #     - Li5(1,2,4,8)
# #     - Li5(1,3,4,7)
# #     + Li5(1,3,4,8)
# #     + Li5(1,3,5,7)
# #     - Li5(1,3,5,8)
# #     - Li5(1,4,5,7)
# #     + Li5(1,4,5,8)
# #     + Li5(1,4,6,7)
# #     - Li5(1,4,6,8)
# #     - Li5(1,5,6,7)
# #     + Li5(1,5,6,8)
# #     - Li5(1,5,7,8)
# #     + Li5(1,6,7,8)
# # )
# )
# l = to_lyndon_basis(project_on_x1(e.without_annotations()))
# print_with_title("Before eliminations", l)
# # exit()

# elim_annotations = Linear()
# for a in [1]:
#     for b in range(1, 9):
#         for c in range(1, 9):
#             for d in range(1, 9):
#                 elim = Li(5, [a,b,c,d], projector=project_on_x1)
#                 expr_expr = to_lyndon_basis(elim.without_annotations())
#                 eliminate(l, elim_annotations, expr_expr, elim.annotations(), min_distinct=3)

# print_with_title("After eliminations", l)
# print_with_title("Elimination expression", elim_annotations)
# # print("---")
# # print(words_with_n_distinct_chars(l, 4))
# # print("---")
# # print(words_with_n_distinct_chars(l, 5))
# exit()


# # print(to_lyndon_basis(project_on_x1(Li5(1,2,3,4))))
# # print(words_with_n_distinct_chars(to_lyndon_basis(project_on_x1(Li5(1,2,3,4))), 5))
# # print()
# # print(words_with_n_distinct_chars(project_on_x1(Li5(1,2,3,4,5,6,7,8)), 5))

# # exit()


num_points = 6
weight = 5
known_eliminations_5 = {}
known_eliminations_3 = {}

find_eliminations(known_eliminations_5, project_on_x1(
    + Li5(1,2,3,4,5,6)
    ,
    keep_annotations=True,
), num_points, weight, projection_axis=1, min_distinct=5)

find_eliminations(known_eliminations_3, project_on_x1(
    + Li5(1,2,3,4)
    ,
    keep_annotations=True,
), num_points, weight, projection_axis=1, min_distinct=3)

# find_eliminations(known_eliminations, project_on_x1(
#     + Li5(1,2,3,4,5,6)
#     ,
#     keep_annotations=True,
# ), num_points, weight, projection_axis=1)
# find_eliminations(known_eliminations, project_on_x1(
#     + Li5(1,2,3,4,5,6)
#     - Li5(2,3,4,5,6,1)
#     ,
#     keep_annotations=True,
# ), num_points, weight, projection_axis=1)
# find_eliminations(known_eliminations, project_on_x1(
#     + Li5(1,2,3,4,5,6)
#     + Li5(1,3,2,4,5,6)
#     + Li5(1,3,4,2,5,6)
#     + Li5(1,3,4,5,2,6)
#     + Li5(1,3,4,5,6,2)
#     ,
#     keep_annotations=True,
# ), num_points, weight, projection_axis=1)
# find_eliminations(known_eliminations, project_on_x1(
#     + Li5(1,2,3,4,5,6)
#     + Li5(2,1,3,4,5,6)
#     + Li5(2,3,1,4,5,6)
#     + Li5(2,3,4,1,5,6)
#     + Li5(2,3,4,5,1,6)
#     ,
#     keep_annotations=True,
# ), num_points, weight, projection_axis=1)


def print_eliminations(eliminations, min_distinct):
    print(f"\n~~~ Known eliminations >= {min_distinct} ~~~\n")
    for _, v in eliminations.items():
        print(f"{v[0]}\nincluding\n{words_with_n_distinct_chars(v[0], min_distinct)}\nvia\n{v[1]}\n")
    print()

print_eliminations(known_eliminations_5, 5)
print_eliminations(known_eliminations_3, 3)


goal_orig = Li(5, 8)

goal_raw = goal_orig.copy()
for axis in range(1, 4):
    print(f"\n\n=== Projecting on x{axis} ===\n")
    goal_before_lyndon = project_on_xi(goal_raw, axis)
    goal = to_lyndon_basis(goal_before_lyndon)
    print_with_title("Goal functional expr", goal_raw.annotations())
    # print_with_title("Goal before Lyndon", goal_before_lyndon)
    print_with_title("Goal", goal)

    eliminated_5 = apply_eliminations(known_eliminations_5, goal, axis, min_distinct=5)
    eliminated_3 = apply_eliminations(known_eliminations_3, eliminated_5.without_annotations(), axis, min_distinct=3)
    # eliminated_prelast = apply_eliminations(known_eliminations, eliminated_5.without_annotations(), axis)
    # eliminated_last = apply_eliminations(known_eliminations, eliminated_prelast.without_annotations(), axis)
    eliminated = eliminated_3.without_annotations()
    # eliminated = eliminated_5.without_annotations()
    # eliminated = eliminated_last.without_annotations()
    eliminated_annotations = (eliminated_5 + eliminated_3).annotations()
    # eliminated_annotations = eliminated_5.annotations()
    # eliminated_annotations = (eliminated_5 + eliminated_prelast + eliminated_last).annotations()
    print_with_title("After eliminations", eliminated)
    # print_with_title(
    #     "After eliminations, without",
    #     eliminated.without_annotations().filtered_obj(lambda word: 8 not in set(word)))
    # print_with_title("After eliminations, filtered", words_with_n_distinct_chars(eliminated.without_annotations(), 5))
    print_with_title("Elimination expression", eliminated_annotations)
    assert eliminated == Linear()
    elimination_expr = Linear()
    for annotation, coeff in eliminated_annotations.items():
        elimination_expr += coeff * annotation_to_func(annotation)
    # print_with_title("Elimination expression reconstructed", elimination_expr.annotations())
    goal_raw += elimination_expr

print(f"\n\n=== Result ===\n")
print_with_title("LHS", goal_orig.annotations())
print_with_title("RHS", (goal_orig - goal_raw).annotations())

print("\nVerifying... ", end="")
t = Tensor(goal_raw.without_annotations())
t.convert_to_lyndon_basis()
assert t.summands == Linear()
print("done")
