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


known_eliminations = {}
find_eliminations(known_eliminations, project_on_x1(
    + Li4(1,2,3,4)
    ,
    keep_annotations=True,
), 4, 4, projection_axis=1)
find_eliminations(known_eliminations, project_on_x1(
    + Li4(1,2,3,4)
    + Li4(1,3,2,4)
    + Li4(1,3,4,2)
    ,
    keep_annotations=True,
), 4, 4, projection_axis=1)
add_elimination(known_eliminations, project_on_x1(
    + Li4(1,2,3,4)
    - Li4(1,Inf,3,4)
    - Li4(1,2,Inf,4)
    - Li4(1,2,3,Inf)
    ,
    keep_annotations=True,
), projection_axis=1)

print("Known eliminations:")
for k, v in known_eliminations.items():
    print(f"{k}\nvia\n{v[1]}\n")
print()


goal_orig = Linear()
for i in range(1,8):
    goal_orig += Li4(rotate_list(list(range(1,8)), i)[:6])

goal_raw = goal_orig.copy()
for axis in range(1, 6):
    print(f"\n\n=== Projecting on x{axis} ===\n")
    goal_before_lyndon = project_on_xi(goal_raw, axis)
    goal = to_lyndon_basis(goal_before_lyndon)
    format.print_expression("Goal functional expr", goal_raw.annotations())
    # format.print_expression("Goal before Lyndon", goal_before_lyndon)
    format.print_expression("Goal", goal)

    eliminated = apply_eliminations(known_eliminations, goal, 7, 3, axis)
    format.print_expression("After eliminations", eliminated.without_annotations())
    format.print_expression("Elimination expression", eliminated.annotations())
    assert eliminated.without_annotations() == Linear()
    elimination_expr = Linear()
    for annotation, coeff in eliminated.annotations().items():
        elimination_expr += coeff * annotation_to_func(annotation)
    # format.print_expression("Elimination expression reconstructed", elimination_expr.annotations())
    goal_raw += elimination_expr

t = Tensor(goal_raw.without_annotations())
t.convert_to_lyndon_basis()
assert t.summands == Linear()
print(f"\n\n=== Result ===\n")
format.print_expression("LHS", goal_orig.annotations())
format.print_expression("RHS", (goal_orig - goal_raw).annotations())
