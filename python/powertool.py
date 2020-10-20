from collections.abc import Callable

from linear import Linear
from util import rotate_list


# def cyclic_sum_range(
#         func: Callable,
#         points: int,
#         num_args: int,
#         *,
#         alternating: bool = False
#     ) -> Linear:
#     ret = Linear()
#     for i in range(len(points)):
#         sign = (-1)**i if alternating else 1
#         ret += sign * func(*rotate_list(points, i)[:num_args])
#     return ret

def cyclic_sum(
        func: Callable,
        num_points: int,
        num_args: int,
        *,
        alternating: bool = False
    ) -> Linear:
    asc = list(range(1, num_points + 1))
    ret = Linear()
    for i in range(num_points):
        sign = (-1)**i if alternating else 1
        ret += sign * func(*rotate_list(asc, i)[:num_args])
    return ret
