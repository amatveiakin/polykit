from dataclasses import dataclass

from linear import Linear, tensor_product
from tensor import D
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


# Generates a polylog of a given weight on a given set of points.
# Points can be a list of points or a number, in which case values
# from 1 to points are used.
def Li(weight, points):
    assert isinstance(points, (int, list, tuple))
    num_points = points if isinstance(points, int) else len(points)
    indexed_points = [
        Point(i+1, i+1 if isinstance(points, int) else points[i])
        for i in range(num_points)
    ]
    return _Li_impl(weight, indexed_points).annotated(
        f"Li{weight}(" + ",".join([str(p) for p in _point_values(indexed_points)]) + ")"
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

def _point_values(indexed_points):
    return [p.point for p in indexed_points]

def _Li_4_point(indexed_points):
    assert len(indexed_points) == 4
    points = _point_values(indexed_points)
    return (
        neg_cross_ratio(*points)
        if indexed_points[0].index % 2 == 1 else
        -neg_cross_ratio(*rotate_list(points, 1))
    )

def _Li_impl(weight, indexed_points):
    num_points = len(indexed_points)
    assert num_points >= 4 and num_points % 2 == 0, f"Bad number of points: {num_points}"
    min_weight = (num_points - 2) // 2
    assert weight >= min_weight, f"Weight {weight} is less than minimum weight {min_weight}"
    def subsums():
        ret = Linear()
        for i in range(num_points - 3):
            ret += symbol_product(
                _Li_4_point(indexed_points[i:i+4]),
                _Li_impl(weight - 1, indexed_points[:i+1] + indexed_points[i+3:]),
            )
        return ret
    if weight == min_weight:
        if num_points == 4:
            return _Li_4_point(indexed_points)
        else:
            return subsums()
    else:
        ret = symbol_product(
            cross_ratio(_point_values(indexed_points)),
            _Li_impl(weight - 1, indexed_points),
        )
        if num_points > 4:
            ret += subsums()
        return ret
