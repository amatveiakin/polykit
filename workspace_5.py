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


e = (
(
 + Li5(1,2,3,4,5,6)
 - Li5(2,3,4,5,6,1)
)
-
(
 + Li5(1,2,3,4)
 - Li5(1,2,3,6)
 + Li5(1,2,5,6)
 - Li5(1,4,5,6)
 - Li5(2,3,4,5)
 + Li5(3,4,5,6)
)
)
print(to_lyndon_basis(e.without_annotations()))
exit()


weight = 5
known_eliminations = {}
find_eliminations(known_eliminations, project_on_x1(
    + Li5(1,2,3,4)
    ,
    keep_annotations=True,
), 4, weight, projection_axis=1)
# find_eliminations(known_eliminations, project_on_x1(
#     + Li5(1,2,3,4)
#     + Li5(1,3,2,4)
#     + Li5(1,3,4,2)
#     ,
#     keep_annotations=True,
# ), 4, weight, projection_axis=1)
add_elimination(known_eliminations, project_on_x1(
    + Li5(1,2,3,4)
    - Li5(1,Inf,3,4)
    - Li5(1,2,Inf,4)
    - Li5(1,2,3,Inf)
    ,
    keep_annotations=True,
), projection_axis=1)

print("Known eliminations:")
for k, v in known_eliminations.items():
    print(f"{k}\nvia\n{v[1]}\n")
print()


goal_orig = (
    + Li5(1,2,3,4,5,6)
    - Li5(2,3,4,5,6,1)
)

goal_raw = goal_orig.copy()
for axis in range(1, 5):
    print(f"\n\n=== Projecting on x{axis} ===\n")
    goal_before_lyndon = project_on_xi(goal_raw, axis)
    goal = to_lyndon_basis(goal_before_lyndon)
    print_with_title("Goal functional expr", goal_raw.annotations())
    # print_with_title("Goal before Lyndon", goal_before_lyndon)
    print_with_title("Goal", goal)

    eliminated = apply_eliminations(known_eliminations, goal, 7, 3, axis)
    print_with_title("After eliminations", eliminated.without_annotations())
    print_with_title("Elimination expression", eliminated.annotations())
    assert eliminated.without_annotations() == Linear()
    elimination_expr = Linear()
    for annotation, coeff in eliminated.annotations().items():
        elimination_expr += coeff * annotation_to_func(annotation)
    # print_with_title("Elimination expression reconstructed", elimination_expr.annotations())
    goal_raw += elimination_expr

t = Tensor(goal_raw.without_annotations())
t.convert_to_lyndon_basis()
assert t.summands == Linear()
print(f"\n\n=== Result ===\n")
print_with_title("LHS", goal_orig.annotations())
print_with_title("RHS", (goal_orig - goal_raw).annotations())
