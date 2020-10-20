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


def print_expression(
        title: str,
        expr: Linear,
        element_to_str: Callable = None
    ):
    data = expr.without_annotations()
    if data != Linear():
        print(
            f"{title} - {len(data)} terms, |coeff| = {data.l1_norm()}:\n" +
            _print_expression_terms(data, element_to_str) + "\n"
        )
    else:
        print(
            f"{title} - {len(expr)} terms:\n" +
            _print_expression_terms(expr, element_to_str) + "\n"
        )

def _print_expression_terms(
        expr: Linear,
        element_to_str: Callable
    ):
    CUTOFF_THRESHOLD = 300
    CUTOFF_DISPLAY = 200
    assert CUTOFF_DISPLAY <= CUTOFF_THRESHOLD
    ret = expr.to_str(element_to_str or str)
    lines = ret.split("\n")
    if len(lines) > CUTOFF_THRESHOLD:
        lines = lines[:CUTOFF_DISPLAY]
        lines.append("...")
    return "\n".join(lines)
