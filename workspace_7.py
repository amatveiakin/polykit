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


for weight in range(3,9):
    sign = (-1) ** weight
    goal_raw = (
        + Li(weight, [1,2,3,4,5,6], projector=project_on_x1)
        + sign * Li(weight, [2,3,4,5,6,1], projector=project_on_x1)
    )

    expr_raw = (
        + Li(weight, [1,2,3,4], projector=project_on_x1)
        - Li(weight, [1,2,3,6], projector=project_on_x1)
        + Li(weight, [1,2,5,6], projector=project_on_x1)
        - Li(weight, [1,4,5,6], projector=project_on_x1)
        + Li(weight, [2,3,4,5], projector=project_on_x1)
        + Li(weight, [3,4,5,6], projector=project_on_x1)
    )

    print(f"Weight {weight}:")
    print(to_lyndon_basis((goal_raw - expr_raw).without_annotations()))
