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


for weight in range(3,9):
    sign = (-1) ** weight
    goal_raw = (
        + Li(weight, [1,2,3,4,5,6])
        + sign * Li(weight, [2,3,4,5,6,1])
    )

    expr_raw = (
        + Li(weight, [1,2,3,4])
        + sign * Li(weight, [2,3,4,5])
        + Li(weight, [3,4,5,6])
        + sign * Li(weight, [4,5,6,1])
        + Li(weight, [5,6,1,2])
        + sign * Li(weight, [6,1,2,3])
    )

    print(f"Weight {weight} confirmed = ", end="")
    confirmed = is_zero(goal_raw - expr_raw)
    print(confirmed)
    if confirmed:
        print(f"{goal_raw.annotations()}\n=\n{expr_raw.annotations()}\n")
