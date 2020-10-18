from dataclasses import dataclass

from linear import Linear, tensor_product
from tensor import D, d_expr_substitute
from util import args_to_iterable, rotate_list


def X(a, b):
    d = D(a, b)
    return Linear() if d.is_nil() else Linear({(d,): 1})


# def cross_ratio(a, b, c, d):
#     return X(a, b) + X(c, d) - X(a, d) - X(c, b)

# def cross_ratio_6(a, b, c, d, e, f):
#     return X(a, b) + X(c, d) + X(e, f) - X(b, c) - X(d, e) - X(a, f)

def cross_ratio(*indexed_points):
    v = args_to_iterable(indexed_points)
    ret = Linear()
    n = len(v)
    for i in range(n):
        sign = 1 if i % 2 == 0 else -1
        ret += sign * X(v[i], v[(i+1)%n])
    return ret

# Returns
#   1 - cross_ratio(a, b, c, d)
# as symbol
def neg_cross_ratio(a, b, c, d):
    return cross_ratio(a, c, b, d)


def symbol_product(*multipliers):
    return tensor_product(multipliers, product=lambda a, b: a + b)


_li_cache = {}

# Generates a polylog of a given weight on a given set of points.
# Points can be a list of points or a number, in which case values
# from 1 to points are used.
def Li(weight, points):
    assert isinstance(points, (int, list, tuple))
    num_points = points if isinstance(points, int) else len(points)
    cache_key = (weight, num_points)
    asc_indices = list(range(1, num_points + 1))
    point_indices = asc_indices if isinstance(points, int) else points
    index_map = {
        asc_indices[i]: point_indices[i]
        for i in range(num_points)
    }
    asc_expr = None
    # print(f"Generating Li{weight}({num_points})... ", end="")
    if cache_key in _li_cache:
        asc_expr = _li_cache[cache_key].copy()
        # print("done (cached)")
    else:
        asc_expr = _Li_impl(weight, asc_indices)
        _li_cache[cache_key] = asc_expr.copy()
        # print("done")
    return d_expr_substitute(
        asc_expr,
        index_map
    ).annotated(
        f"Li{weight}(" + ",".join([str(p) for p in point_indices]) + ")"
    )


def Li2(*points):
    return Li(2, args_to_iterable(points))

def Li3(*points):
    return Li(3, args_to_iterable(points))

def Li4(*points):
    return Li(4, args_to_iterable(points))

def Li5(*points):
    return Li(5, args_to_iterable(points))

def Li6(*points):
    return Li(6, args_to_iterable(points))

def Li7(*points):
    return Li(7, args_to_iterable(points))

def Li8(*points):
    return Li(8, args_to_iterable(points))


@dataclass(frozen=True)
class Point:
    index: int
    point: int

def _Li_4_point(points):
    assert len(points) == 4
    return (
        neg_cross_ratio(*points)
        if points[0] % 2 == 1 else
        -neg_cross_ratio(*rotate_list(points, 1))
    )

def _Li_impl(weight, points):
    num_points = len(points)
    assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
    min_weight = (num_points - 2) // 2
    assert weight >= min_weight, f"Weight {weight} is less than minimum weight {min_weight}"
    def subsums():
        ret = Linear()
        for i in range(num_points - 3):
            ret += symbol_product(
                _Li_4_point(points[i:i+4]),
                _Li_impl(weight - 1, points[:i+1] + points[i+3:]),
            )
        return ret
    if weight == min_weight:
        if num_points == 4:
            return _Li_4_point(points)
        else:
            return subsums()
    else:
        ret = symbol_product(
            cross_ratio(points),
            _Li_impl(weight - 1, points),
        )
        if num_points > 4:
            ret += subsums()
        return ret
